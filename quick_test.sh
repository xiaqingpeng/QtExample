#!/bin/bash

# 快速API测试脚本
BASE_URL="http://120.48.95.51:7001"

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

echo "=== 快速API测试 ==="

# 1. 测试系统日志接口 (已验证可用)
echo "1. 测试系统日志统计..."
curl -s "$BASE_URL/system/logs/stats" | jq '{
  total: .total,
  avgDuration: .avgDurationMs,
  recentRequests: .rows[0:3] | map({path, method, platform, durationMs})
}' 2>/dev/null || curl -s "$BASE_URL/system/logs/stats"

echo -e "\n2. 测试登录接口..."
LOGIN_RESPONSE=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"626143872@qq.com","password":"123456"}')

echo "$LOGIN_RESPONSE" | jq '.' 2>/dev/null || echo "$LOGIN_RESPONSE"

# 提取Token
TOKEN=$(echo "$LOGIN_RESPONSE" | jq -r '.token // empty' 2>/dev/null)

if [ -n "$TOKEN" ] && [ "$TOKEN" != "null" ] && [ "$TOKEN" != "empty" ]; then
    echo -e "\n✅ 登录成功，Token: ${TOKEN:0:20}..."
    
    echo -e "\n3. 测试分析接口..."
    # 获取最近7天的数据
    START_DATE=$(date -u -v-7d +%Y-%m-%d 2>/dev/null || date -u -d '7 days ago' +%Y-%m-%d)
    END_DATE=$(date -u +%Y-%m-%d)
    
    curl -s "$BASE_URL/api/analytics/trends?startDate=$START_DATE&endDate=$END_DATE" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "分析接口响应获取中..."
    
    echo -e "\n4. 测试用户标签接口..."
    USER_ID=$(echo "$LOGIN_RESPONSE" | jq -r '.data.id // 2')
    curl -s "$BASE_URL/api/analytics/user/tags?userId=$USER_ID" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.' 2>/dev/null || echo "用户标签接口响应获取中..."
      
    echo -e "\n5. 测试事件统计接口..."
    curl -s "$BASE_URL/api/analytics/event-stats?startDate=$START_DATE&endDate=$END_DATE" \
      -H "Authorization: Bearer $TOKEN" | \
      jq '.data[0:3]' 2>/dev/null || echo "事件统计接口响应获取中..."
      
else
    echo "❌ 登录失败，跳过需要认证的接口"
fi

echo -e "\n=== 测试完成 ==="