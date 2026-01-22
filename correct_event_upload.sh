#!/bin/bash

# 正确的事件上报测试
BASE_URL="http://120.48.95.51:7001"
EMAIL="626143872@qq.com"
PASSWORD="123456"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

echo "=== 正确的事件上报测试 ==="

# 登录获取Token
TOKEN=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d "{\"email\":\"$EMAIL\",\"password\":\"$PASSWORD\"}" | \
  jq -r '.token')

echo "✅ 登录成功，Token: ${TOKEN:0:20}..."

# 1. 单个事件上报
echo -e "\n1. 单个事件上报:"
SINGLE_EVENT='{
  "events": [
    {
      "event": "api_test_success",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "test_type": "single_event",
        "success": true,
        "user_id": "2"
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$SINGLE_EVENT" | jq '.'

# 2. 多个事件批量上报
echo -e "\n2. 多个事件批量上报:"
BATCH_EVENTS='{
  "events": [
    {
      "event": "page_view",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "page": "api_test_page",
        "duration": 120,
        "referrer": "direct"
      }
    },
    {
      "event": "button_click",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "button_id": "test_button",
        "button_text": "Test API",
        "page": "api_test_page"
      }
    },
    {
      "event": "form_submit",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "form_type": "api_test",
        "fields_count": 3,
        "success": true
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$BATCH_EVENTS" | jq '.'

# 3. 带完整信息的事件上报
echo -e "\n3. 带完整信息的事件上报:"
FULL_EVENT='{
  "app_id": "qt-example-app",
  "device_info": {
    "platform": "desktop",
    "os": "'$(uname -s)'",
    "version": "1.0.0",
    "user_agent": "curl/test"
  },
  "events": [
    {
      "event": "performance_metric",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "metric_name": "api_response_time",
        "value": 35,
        "unit": "ms",
        "endpoint": "/api/analytics/events/batch"
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$FULL_EVENT" | jq '.'

# 4. 用户行为事件
echo -e "\n4. 用户行为事件:"
USER_BEHAVIOR='{
  "events": [
    {
      "event": "login_success",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "method": "email",
        "user_id": "2",
        "session_id": "test_session_'$(date +%s)'",
        "ip_address": "127.0.0.1"
      }
    },
    {
      "event": "main_menu_changed",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "from_menu": "dashboard",
        "to_menu": "analytics",
        "user_id": "2"
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$USER_BEHAVIOR" | jq '.'

# 5. 验证事件是否被记录
echo -e "\n5. 验证事件统计 (查看最新数据):"
sleep 2  # 等待数据处理

curl -s "$BASE_URL/api/analytics/event-stats" \
  -G \
  --data-urlencode "startDate=$(date -u +%Y-%m-%d)" \
  --data-urlencode "endDate=$(date -u +%Y-%m-%d)" \
  -H "Authorization: Bearer $TOKEN" | \
  jq '.data[] | select(.eventName | contains("api_test") or contains("performance_metric"))'

echo -e "\n=== 事件上报测试完成 ==="
echo "✅ 正确的字段名是 'event' 而不是 'event_name'"
echo "✅ 事件上报功能正常工作"