#!/bin/bash

# 寻找有效登录凭据的脚本
BASE_URL="http://120.48.95.51:7001"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

echo "=== 寻找有效的登录凭据 ==="

# 常见的测试账号组合
declare -A TEST_ACCOUNTS=(
    ["admin@admin.com"]="admin"
    ["admin@example.com"]="admin123"
    ["test@test.com"]="test"
    ["test@example.com"]="test123"
    ["user@user.com"]="user"
    ["demo@demo.com"]="demo"
    ["guest@guest.com"]="guest"
    ["root@root.com"]="root"
    ["admin@test.com"]="123456"
    ["test@admin.com"]="password"
    ["user@demo.com"]="demo123"
    ["admin"]="admin"
    ["test"]="test"
    ["user"]="user"
)

# 常见密码列表
COMMON_PASSWORDS=("123456" "password" "admin" "test" "user" "demo" "123" "000000" "111111" "qwerty")

echo "正在测试常见账号组合..."

# 测试预定义的账号组合
for email in "${!TEST_ACCOUNTS[@]}"; do
    password="${TEST_ACCOUNTS[$email]}"
    
    echo -n "测试 $email : $password ... "
    
    response=$(curl -s -X POST "$BASE_URL/login" \
        -H "Content-Type: application/json" \
        -d "{\"email\":\"$email\",\"password\":\"$password\"}" \
        --connect-timeout 5 --max-time 10)
    
    code=$(echo "$response" | jq -r '.code // 401' 2>/dev/null)
    
    if [ "$code" = "0" ]; then
        echo "✅ 成功!"
        echo "有效凭据找到:"
        echo "  邮箱: $email"
        echo "  密码: $password"
        echo "完整响应:"
        echo "$response" | jq '.' 2>/dev/null || echo "$response"
        
        # 保存到文件
        echo "email=$email" > valid_credentials.txt
        echo "password=$password" >> valid_credentials.txt
        echo "token=$(echo "$response" | jq -r '.token')" >> valid_credentials.txt
        
        echo "凭据已保存到 valid_credentials.txt"
        exit 0
    else
        echo "❌"
    fi
done

# 如果预定义账号都失败，尝试从日志中提取可能的用户信息
echo -e "\n正在从系统日志中分析可能的用户信息..."

# 获取日志数据
logs=$(curl -s "$BASE_URL/system/logs/stats?limit=50" | jq -r '.rows[]? | select(.path == "/login") | .ip' 2>/dev/null | sort | uniq)

if [ -n "$logs" ]; then
    echo "发现以下IP地址曾经登录过:"
    echo "$logs"
    
    # 基于IP生成可能的邮箱
    for ip in $logs; do
        # 将IP转换为可能的用户名
        username=$(echo $ip | tr '.' '_')
        possible_emails=(
            "user_${username}@test.com"
            "test_${username}@example.com"
            "${username}@demo.com"
        )
        
        for email in "${possible_emails[@]}"; do
            for password in "${COMMON_PASSWORDS[@]}"; do
                echo -n "尝试 $email : $password ... "
                
                response=$(curl -s -X POST "$BASE_URL/login" \
                    -H "Content-Type: application/json" \
                    -d "{\"email\":\"$email\",\"password\":\"$password\"}" \
                    --connect-timeout 3 --max-time 5)
                
                code=$(echo "$response" | jq -r '.code // 401' 2>/dev/null)
                
                if [ "$code" = "0" ]; then
                    echo "✅ 成功!"
                    echo "有效凭据找到:"
                    echo "  邮箱: $email"
                    echo "  密码: $password"
                    echo "$response" | jq '.' 2>/dev/null || echo "$response"
                    
                    echo "email=$email" > valid_credentials.txt
                    echo "password=$password" >> valid_credentials.txt
                    echo "token=$(echo "$response" | jq -r '.token')" >> valid_credentials.txt
                    
                    exit 0
                else
                    echo "❌"
                fi
            done
        done
    done
fi

echo -e "\n❌ 未找到有效的登录凭据"
echo -e "\n💡 建议:"
echo "1. 联系服务器管理员获取测试账号"
echo "2. 检查是否需要先注册账号"
echo "3. 确认服务器是否启用了用户认证"

# 测试注册接口是否可用
echo -e "\n正在测试注册接口..."
test_user="test_$(date +%s)"
register_response=$(curl -s -X POST "$BASE_URL/register" \
    -H "Content-Type: application/json" \
    -d "{\"username\":\"$test_user\",\"email\":\"${test_user}@test.com\",\"password\":\"test123\"}")

register_code=$(echo "$register_response" | jq -r '.code // 400' 2>/dev/null)

if [ "$register_code" = "0" ]; then
    echo "✅ 注册接口可用，已创建测试账号:"
    echo "  邮箱: ${test_user}@test.com"
    echo "  密码: test123"
    echo "$register_response" | jq '.' 2>/dev/null || echo "$register_response"
    
    # 尝试用新注册的账号登录
    echo -e "\n尝试用新账号登录..."
    login_response=$(curl -s -X POST "$BASE_URL/login" \
        -H "Content-Type: application/json" \
        -d "{\"email\":\"${test_user}@test.com\",\"password\":\"test123\"}")
    
    login_code=$(echo "$login_response" | jq -r '.code // 401' 2>/dev/null)
    
    if [ "$login_code" = "0" ]; then
        echo "✅ 新账号登录成功!"
        echo "email=${test_user}@test.com" > valid_credentials.txt
        echo "password=test123" >> valid_credentials.txt
        echo "token=$(echo "$login_response" | jq -r '.token')" >> valid_credentials.txt
        echo "凭据已保存到 valid_credentials.txt"
    else
        echo "❌ 新账号登录失败"
    fi
else
    echo "❌ 注册接口不可用或需要额外参数"
    echo "响应: $register_response"
fi