#!/bin/bash

# 改进的API测试脚本 - 使用正确的参数
BASE_URL="http://120.48.95.51:7001"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

# 颜色输出
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo "=== 改进的API测试 (使用正确参数) ==="

# 1. 测试无需认证的接口
echo -e "${GREEN}1. 测试系统接口 (无需认证)${NC}"

echo "系统信息:"
curl -s "$BASE_URL/system/info" | jq '{
  cpu_usage: .data.cpu_usage,
  mem_usage: .data.mem_usage,
  disk_usage: .data.disk_usage,
  uptime_days: .data.uptime_days,
  os_info: .data.os_info
}' 2>/dev/null

echo -e "\n系统日志统计 (最近5条):"
curl -s "$BASE_URL/system/logs/stats?limit=5" | jq '{
  total: .total,
  avgDuration: .avgDurationMs,
  recentRequests: .rows[0:3] | map({path, method, platform, durationMs})
}' 2>/dev/null

# 2. 尝试不同的登录凭据
echo -e "\n${GREEN}2. 测试登录接口 (尝试不同凭据)${NC}"

# 常见的测试账号
TEST_ACCOUNTS=(
    '{"email":"admin@example.com","password":"admin123"}'
    '{"email":"test@example.com","password":"test123"}'
    '{"email":"user@test.com","password":"123456"}'
    '{"email":"demo@demo.com","password":"demo123"}'
)

TOKEN=""
for account in "${TEST_ACCOUNTS[@]}"; do
    echo "尝试登录: $(echo $account | jq -r '.email')"
    
    response=$(curl -s -X POST "$BASE_URL/login" \
        -H "Content-Type: application/json" \
        -d "$account")
    
    code=$(echo "$response" | jq -r '.code // 401')
    
    if [ "$code" = "0" ]; then
        echo -e "${GREEN}✅ 登录成功!${NC}"
        echo "$response" | jq '.'
        TOKEN=$(echo "$response" | jq -r '.token')
        break
    else
        echo -e "${YELLOW}❌ 登录失败: $(echo "$response" | jq -r '.msg // "Unknown error"')${NC}"
    fi
done

# 3. 如果有Token，测试需要认证的接口
if [ -n "$TOKEN" ] && [ "$TOKEN" != "null" ]; then
    echo -e "\n${GREEN}3. 测试需要认证的接口${NC}"
    echo "使用Token: ${TOKEN:0:20}..."
    
    # 设置日期参数
    START_DATE="2026-01-01"
    END_DATE="2026-01-18"
    
    echo -e "\n分析趋势 (带正确参数):"
    curl -s "$BASE_URL/api/analytics/trends" \
        -G \
        --data-urlencode "startDate=$START_DATE" \
        --data-urlencode "endDate=$END_DATE" \
        -H "Authorization: Bearer $TOKEN" | \
        jq '.' 2>/dev/null || echo "请求失败或返回非JSON数据"
    
    echo -e "\n页面访问统计 (带正确参数):"
    curl -s "$BASE_URL/api/analytics/page-views" \
        -G \
        --data-urlencode "startDate=$START_DATE" \
        --data-urlencode "endDate=$END_DATE" \
        -H "Authorization: Bearer $TOKEN" | \
        jq '.' 2>/dev/null || echo "请求失败或返回非JSON数据"
    
    echo -e "\n事件统计 (带正确参数):"
    curl -s "$BASE_URL/api/analytics/event-stats" \
        -G \
        --data-urlencode "startDate=$START_DATE" \
        --data-urlencode "endDate=$END_DATE" \
        -H "Authorization: Bearer $TOKEN" | \
        jq '.' 2>/dev/null || echo "请求失败或返回非JSON数据"
    
    echo -e "\n用户画像 (ID: 1):"
    curl -s "$BASE_URL/api/user-profile/1" \
        -H "Authorization: Bearer $TOKEN" | \
        jq '.' 2>/dev/null || echo "请求失败或返回非JSON数据"
        
else
    echo -e "\n${RED}❌ 无有效Token，跳过需要认证的接口测试${NC}"
    echo -e "${YELLOW}💡 提示: 请联系管理员获取正确的登录凭据${NC}"
fi

# 4. 性能测试
echo -e "\n${GREEN}4. 性能测试${NC}"
echo "测试系统信息接口响应时间 (5次):"

total_time=0
for i in {1..5}; do
    start=$(date +%s%N)
    curl -s "$BASE_URL/system/info" > /dev/null
    end=$(date +%s%N)
    duration=$(( (end - start) / 1000000 ))
    echo "请求 $i: ${duration}ms"
    total_time=$((total_time + duration))
done

avg_time=$((total_time / 5))
echo "平均响应时间: ${avg_time}ms"

# 5. 生成可用接口列表
echo -e "\n${GREEN}5. 可用接口总结${NC}"
echo "✅ 确认可用的接口:"
echo "  GET  /system/info - 系统信息"
echo "  GET  /system/logs/stats - 日志统计"
echo "  POST /login - 用户登录"

if [ -n "$TOKEN" ] && [ "$TOKEN" != "null" ]; then
    echo "  GET  /api/analytics/trends - 分析趋势 (需要startDate, endDate)"
    echo "  GET  /api/analytics/page-views - 页面统计 (需要startDate, endDate)"
    echo "  GET  /api/analytics/event-stats - 事件统计 (需要startDate, endDate)"
    echo "  GET  /api/user-profile/{id} - 用户画像"
fi

echo -e "\n${GREEN}测试完成!${NC}"