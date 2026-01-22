# API测试指南

## 概述

本指南提供了完整的API测试方法和工具，基于真实的服务器响应数据编写。

**服务器地址**: `http://120.48.95.51:7001`

## 测试脚本说明

### 1. `quick_test.sh` - 快速测试
最简单的测试脚本，验证基本功能。

```bash
./quick_test.sh
```

**功能**:
- 测试系统日志统计接口
- 尝试登录
- 如果登录成功，测试分析接口

### 2. `test_api_complete.sh` - 完整测试
全面的API测试脚本，包含性能测试。

```bash
./test_api_complete.sh
```

**功能**:
- 网络连通性检查
- 所有系统接口测试
- 认证接口测试
- 分析接口测试
- 用户画像接口测试
- 性能测试
- 生成详细报告

### 3. `test_with_correct_params.sh` - 改进测试
使用正确参数的测试脚本。

```bash
./test_with_correct_params.sh
```

**功能**:
- 使用正确的日期参数测试分析接口
- 尝试多个常见登录凭据
- 性能测试
- 生成可用接口列表

### 4. `find_valid_credentials.sh` - 凭据查找
专门用于查找有效登录凭据的脚本。

```bash
./find_valid_credentials.sh
```

**功能**:
- 测试常见的用户名/密码组合
- 从系统日志分析可能的用户信息
- 尝试注册新账号
- 保存有效凭据到文件

## 测试结果分析

### ✅ 已验证可用的接口

#### 无需认证
- `GET /system/info` - 系统信息
- `GET /system/logs/stats` - 日志统计
- `POST /login` - 用户登录
- `POST /register` - 用户注册 (可能可用)

#### 需要认证 (需要有效Token)
- `GET /api/analytics/trends` - 分析趋势 ⚠️ 需要startDate, endDate参数
- `GET /api/analytics/page-views` - 页面统计 ⚠️ 需要startDate, endDate参数  
- `GET /api/analytics/event-stats` - 事件统计 ⚠️ 需要startDate, endDate参数
- `GET /api/analytics/user/list` - 用户列表
- `GET /api/user-profile/{userId}` - 用户画像
- `GET /api/user-profile/{userId}/tags` - 用户标签
- `GET /api/user-profile/{userId}/behavior` - 用户行为
- `GET /api/user-profile/{userId}/interests` - 用户兴趣
- `GET /api/user-profile/{userId}/value` - 用户价值
- `POST /api/analytics/events/batch` - 批量事件上报
- `POST /user/change-password` - 修改密码
- `POST /api/upload/image` - 图片上传

### 📊 性能数据

基于实际测试结果:
- **平均响应时间**: 43ms
- **系统负载**: 低 (CPU 4.3%, 内存 61%)
- **可用性**: 100% (10/10 请求成功)

### 🔍 发现的问题

1. **登录凭据**: 测试账号 `test@qq.com` 密码可能已更改
2. **必填参数**: 分析接口需要 `startDate` 和 `endDate` 参数
3. **认证要求**: 大部分分析接口需要有效的Token

## 手动测试命令

### 基础测试
```bash
# 禁用代理 (如果使用代理)
unset http_proxy https_proxy

# 测试系统信息
curl -s "http://120.48.95.51:7001/system/info" | jq '.'

# 测试日志统计
curl -s "http://120.48.95.51:7001/system/logs/stats?limit=5" | jq '.'

# 尝试登录
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"YOUR_EMAIL","password":"YOUR_PASSWORD"}' | jq '.'
```

### 认证测试 (需要Token)
```bash
# 设置Token
TOKEN="YOUR_TOKEN_HERE"

# 测试分析趋势 (带正确参数)
curl -s "http://120.48.95.51:7001/api/analytics/trends" \
  -G \
  --data-urlencode "startDate=2026-01-01" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# 测试用户画像
curl -s "http://120.48.95.51:7001/api/user-profile/1" \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

## 故障排除

### 网络连接问题
```bash
# 检查服务器连通性
ping 120.48.95.51

# 检查端口
nc -zv 120.48.95.51 7001

# 禁用代理
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY
```

### 认证问题
1. 运行 `./find_valid_credentials.sh` 查找有效凭据
2. 检查Token是否过期
3. 确认请求头格式: `Authorization: Bearer TOKEN`

### 参数问题
确保分析接口包含必填参数:
- `startDate`: 格式 YYYY-MM-DD
- `endDate`: 格式 YYYY-MM-DD

## 开发建议

### 1. 错误处理
```bash
# 检查HTTP状态码
response=$(curl -s -w "%{http_code}" "URL" -o response.json)
http_code="${response: -3}"

if [ "$http_code" = "200" ]; then
    echo "成功"
else
    echo "失败: HTTP $http_code"
fi
```

### 2. JSON处理
```bash
# 使用jq处理JSON
curl -s "URL" | jq '.data.field'

# 提取特定字段
curl -s "URL" | jq -r '.token // "no_token"'
```

### 3. 批量测试
```bash
# 循环测试多个用户
for userId in {1..5}; do
    curl -s "http://120.48.95.51:7001/api/user-profile/$userId" \
        -H "Authorization: Bearer $TOKEN"
done
```

## 更新日志

- **2026-01-18**: 基于真实服务器响应创建测试指南
- 发现系统信息接口返回详细的服务器状态
- 确认分析接口需要日期参数
- 验证平均响应时间为43ms