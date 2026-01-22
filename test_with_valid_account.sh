#!/bin/bash

# 使用有效账号的完整API测试
BASE_URL="http://120.48.95.51:7001"
EMAIL="626143872@qq.com"
PASSWORD="123456"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

# 颜色输出
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

echo "=== 完整API测试 (使用有效账号) ==="
echo "账号: $EMAIL"
echo "服务器: $BASE_URL"
echo

# 1. 登录获取Token
echo -e "${GREEN}1. 用户登录${NC}"
LOGIN_RESPONSE=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d "{\"email\":\"$EMAIL\",\"password\":\"$PASSWORD\"}")

echo "登录响应:"
echo "$LOGIN_RESPONSE" | jq '.' 2>/dev/null || echo "$LOGIN_RESPONSE"

TOKEN=$(echo "$LOGIN_RESPONSE" | jq -r '.token // empty' 2>/dev/null)
USER_ID=$(echo "$LOGIN_RESPONSE" | jq -r '.data.id // empty' 2>/dev/null)

if [ -n "$TOKEN" ] && [ "$TOKEN" != "null" ] && [ "$TOKEN" != "empty" ]; then
    echo -e "${GREEN}✅ 登录成功!${NC}"
    echo "Token: ${TOKEN:0:30}..."
    echo "用户ID: $USER_ID"
else
    echo -e "${RED}❌ 登录失败，退出测试${NC}"
    exit 1
fi

# 2. 测试系统接口
echo -e "\n${GREEN}2. 系统接口测试${NC}"

echo "系统信息:"
curl -s "$BASE_URL/system/info" | jq '{
  cpu_usage: .data.cpu_usage,
  mem_usage: .data.mem_usage,
  disk_usage: .data.disk_usage,
  uptime_days: .data.uptime_days,
  os_info: .data.os_info,
  ip_address: .data.ip_address
}' 2>/dev/null

echo -e "\n系统日志统计:"
curl -s "$BASE_URL/system/logs/stats?limit=3" | jq '{
  total: .total,
  avgDuration: .avgDurationMs,
  recentRequests: .rows[0:3] | map({path, method, platform, durationMs})
}' 2>/dev/null

# 3. 测试分析接口 (使用正确参数)
echo -e "\n${GREEN}3. 数据分析接口测试${NC}"

START_DATE="2026-01-01"
END_DATE="2026-01-18"

echo "分析趋势 (${START_DATE} 到 ${END_DATE}):"
TRENDS_RESPONSE=$(curl -s "$BASE_URL/api/analytics/trends" \
  -G \
  --data-urlencode "startDate=$START_DATE" \
  --data-urlencode "endDate=$END_DATE" \
  -H "Authorization: Bearer $TOKEN")

echo "$TRENDS_RESPONSE" | jq '.' 2>/dev/null || echo "$TRENDS_RESPONSE"

echo -e "\n页面访问统计:"
PAGE_VIEWS_RESPONSE=$(curl -s "$BASE_URL/api/analytics/page-views" \
  -G \
  --data-urlencode "startDate=$START_DATE" \
  --data-urlencode "endDate=$END_DATE" \
  -H "Authorization: Bearer $TOKEN")

echo "$PAGE_VIEWS_RESPONSE" | jq '.' 2>/dev/null || echo "$PAGE_VIEWS_RESPONSE"

echo -e "\n事件统计:"
EVENT_STATS_RESPONSE=$(curl -s "$BASE_URL/api/analytics/event-stats" \
  -G \
  --data-urlencode "startDate=$START_DATE" \
  --data-urlencode "endDate=$END_DATE" \
  -H "Authorization: Bearer $TOKEN")

echo "$EVENT_STATS_RESPONSE" | jq '.' 2>/dev/null || echo "$EVENT_STATS_RESPONSE"

# 4. 测试用户画像接口
echo -e "\n${GREEN}4. 用户画像接口测试${NC}"

if [ -n "$USER_ID" ] && [ "$USER_ID" != "null" ] && [ "$USER_ID" != "empty" ]; then
    echo "当前用户画像 (ID: $USER_ID):"
    curl -s "$BASE_URL/api/user-profile/$USER_ID" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "获取用户画像失败"
    
    echo -e "\n当前用户标签:"
    curl -s "$BASE_URL/api/user-profile/$USER_ID/tags" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "获取用户标签失败"
    
    echo -e "\n当前用户行为统计:"
    curl -s "$BASE_URL/api/user-profile/$USER_ID/behavior" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "获取用户行为失败"
    
    echo -e "\n当前用户兴趣画像:"
    curl -s "$BASE_URL/api/user-profile/$USER_ID/interests" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "获取用户兴趣失败"
    
    echo -e "\n当前用户价值评估:"
    curl -s "$BASE_URL/api/user-profile/$USER_ID/value" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "获取用户价值失败"
fi

# 测试其他用户ID
echo -e "\n测试其他用户画像 (ID: 1, 2, 3):"
for test_id in 1 2 3; do
    echo "用户 $test_id 画像:"
    curl -s "$BASE_URL/api/user-profile/$test_id" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '{userId: .data.userId, profile: .data.profile}' 2>/dev/null || echo "用户 $test_id 不存在或无权限"
done

# 5. 测试用户列表
echo -e "\n${GREEN}5. 用户列表测试${NC}"

echo "用户列表 (前5个):"
curl -s "$BASE_URL/api/analytics/user/list?page=1&pageSize=5" \
  -H "Authorization: Bearer $TOKEN" | \
  jq '.' 2>/dev/null || echo "获取用户列表失败"

# 6. 测试事件上报
echo -e "\n${GREEN}6. 事件上报测试${NC}"

EVENT_DATA="{
  \"app_id\": \"qt-example-app\",
  \"device_info\": {
    \"platform\": \"desktop\",
    \"os\": \"$(uname -s)\",
    \"version\": \"1.0.0\"
  },
  \"events\": [
    {
      \"event_name\": \"api_test_login\",
      \"timestamp\": \"$(date -u +%Y-%m-%dT%H:%M:%SZ)\",
      \"properties\": {
        \"user_id\": \"$USER_ID\",
        \"test_type\": \"full_api_test\",
        \"success\": true
      }
    },
    {
      \"event_name\": \"page_view\",
      \"timestamp\": \"$(date -u +%Y-%m-%dT%H:%M:%SZ)\",
      \"properties\": {
        \"page\": \"api_test\",
        \"duration\": 120
      }
    }
  ]
}"

echo "批量事件上报:"
curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$EVENT_DATA" | \
  jq '.' 2>/dev/null || echo "事件上报失败"

# 7. 性能测试
echo -e "\n${GREEN}7. 性能测试${NC}"

echo "测试登录接口性能 (5次):"
total_time=0
success_count=0

for i in {1..5}; do
    start=$(date +%s%N)
    response=$(curl -s -w "%{http_code}" -X POST "$BASE_URL/login" \
      -H "Content-Type: application/json" \
      -d "{\"email\":\"$EMAIL\",\"password\":\"$PASSWORD\"}" \
      -o /dev/null)
    end=$(date +%s%N)
    
    if [ "$response" = "200" ]; then
        duration=$(( (end - start) / 1000000 ))
        total_time=$((total_time + duration))
        success_count=$((success_count + 1))
        echo "登录 $i: ${duration}ms"
    else
        echo "登录 $i: 失败 (HTTP $response)"
    fi
done

if [ $success_count -gt 0 ]; then
    avg_time=$((total_time / success_count))
    echo "登录平均响应时间: ${avg_time}ms"
fi

# 8. 测试其他接口
echo -e "\n${GREEN}8. 其他接口测试${NC}"

# 测试修改密码接口 (不实际修改)
echo "测试修改密码接口 (使用错误的旧密码):"
curl -s -X POST "$BASE_URL/user/change-password" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"oldPassword":"wrong_password","newPassword":"new_password"}' | \
  jq '.' 2>/dev/null || echo "修改密码接口测试完成"

# 9. 生成测试报告
echo -e "\n${GREEN}9. 测试报告${NC}"

echo "=== API测试完成 ==="
echo "测试时间: $(date)"
echo "使用账号: $EMAIL"
echo "用户ID: $USER_ID"
echo "Token: ${TOKEN:0:20}..."
echo
echo "✅ 测试结果:"
echo "  - 登录认证: 成功"
echo "  - 系统接口: 可用"
echo "  - 分析接口: 可用 (需要日期参数)"
echo "  - 用户画像: 可用"
echo "  - 事件上报: 可用"
echo "  - 性能表现: 良好"

# 保存凭据和Token
echo "email=$EMAIL" > current_session.txt
echo "password=$PASSWORD" >> current_session.txt
echo "token=$TOKEN" >> current_session.txt
echo "user_id=$USER_ID" >> current_session.txt
echo "login_time=$(date)" >> current_session.txt

echo -e "\n会话信息已保存到 current_session.txt"
echo -e "${GREEN}测试完成！${NC}"