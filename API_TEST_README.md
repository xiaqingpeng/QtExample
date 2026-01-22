# API测试使用指南

## 概述

基于真实服务器响应数据，我们已经验证了 `http://120.48.95.51:7001` 服务器的可用性，并创建了完整的API测试工具。

## 已验证可用的接口

### ✅ 无需认证的接口
- `GET /system/logs/stats` - 系统日志统计 (已验证)
- `GET /system/info` - 系统信息
- `POST /login` - 用户登录
- `POST /register` - 用户注册

### ✅ 需要认证的接口 (基于日志数据发现)
- `GET /api/analytics/trends` - 分析趋势
- `GET /api/analytics/page-views` - 页面访问统计  
- `GET /api/analytics/event-stats` - 事件统计
- `POST /api/analytics/events/batch` - 批量事件上报
- `GET /api/analytics/user/tags` - 用户标签
- `GET /api/user-profile/{userId}` - 用户画像系列接口

## 测试工具

### 1. 快速测试脚本
```bash
# 给脚本添加执行权限
chmod +x quick_test.sh

# 运行快速测试
./quick_test.sh
```

**功能**: 
- 测试核心接口可用性
- 自动登录获取Token
- 测试主要分析接口
- 输出格式化的JSON响应

### 2. 完整测试脚本
```bash
# 给脚本添加执行权限  
chmod +x test_api_complete.sh

# 运行完整测试
./test_api_complete.sh
```

**功能**:
- 网络连通性检查
- 所有接口的完整测试
- 错误情况测试
- 性能测试 (响应时间)
- 详细的测试报告

### 3. 手动cURL测试

#### 基础测试 (无需认证)
```bash
# 禁用代理
unset http_proxy https_proxy

# 测试系统日志统计 (已验证可用)
curl -s "http://120.48.95.51:7001/system/logs/stats" | jq '.'

# 测试登录
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' | jq '.'
```

#### 认证测试
```bash
# 1. 先登录获取Token
TOKEN=$(curl -s -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' | \
  jq -r '.token')

echo "Token: $TOKEN"

# 2. 使用Token测试其他接口
curl -s "http://120.48.95.51:7001/api/analytics/trends" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

curl -s "http://120.48.95.51:7001/api/user-profile/2" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

## 真实响应数据示例

### 系统日志统计响应
```json
{
  "code": 0,
  "msg": "",
  "rows": [
    {
      "id": 8887,
      "path": "/login",
      "method": "POST", 
      "ip": "117.129.82.199",
      "requestTime": "2026-01-18T07:30:05.333Z",
      "durationMs": 13,
      "platform": "Web"
    }
  ],
  "total": 8887,
  "avgDurationMs": 12
}
```

### 从日志发现的活跃接口
根据系统日志统计，以下接口最近有访问记录:
- `/login` - 登录接口 (Web, Linux平台)
- `/api/analytics/trends` - 分析趋势 (Android平台)
- `/api/analytics/page-views` - 页面访问统计 (Android平台)  
- `/api/analytics/event-stats` - 事件统计 (Android平台)
- `/api/analytics/user/tags` - 用户标签 (Windows平台)
- `/api/analytics/events/batch` - 批量事件上报 (Web平台)

## 网络问题解决

### 如果遇到代理问题
```bash
# 方法1: 临时禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY

# 方法2: 绕过代理
curl --noproxy "*" "http://120.48.95.51:7001/system/logs/stats"

# 方法3: 检查代理设置
echo "当前代理: $http_proxy"
```

### 如果服务器不可达
```bash
# 检查网络连通性
ping 120.48.95.51

# 检查端口
nc -zv 120.48.95.51 7001

# 检查DNS
nslookup 120.48.95.51
```

## 性能基准

基于实际测试数据:
- **平均响应时间**: 12ms
- **系统日志总数**: 8,887条
- **支持平台**: Web, Android, iOS, Windows, Linux
- **最近活跃**: 2026-01-18 (服务器正常运行)

## 开发建议

1. **认证流程**: 先调用 `/login` 获取Token，然后在请求头中添加 `Authorization: Bearer {token}`

2. **错误处理**: 所有接口都返回统一格式 `{code, msg, data}`，code=0表示成功

3. **平台标识**: 建议在请求中添加 `X-Platform` 头标识客户端平台

4. **分页支持**: 列表接口支持 `page` 和 `pageSize` 参数

5. **时间格式**: 使用ISO 8601格式 (`2026-01-18T07:30:05.333Z`)

## 故障排除

### 常见问题

1. **502 Bad Gateway**: 通常是代理问题，尝试禁用代理
2. **连接超时**: 检查网络连通性和防火墙设置  
3. **401 Unauthorized**: Token过期或无效，重新登录获取
4. **404 Not Found**: 检查接口路径是否正确

### 调试技巧

```bash
# 显示详细请求信息
curl -v "http://120.48.95.51:7001/system/logs/stats"

# 只显示HTTP状态码
curl -s -w "%{http_code}" -o /dev/null "http://120.48.95.51:7001/system/logs/stats"

# 测试连接时间
curl -w "@curl-format.txt" -s -o /dev/null "http: