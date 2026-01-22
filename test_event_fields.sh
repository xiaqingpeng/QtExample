#!/bin/bash

# 测试不同的事件字段名称
BASE_URL="http://120.48.95.51:7001"
EMAIL="626143872@qq.com"
PASSWORD="123456"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

echo "=== 测试不同的事件字段名称 ==="

# 登录获取Token
TOKEN=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d "{\"email\":\"$EMAIL\",\"password\":\"$PASSWORD\"}" | \
  jq -r '.token')

echo "Token: ${TOKEN:0:20}..."

# 测试不同的字段名称
echo -e "\n测试不同的事件字段名称:"

# 1. eventName (驼峰命名)
echo "1. 测试 eventName:"
curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "events": [
      {
        "eventName": "test_event",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "test": true
        }
      }
    ]
  }' | jq '.'

# 2. name
echo -e "\n2. 测试 name:"
curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "events": [
      {
        "name": "test_event",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "test": true
        }
      }
    ]
  }' | jq '.'

# 3. event
echo -e "\n3. 测试 event:"
curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "events": [
      {
        "event": "test_event",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "test": true
        }
      }
    ]
  }' | jq '.'

# 4. type
echo -e "\n4. 测试 type:"
curl -s -X POST "$BASE_URL/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "events": [
      {
        "type": "test_event",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "test": true
        }
      }
    ]
  }' | jq '.'

# 5. 尝试查看现有的事件格式
echo -e "\n5. 查看现有事件的格式 (从日志中学习):"
curl -s "$BASE_URL/system/logs/stats?limit=10" | \
  jq '.rows[] | select(.path == "/api/analytics/events/batch")'

echo -e "\n=== 测试完成 ==="