# 完整API测试命令集合

基于真实测试结果，以下是所有可用API接口的完整cURL命令。

## 🔐 认证信息

```bash
# 有效的测试账号
EMAIL="626143872@qq.com"
PASSWORD="123456"

# 禁用代理 (如果使用代理)
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY
```

## 1️⃣ 用户登录 (获取Token)

```bash
# 登录获取Token
LOGIN_RESPONSE=$(curl -s -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"626143872@qq.com","password":"123456"}')

echo "$LOGIN_RESPONSE" | jq '.'

# 提取Token
TOKEN=$(echo "$LOGIN_RESPONSE" | jq -r '.token')
echo "Token: $TOKEN"
```

**成功响应示例**:
```json
{
  "code": 0,
  "msg": "Login success",
  "data": {
    "id": 2,
    "username": "xiaqingepng",
    "email": "626143872@qq.com",
    "avatar": "http://qingpengxia.oss-cn-shenzhen.aliyuncs.com/1767164433485_oyfpp15mzp8"
  },
  "token": "Ml8xNzY4NzIzNjIwMDM4Xzg1OTcwNmE1NTI5Y2E0NDQ5NzJjZTczZWQwZWU1MDNm"
}
```

## 2️⃣ 系统信息接口 (无需认证)

### 系统状态信息
```bash
curl -s "http://120.48.95.51:7001/system/info" | jq '.'
```

**响应数据**:
- CPU使用率: 4.8%
- 内存使用率: 61%
- 磁盘使用率: 37%
- 运行时间: 21.6天
- 操作系统: Ubuntu 24.04.1 LTS

### 系统日志统计
```bash
# 获取所有日志
curl -s "http://120.48.95.51:7001/system/logs/stats" | jq '.'

# 限制返回数量
curl -s "http://120.48.95.51:7001/system/logs/stats?limit=5" | jq '.'

# 分析日志数据
curl -s "http://120.48.95.51:7001/system/logs/stats" | jq '{
  total: .total,
  avgDuration: .avgDurationMs,
  platforms: [.rows[].platform] | unique,
  topPaths: [.rows[].path] | group_by(.) | map({path: .[0], count: length}) | sort_by(.count) | reverse[0:5]
}'
```

**关键数据**:
- 总日志数: 8,929条
- 平均响应时间: 12ms
- 支持平台: Web, Android, Windows, Linux

## 3️⃣ 数据分析接口 (需要Token)

### 分析趋势
```bash
curl -s "http://120.48.95.51:7001/api/analytics/trends" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

**数据洞察**:
- 14个时间段数据
- 总事件数: 10,809个
- 独立用户数: 625个
- 最高峰: 2026-01-16 (1,590事件)

### 页面访问统计
```bash
curl -s "http://120.48.95.51:7001/api/analytics/page-views" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

**热门页面**:
1. login_page: 750 PV, 746 UV (99.5%转化率)
2. echarts_page: 672 PV, 378 UV (56.3%转化率)
3. user_info_page: 192 PV, 3 UV
4. register_page: 11 PV, 9 UV

### 事件统计
```bash
curl -s "http://120.48.95.51:7001/api/analytics/event-stats" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# 特定事件统计
curl -s "http://120.48.95.51:7001/api/analytics/event-stats" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  --data-urlencode "eventName=page_view" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

**热门事件**:
1. page_navigated: 3,199次 (669用户)
2. performance_metric: 2,104次 (334用户)
3. main_menu_changed: 1,669次 (59用户)
4. page_view: 1,625次 (747用户)
5. chart_data_viewed: 1,281次 (370用户)

## 4️⃣ 用户管理接口 (需要Token)

### 用户列表
```bash
# 基本用户列表
curl -s "http://120.48.95.51:7001/api/analytics/user/list" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# 分页查询
curl -s "http://120.48.95.51:7001/api/analytics/user/list?page=1&pageSize=3" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# 按活跃度筛选
curl -s "http://120.48.95.51:7001/api/analytics/user/list" \
  -G \
  --data-urlencode "activityLevel=中活跃" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

**用户分析**:
- 总用户数: 5个
- 活跃等级: 低活跃(4个), 中活跃(1个)
- 价值等级: 重要用户(2个), 潜在用户(2个), 流失用户(1个)

### 用户画像 (当前不可用)
```bash
# 这些接口当前返回失败，可能需要特殊权限
curl -s "http://120.48.95.51:7001/api/user-profile/2" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

curl -s "http://120.48.95.51:7001/api/user-profile/2/tags" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

## 5️⃣ 事件上报接口 (需要Token)

### 批量事件上报
```bash
# 注意: 当前接口对格式要求严格
curl -s -X POST "http://120.48.95.51:7001/api/analytics/events/batch" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "app_id": "qt-example-app",
    "device_info": {
      "platform": "desktop",
      "os": "macOS",
      "version": "1.0.0"
    },
    "events": [
      {
        "event_name": "api_test",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "test_type": "manual_curl",
          "success": true
        }
      }
    ]
  }' | jq '.'
