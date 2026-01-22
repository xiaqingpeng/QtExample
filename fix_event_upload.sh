#!/bin/bash

# 修复事件上报接口测试
BASE_URL="http://120.48.95.51:7001"
EMAIL="626143872@qq.com"
PASSWORD="123456"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

echo "=== 修复事件上报接口测试 ==="

# 1. 登录获取Token
echo "1. 登录获取Token..."
LOGIN_RESPONSE=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d "{\"email\":\"$EMAIL\",\"password\":\"$PASSWORD\"}")

TOKEN=$(echo "$LOGIN_RESPONSE" | jq -r '.token // empty' 2>/dev/null)

if [ -z "$TOKEN" ] || [ "$TOKEN" = "null" ]; then
    echo "❌ 登录失败，无法测试事件上报"
    exit 1
fi

echo "✅ 登录成功，Token: ${TOKEN:0:20}..."

# 2. 测试不同的事件上报格式

echo -e "\n2. 测试事件上报格式..."

# 格式1: 简单事件格式
echo "测试格式1 - 简单事件:"
SIMPLE_EVENT='{
  "events": [
    {
      "event_name": "test_event",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "test": true
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$SIMPLE_EVENT" | jq '.'

# 格式2: 完整事件格式
echo -e "\n测试格式2 - 完整事件:"
FULL_EVENT='{
  "app_id": "qt-example-app",
  "events": [
    {
      "event_name": "login_success",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "method": "email",
        "success": true
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$FULL_EVENT" | jq '.'

# 格式3: 带设备信息的事件
echo -e "\n测试格式3 - 带设备信息:"
DEVICE_EVENT='{
  "app_id": "qt-example-app",
  "device_info": {
    "platform": "desktop",
    "os": "'$(uname -s)'",
    "version": "1.0.0"
  },
  "events": [
    {
      "event_name": "page_view",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "page": "test_page",
        "duration": 120
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$DEVICE_EVENT" | jq '.'

# 格式4: 多个事件
echo -e "\n测试格式4 - 多个事件:"
MULTI_EVENTS='{
  "events": [
    {
      "event_name": "button_click",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "button_id": "test_button",
        "page": "test_page"
      }
    },
    {
      "event_name": "form_submit",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "form_type": "contact",
        "success": true
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$MULTI_EVENTS" | jq '.'

# 3. 测试错误格式
echo -e "\n3. 测试错误格式 (用于理解错误信息):"

# 缺少event_name的事件
echo "测试缺少event_name:"
BAD_EVENT='{
  "events": [
    {
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "test": true
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$BAD_EVENT" | jq '.'

# 空的event_name
echo -e "\n测试空的event_name:"
EMPTY_NAME_EVENT='{
  "events": [
    {
      "event_name": "",
      "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
      "properties": {
        "test": true
      }
    }
  ]
}'

curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "$EMPTY_NAME_EVENT" | jq '.'

echo -e "\n=== 测试完成 ==="
echo "💡 根据测试结果，找出正确的事件上报格式"