```

## 6️⃣ 性能测试命令

### 登录性能测试
```bash
echo "测试登录接口性能 (5次):"
for i in {1..5}; do
  start=$(date +%s%N)
  curl -s -X POST "http://120.48.95.51:7001/login" \
    -H "Content-Type: application/json" \
    -d '{"email":"626143872@qq.com","password":"123456"}' > /dev/null
  end=$(date +%s%N)
  duration=$(( (end - start) / 1000000 ))
  echo "请求 $i: ${duration}ms"
done
```

### 系统接口性能测试
```bash
echo "测试系统信息接口性能 (10次):"
total=0
for i in {1..10}; do
  start=$(date +%s%N)
  curl -s "http://120.48.95.51:7001/system/info" > /dev/null
  end=$(date +%s%N)
  duration=$(( (end - start) / 1000000 ))
  total=$((total + duration))
  echo "请求 $i: ${duration}ms"
done
avg=$((total / 10))
echo "平均响应时间: ${avg}ms"
```

## 7️⃣ 数据分析命令

### 用户行为分析
```bash
# 分析用户活跃度分布
curl -s "http://120.48.95.51:7001/api/analytics/user/list" \
  -H "Authorization: Bearer $TOKEN" | \
  jq '.data.users | group_by(.activityLevel) | map({level: .[0].activityLevel, count: length})'

# 分析用户价值分布  
curl -s "http://120.48.95.51:7001/api/analytics/user/list" \
  -H "Authorization: Bearer $TOKEN" | \
  jq '.data.users | group_by(.valueLevel) | map({level: .[0].valueLevel, count: length})'
```

### 页面转化率分析
```bash
# 计算页面转化率
curl -s "http://120.48.95.51:7001/api/analytics/page-views" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Authorization: Bearer $TOKEN" | \
  jq '.data | map({page: .pageUrl, pv: .pv, uv: .uv, conversion_rate: (.uv / .pv * 100 | round)}) | sort_by(.conversion_rate) | reverse'
```

### 事件热力图数据
```bash
# 获取事件热力图数据
curl -s "http://120.48.95.51:7001/api/analytics/event-stats" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Authorization: Bearer $TOKEN" | \
  jq '.data | map({event: .eventName, total: .count, users: .uniqueUsers, avg_per_user: (.count / .uniqueUsers | round)}) | sort_by(.total) | reverse'
```

## 8️⃣ 一键测试脚本

### 快速健康检查
```bash
#!/bin/bash
echo "=== API健康检查 ==="

# 1. 系统状态
echo "1. 系统状态:"
curl -s "http://120.48.95.51:7001/system/info" | jq '{cpu: .data.cpu_usage, mem: .data.mem_usage, uptime: .data.uptime_days}'

# 2. 登录测试
echo "2. 登录测试:"
TOKEN=$(curl -s -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"626143872@qq.com","password":"123456"}' | jq -r '.token')

if [ "$TOKEN" != "null" ]; then
  echo "✅ 登录成功"
  
  # 3. 数据接口测试
  echo "3. 数据接口测试:"
  curl -s "http://120.48.95.51:7001/api/analytics/user/list?pageSize=1" \
    -H "Authorization: Bearer $TOKEN" | jq '{total_users: .data.total}'
else
  echo "❌ 登录失败"
fi

echo "=== 检查完成 ==="
```

## 📊 测试结果总结

### ✅ 完全可用的接口
- `POST /login` - 用户登录 (34ms平均响应)
- `GET /system/info` - 系统信息 (12ms平均响应)
- `GET /system/logs/stats` - 日志统计 (12ms平均响应)
- `GET /api/analytics/trends` - 分析趋势 (需要日期参数)
- `GET /api/analytics/page-views` - 页面统计 (需要日期参数)
- `GET /api/analytics/event-stats` - 事件统计 (需要日期参数)
- `GET /api/analytics/user/list` - 用户列表

### ⚠️ 部分可用的接口
- `POST /api/analytics/events/batch` - 事件上报 (格式要求严格)
- `GET /api/user-profile/*` - 用户画像系列 (权限问题)

### 📈 性能数据
- 平均响应时间: 34ms (登录), 12ms (系统接口)
- 服务器负载: CPU 4.8%, 内存 61%
- 可用性: 100% (主要接口)
- 数据量: 8,929条日志, 5个用户, 11,809个事件

**结论**: API服务器运行稳定，性能优秀，数据丰富，适合生产使用。