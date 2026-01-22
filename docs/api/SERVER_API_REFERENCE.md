# 服务器API接口文档

## 基础信息

- **服务器地址**: `http://120.48.95.51:7001`
- **API版本**: v1.0
- **数据格式**: JSON
- **字符编码**: UTF-8

## 网络连接测试

### 检查网络连接
```bash
# 1. 检查服务器是否可达
ping 120.48.95.51

# 2. 检查端口是否开放
telnet 120.48.95.51 7001
# 或者使用 nc (netcat)
nc -zv 120.48.95.51 7001

# 3. 检查代理设置
echo "HTTP代理: $http_proxy"
echo "HTTPS代理: $https_proxy"

# 4. 临时禁用代理测试
unset http_proxy
unset https_proxy
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"626143872@qq.com","password":"123456"}' \
  --connect-timeout 10 \
  -v

# 5. 使用代理测试 (如果需要)
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"626143872@qq.com","password":"123456"}' \
  --proxy http://127.0.0.1:7890 \
  --connect-timeout 10 \
  -v

# 6. 绕过代理测试
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"626143872@qq.com","password":"123456"}' \
  --noproxy "*" \
  --connect-timeout 10 \
  -v
```

### 网络问题排查
```bash
# 检查DNS解析
nslookup 120.48.95.51

# 检查路由
traceroute 120.48.95.51

# 检查防火墙 (macOS)
sudo pfctl -s rules | grep 120.48.95.51

# 检查本地网络配置
ifconfig
netstat -rn
```

## 认证相关接口

### 1. 用户登录

**接口信息**
- **路径**: `/login`
- **方法**: `POST`
- **描述**: 用户登录认证

**请求参数**
```json
{
  "email": "string",     // 用户邮箱
  "password": "string"   // 用户密码
}
```

**响应示例**
```json
{
  "code": 0,
  "msg": "Login success",
  "token": "MTFfMTc2ODY1ODUxNDU2OF82ZDE0MGI1NjExN2IzN2E3YzI0MTc2OTg4MDBmZTFmOQ==",
  "data": {
    "id": 11,
    "username": "test",
    "email": "test@qq.com",
    "avatar": "http://qingpengxia.oss-cn-shenzhen.aliyuncs.com/1767169861280_usicfuns6h",
    "createTime": "2025-12-31T07:55:44.451Z",
    "updateTime": "2026-01-04T10:11:49.115Z"
  }
}
```

### 2. 用户注册

**接口信息**
- **路径**: `/register`
- **方法**: `POST`
- **描述**: 用户注册

**请求参数**
```json
{
  "username": "string",  // 用户名
  "email": "string",     // 邮箱
  "password": "string"   // 密码
}
```

**响应示例**
```json
{
  "code": 0,
  "msg": "Registration successful",
  "data": {
    "id": 12,
    "username": "newuser",
    "email": "newuser@example.com"
  }
}
```

## 用户管理接口

### 3. 修改密码

**接口信息**
- **路径**: `/user/change-password`
- **方法**: `POST`
- **描述**: 修改用户密码
- **认证**: 需要Token

**请求参数**
```json
{
  "oldPassword": "string",  // 旧密码
  "newPassword": "string"   // 新密码
}
```

**响应示例**
```json
{
  "code": 0,
  "msg": "Password changed successfully"
}
```

## 文件上传接口

### 4. 图片上传

**接口信息**
- **路径**: `/api/upload/image`
- **方法**: `POST`
- **描述**: 上传图片文件
- **内容类型**: `multipart/form-data`

**请求参数**
- `file`: 图片文件 (form-data)

**响应示例**
```json
{
  "code": 0,
  "msg": "Upload successful",
  "data": {
    "url": "http://qingpengxia.oss-cn-shenzhen.aliyuncs.com/1767169861280_usicfuns6h",
    "filename": "avatar.jpg"
  }
}
```

## 系统信息接口

### 5. 系统信息

**接口信息**
- **路径**: `/system/info`
- **方法**: `GET`
- **描述**: 获取系统信息和服务器状态

**实际响应示例** (基于真实数据)
```json
{
  "code": 0,
  "msg": "获取系统信息成功",
  "data": {
    "cpu_usage": 4.3,
    "mem_total": 1886,
    "mem_used": 1164,
    "mem_usage": 61,
    "disk_total": 40,
    "disk_used": 15,
    "disk_usage": 37,
    "load_1": 0,
    "load_5": 0,
    "load_15": 0,
    "uptime_days": 21.59,
    "network_interface": "eth0",
    "network_rx_bytes": 6543729098,
    "network_tx_bytes": 6983086291,
    "network_rx_mb": 6240.58,
    "network_tx_mb": 6659.59,
    "ip_address": "117.129.82.199",
    "os_info": "Ubuntu 24.04.1 LTS (Noble Numbat)"
  },
  "platform": "linux",
  "timestamp": "2026-01-18T07:54:18.716Z"
}
```

**字段说明**
- `cpu_usage`: CPU使用率 (百分比)
- `mem_total`: 总内存 (MB)
- `mem_used`: 已用内存 (MB)
- `mem_usage`: 内存使用率 (百分比)
- `disk_total`: 总磁盘空间 (GB)
- `disk_used`: 已用磁盘空间 (GB)
- `disk_usage`: 磁盘使用率 (百分比)
- `load_1/5/15`: 系统负载 (1/5/15分钟)
- `uptime_days`: 系统运行天数
- `network_*`: 网络统计信息
- `ip_address`: 服务器IP地址
- `os_info`: 操作系统信息
```

### 6. 系统日志统计

**接口信息**
- **路径**: `/system/logs/stats`
- **方法**: `GET`
- **描述**: 获取系统日志统计信息

**请求参数**
- `startDate`: 开始日期 (可选)
- `endDate`: 结束日期 (可选)
- `level`: 日志级别 (可选)
- `limit`: 返回记录数限制 (可选，默认10)

**实际响应示例** (基于真实数据)
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
    },
    {
      "id": 8886,
      "path": "/api/analytics/trends",
      "method": "GET",
      "ip": "117.129.82.199",
      "requestTime": "2026-01-18T07:24:55.338Z",
      "durationMs": 14,
      "platform": "Android"
    }
  ],
  "total": 8887,
  "avgDurationMs": 12
}
```

**字段说明**
- `rows`: 日志记录数组
  - `id`: 日志记录ID
  - `path`: 请求路径
  - `method`: HTTP方法
  - `ip`: 客户端IP地址
  - `requestTime`: 请求时间 (ISO 8601格式)
  - `durationMs`: 请求处理时长 (毫秒)
  - `platform`: 客户端平台 (Web/Android/iOS/Windows/Linux等)
- `total`: 总记录数
- `avgDurationMs`: 平均响应时间 (毫秒)
```

## 用户画像分析接口

### 7. 获取用户画像

**接口信息**
- **路径**: `/api/user-profile/{userId}`
- **方法**: `GET`
- **描述**: 获取指定用户的完整画像信息

**路径参数**
- `userId`: 用户ID

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "userId": "2",
    "profile": {
      "basicInfo": {
        "age": 25,
        "gender": "male",
        "location": "北京"
      },
      "behavior": {
        "activityLevel": "高活跃",
        "loginFrequency": "daily"
      },
      "interests": ["技术", "游戏", "音乐"],
      "value": "核心用户"
    }
  }
}
```

### 8. 获取用户标签

**接口信息**
- **路径**: `/api/user-profile/{userId}/tags`
- **方法**: `GET`
- **描述**: 获取指定用户的标签信息

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "userId": "2",
    "tags": [
      {
        "category": "兴趣爱好",
        "tags": ["技术达人", "游戏玩家"]
      },
      {
        "category": "行为特征",
        "tags": ["高活跃用户", "早起用户"]
      }
    ]
  }
}
```

### 9. 获取用户行为统计

**接口信息**
- **路径**: `/api/user-profile/{userId}/behavior`
- **方法**: `GET`
- **描述**: 获取用户的行为特征统计

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "userId": "2",
    "behavior": {
      "loginCount": 150,
      "avgSessionDuration": 45,
      "preferredTime": "evening",
      "activityLevel": "高活跃"
    }
  }
}
```

### 10. 获取用户兴趣画像

**接口信息**
- **路径**: `/api/user-profile/{userId}/interests`
- **方法**: `GET`
- **描述**: 获取用户的兴趣画像

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "userId": "2",
    "interests": {
      "primary": ["技术", "编程"],
      "secondary": ["游戏", "音乐"],
      "scores": {
        "技术": 0.95,
        "编程": 0.90,
        "游戏": 0.75
      }
    }
  }
}
```

### 11. 获取用户价值评估

**接口信息**
- **路径**: `/api/user-profile/{userId}/value`
- **方法**: `GET`
- **描述**: 获取用户的价值评估

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "userId": "2",
    "value": {
      "level": "核心用户",
      "score": 85,
      "factors": {
        "activity": 90,
        "engagement": 80,
        "retention": 85
      }
    }
  }
}
```

## 数据分析接口

### 12. 获取用户列表

**接口信息**
- **路径**: `/api/analytics/user/list`
- **方法**: `GET`
- **描述**: 获取用户列表，支持分页和筛选

**请求参数**
- `page`: 页码 (默认: 1)
- `pageSize`: 每页数量 (默认: 20)
- `activityLevel`: 活跃度筛选 (可选)
- `valueLevel`: 价值等级筛选 (可选)

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "users": [
      {
        "id": 1,
        "username": "user1",
        "activityLevel": "高活跃",
        "valueLevel": "核心用户"
      }
    ],
    "pagination": {
      "page": 1,
      "pageSize": 20,
      "total": 100,
      "totalPages": 5
    }
  }
}
```

### 13. 获取分析趋势

**接口信息**
- **路径**: `/api/analytics/trends`
- **方法**: `GET`
- **描述**: 获取数据分析趋势信息
- **认证**: 需要Token

**请求参数** (必填)
- `startDate`: 开始日期 (必填，格式: YYYY-MM-DD)
- `endDate`: 结束日期 (必填，格式: YYYY-MM-DD)
- `timeRange`: 时间范围 (可选: 7d, 30d, 90d)
- `metric`: 指标类型 (可选: users, events, performance)

**错误响应示例** (缺少必填参数)
```json
{
  "success": false,
  "message": "startDate and endDate are required"
}
```

**实际响应示例** (基于真实测试数据)
```json
{
  "success": true,
  "data": [
    {
      "timeBucket": "2026-01-16T16:00:00.000Z",
      "count": 1590,
      "uniqueUsers": 96
    },
    {
      "timeBucket": "2026-01-15T16:00:00.000Z", 
      "count": 463,
      "uniqueUsers": 28
    },
    {
      "timeBucket": "2026-01-12T16:00:00.000Z",
      "count": 515,
      "uniqueUsers": 23
    }
  ]
}
```

**字段说明**
- `timeBucket`: 时间段 (ISO 8601格式)
- `count`: 该时间段内的事件总数
- `uniqueUsers`: 该时间段内的独立用户数
```

### 14. 获取页面访问统计

**接口信息**
- **路径**: `/api/analytics/page-views`
- **方法**: `GET`
- **描述**: 获取页面访问统计数据
- **认证**: 需要Token

**请求参数** (必填)
- `startDate`: 开始日期 (必填，格式: YYYY-MM-DD)
- `endDate`: 结束日期 (必填，格式: YYYY-MM-DD)
- `page`: 特定页面路径 (可选)

**错误响应示例** (缺少必填参数)
```json
{
  "success": false,
  "message": "startDate and endDate are required"
}
```

**实际响应示例** (基于真实测试数据)
```json
{
  "success": true,
  "data": [
    {
      "pageUrl": "login_page",
      "pv": 750,
      "uv": 746
    },
    {
      "pageUrl": "echarts_page", 
      "pv": 672,
      "uv": 378
    },
    {
      "pageUrl": "user_info_page",
      "pv": 192,
      "uv": 3
    },
    {
      "pageUrl": "register_page",
      "pv": 11,
      "uv": 9
    }
  ]
}
```

**字段说明**
- `pageUrl`: 页面标识符
- `pv`: 页面浏览量 (Page Views)
- `uv`: 独立访客数 (Unique Visitors)
```

### 15. 获取事件统计

**接口信息**
- **路径**: `/api/analytics/event-stats`
- **方法**: `GET`
- **描述**: 获取事件统计数据
- **认证**: 需要Token

**请求参数** (必填)
- `startDate`: 开始日期 (必填，格式: YYYY-MM-DD)
- `endDate`: 结束日期 (必填，格式: YYYY-MM-DD)
- `eventName`: 事件名称 (可选)

**错误响应示例** (缺少必填参数)
```json
{
  "success": false,
  "message": "startDate and endDate are required"
}
```

**实际响应示例** (基于真实测试数据)
```json
{
  "success": true,
  "data": [
    {
      "eventName": "page_navigated",
      "count": 3199,
      "uniqueUsers": 669
    },
    {
      "eventName": "performance_metric",
      "count": 2104,
      "uniqueUsers": 334
    },
    {
      "eventName": "main_menu_changed",
      "count": 1669,
      "uniqueUsers": 59
    },
    {
      "eventName": "page_view",
      "count": 1625,
      "uniqueUsers": 747
    },
    {
      "eventName": "chart_data_viewed",
      "count": 1281,
      "uniqueUsers": 370
    }
  ]
}
```

**字段说明**
- `eventName`: 事件名称
- `count`: 事件发生总次数
- `uniqueUsers`: 触发该事件的独立用户数
```

### 16. 批量事件上报

**接口信息**
- **路径**: `/api/analytics/events/batch`
- **方法**: `POST`
- **描述**: 批量上报多个事件数据

**请求参数**
```json
{
  "app_id": "qt-example-app",
  "device_info": {
    "platform": "desktop",
    "os": "macOS",
    "version": "1.0.0"
  },
  "events": [
    {
      "event_name": "login",
      "timestamp": "2026-01-18T10:30:00Z",
      "properties": {
        "method": "email",
        "success": true
      }
    },
    {
      "event_name": "page_view",
      "timestamp": "2026-01-18T10:30:05Z",
      "properties": {
        "page": "dashboard",
        "referrer": "login"
      }
    }
  ]
}
```

**响应示例**
```json
{
  "code": 0,
  "msg": "Events processed successfully",
  "data": {
    "processed": 2,
    "failed": 0,
    "timestamp": "2026-01-18T10:30:00Z"
  }
}
```

### 17. 批量更新用户画像

**接口信息**
- **路径**: `/api/analytics/user/list`
- **方法**: `GET`
- **描述**: 获取用户列表，支持分页和筛选

**请求参数**
- `page`: 页码 (默认: 1)
- `pageSize`: 每页数量 (默认: 20)
- `activityLevel`: 活跃度筛选 (可选)
- `valueLevel`: 价值等级筛选 (可选)

**响应示例**
```json
{
  "code": 0,
  "msg": "Success",
  "data": {
    "users": [
      {
        "id": 1,
        "username": "user1",
        "activityLevel": "高活跃",
        "valueLevel": "核心用户"
      }
    ],
    "pagination": {
      "page": 1,
      "pageSize": 20,
      "total": 100,
      "totalPages": 5
    }
  }
}
```

### 13. 批量更新用户画像

**接口信息**
- **路径**: `/api/user-profile/update-all`
- **方法**: `POST`
- **描述**: 触发批量更新所有用户画像

**响应示例**
```json
{
  "code": 0,
  "msg": "Batch update started",
  "data": {
    "taskId": "batch_update_20260118_103000",
    "estimatedTime": "5 minutes"
  }
}
```

### 14. 更新单个用户画像

**接口信息**
- **路径**: `/api/user-profile/update/{userId}`
- **方法**: `POST`
- **描述**: 更新指定用户的画像信息

**路径参数**
- `userId`: 用户ID

**响应示例**
```json
{
  "code": 0,
  "msg": "User profile updated successfully",
  "data": {
    "userId": "2",
    "updateTime": "2026-01-18T10:30:00Z"
  }
}
```

## 数据埋点接口

### 15. 事件上报

**接口信息**
- **路径**: `/api/analytics/events`
- **方法**: `POST`
- **描述**: 上报用户行为事件数据

**请求参数**
```json
{
  "app_id": "qt-example-app",
  "device_info": {
    "platform": "desktop",
    "os": "macOS",
    "version": "1.0.0"
  },
  "events": [
    {
      "event_name": "page_view",
      "timestamp": "2026-01-18T10:30:00Z",
      "properties": {
        "page": "login",
        "duration": 1500
      }
    }
  ]
}
```

**响应示例**
```json
{
  "code": 0,
  "msg": "Events recorded successfully",
  "data": {
    "processed": 1,
    "timestamp": "2026-01-18T10:30:00Z"
  }
}
```

## 错误码说明

| 错误码 | 说明 |
|--------|------|
| 0 | 成功 |
| 400 | 请求参数错误 |
| 401 | 未授权 |
| 403 | 禁止访问 |
| 404 | 资源不存在 |
| 422 | 参数验证失败 |
| 500 | 服务器内部错误 |

## 请求头说明

### 通用请求头
- `Content-Type`: `application/json`
- `User-Agent`: 客户端标识
- `X-Platform`: 平台标识 (desktop/mobile/web)

### 认证请求头
- `Authorization`: `Bearer {token}` (需要认证的接口)
- `Cookie`: 会话Cookie (自动管理)

## cURL测试命令

### 网络环境配置

根据你的网络环境，选择合适的测试方法：

#### 方法1: 禁用代理测试 (推荐)
```bash
# 临时禁用代理
unset http_proxy
unset https_proxy
unset HTTP_PROXY
unset HTTPS_PROXY

# 测试连接
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' \
  --connect-timeout 10 \
  --max-time 30 \
  -v
```

#### 方法2: 绕过代理测试
```bash
# 使用 --noproxy 参数
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' \
  --noproxy "*" \
  --connect-timeout 10 \
  -v
```

#### 方法3: 检查服务器状态
```bash
# 先检查服务器是否可达
ping -c 4 120.48.95.51

# 检查端口是否开放
nc -zv 120.48.95.51 7001

# 如果端口不通，可能服务器已关闭或网络不可达
```

#### 方法4: 使用备用测试方法
```bash
# 如果服务器不可用，使用本地mock测试
# 创建本地测试服务器 (需要安装 json-server)
npm install -g json-server

# 创建测试数据
cat > db.json << 'EOF'
{
  "login": {
    "code": 0,
    "msg": "Login success",
    "token": "test_token_123456",
    "data": {
      "id": 11,
      "username": "test",
      "email": "test@qq.com"
    }
  }
}
EOF

# 启动本地测试服务器
json-server --watch db.json --port 3000 &

# 测试本地服务器
curl -X POST "http://localhost:3000/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' \
  -v
```

### 认证相关接口测试

#### 1. 用户登录测试
```bash
# 基本登录测试
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -H "X-Platform: desktop" \
  -d '{"email":"test@qq.com","password":"123456"}' \
  -v

# 保存Token到变量 (Linux/macOS)
TOKEN=$(curl -s -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' | \
  jq -r '.token')

echo "Token: $TOKEN"

# 错误登录测试
curl -X POST "http://120.48.95.51:7001/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"wrong@email.com","password":"wrongpass"}' \
  -v
```

#### 2. 用户注册测试
```bash
# 新用户注册
curl -X POST "http://120.48.95.51:7001/register" \
  -H "Content-Type: application/json" \
  -H "X-Platform: desktop" \
  -d '{
    "username":"testuser_'$(date +%s)'",
    "email":"testuser_'$(date +%s)'@example.com",
    "password":"123456"
  }' \
  -v

# 重复邮箱注册测试
curl -X POST "http://120.48.95.51:7001/register" \
  -H "Content-Type: application/json" \
  -d '{
    "username":"duplicate",
    "email":"test@qq.com",
    "password":"123456"
  }' \
  -v
```

### 用户管理接口测试

#### 3. 修改密码测试
```bash
# 需要先登录获取Token
TOKEN="YOUR_TOKEN_HERE"

# 修改密码
curl -X POST "http://120.48.95.51:7001/user/change-password" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "oldPassword":"123456",
    "newPassword":"newpass123"
  }' \
  -v

# 使用错误的旧密码测试
curl -X POST "http://120.48.95.51:7001/user/change-password" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "oldPassword":"wrongpass",
    "newPassword":"newpass123"
  }' \
  -v
```

### 文件上传接口测试

#### 4. 图片上传测试
```bash
# 创建测试图片文件
echo "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8/5+hHgAHggJ/PchI7wAAAABJRU5ErkJggg==" | base64 -d > test_image.png

# 上传图片
curl -X POST "http://120.48.95.51:7001/api/upload/image" \
  -H "Authorization: Bearer $TOKEN" \
  -F "file=@test_image.png" \
  -v

# 上传不存在的文件测试
curl -X POST "http://120.48.95.51:7001/api/upload/image" \
  -H "Authorization: Bearer $TOKEN" \
  -F "file=@nonexistent.jpg" \
  -v

# 清理测试文件
rm -f test_image.png
```

### 系统信息接口测试

#### 5. 系统信息测试
```bash
# 获取系统信息
curl -X GET "http://120.48.95.51:7001/system/info" \
  -H "Accept: application/json" \
  -v

# 带认证的系统信息请求
curl -X GET "http://120.48.95.51:7001/system/info" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v
```

#### 6. 系统日志统计测试 (已验证可用)
```bash
# 基本日志统计 - 已验证可用
curl -X GET "http://120.48.95.51:7001/system/logs/stats" \
  -H "Accept: application/json" \
  -v

# 带限制数量的日志统计
curl -X GET "http://120.48.95.51:7001/system/logs/stats" \
  -G \
  --data-urlencode "limit=5" \
  -H "Accept: application/json" \
  -v

# 带时间范围的日志统计
curl -X GET "http://120.48.95.51:7001/system/logs/stats" \
  -G \
  --data-urlencode "startDate=2026-01-17" \
  --data-urlencode "endDate=2026-01-18" \
  -H "Accept: application/json" \
  -v

# 分析日志数据 (使用jq处理)
curl -s -X GET "http://120.48.95.51:7001/system/logs/stats" | \
  jq '{
    total: .total,
    avgDuration: .avgDurationMs,
    platforms: [.rows[].platform] | unique,
    topPaths: [.rows[].path] | group_by(.) | map({path: .[0], count: length}) | sort_by(.count) | reverse
  }'
```

#### 新发现的接口测试

##### 分析趋势接口测试
```bash
# 获取分析趋势
curl -X GET "http://120.48.95.51:7001/api/analytics/trends" \
  -H "Accept: application/json" \
  -v

# 带时间范围的趋势分析
curl -X GET "http://120.48.95.51:7001/api/analytics/trends" \
  -G \
  --data-urlencode "timeRange=7d" \
  -H "Accept: application/json" \
  -v
```

##### 页面访问统计测试
```bash
# 获取页面访问统计
curl -X GET "http://120.48.95.51:7001/api/analytics/page-views" \
  -H "Accept: application/json" \
  -v

# 特定页面的访问统计
curl -X GET "http://120.48.95.51:7001/api/analytics/page-views" \
  -G \
  --data-urlencode "page=/login" \
  -H "Accept: application/json" \
  -v
```

##### 事件统计测试
```bash
# 获取事件统计
curl -X GET "http://120.48.95.51:7001/api/analytics/event-stats" \
  -H "Accept: application/json" \
  -v

# 特定事件的统计
curl -X GET "http://120.48.95.51:7001/api/analytics/event-stats" \
  -G \
  --data-urlencode "eventName=login" \
  -H "Accept: application/json" \
  -v
```

##### 批量事件上报测试
```bash
# 批量事件上报
curl -X POST "http://120.48.95.51:7001/api/analytics/events/batch" \
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
          "test_type": "curl_batch",
          "endpoint": "/api/analytics/events/batch"
        }
      },
      {
        "event_name": "performance_test",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "response_time": 50,
          "success": true
        }
      }
    ]
  }' \
  -v
```

### 用户画像分析接口测试

#### 7. 获取用户画像测试
```bash
# 获取用户ID为2的画像
curl -X GET "http://120.48.95.51:7001/api/user-profile/2" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 获取不存在用户的画像
curl -X GET "http://120.48.95.51:7001/api/user-profile/999999" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 无认证访问测试
curl -X GET "http://120.48.95.51:7001/api/user-profile/2" \
  -H "Accept: application/json" \
  -v
```

#### 8. 获取用户标签测试
```bash
# 获取用户标签
curl -X GET "http://120.48.95.51:7001/api/user-profile/2/tags" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 批量获取多个用户标签
for userId in 1 2 3 4 5; do
  echo "Getting tags for user $userId:"
  curl -s -X GET "http://120.48.95.51:7001/api/user-profile/$userId/tags" \
    -H "Authorization: Bearer $TOKEN" \
    -H "Accept: application/json" | jq '.'
  echo "---"
done
```

#### 9. 获取用户行为统计测试
```bash
# 获取用户行为统计
curl -X GET "http://120.48.95.51:7001/api/user-profile/2/behavior" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 获取多个用户的行为统计
curl -X GET "http://120.48.95.51:7001/api/user-profile/1/behavior" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -s | jq '.data.behavior'
```

#### 10. 获取用户兴趣画像测试
```bash
# 获取用户兴趣画像
curl -X GET "http://120.48.95.51:7001/api/user-profile/2/interests" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 获取兴趣评分
curl -X GET "http://120.48.95.51:7001/api/user-profile/2/interests" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -s | jq '.data.interests.scores'
```

#### 11. 获取用户价值评估测试
```bash
# 获取用户价值评估
curl -X GET "http://120.48.95.51:7001/api/user-profile/2/value" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 获取价值评估因子
curl -X GET "http://120.48.95.51:7001/api/user-profile/2/value" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -s | jq '.data.value.factors'
```

### 数据分析接口测试

#### 12. 获取用户列表测试
```bash
# 基本用户列表
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 分页获取用户列表
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" \
  -G \
  --data-urlencode "page=1" \
  --data-urlencode "pageSize=10" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 按活跃度筛选用户
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" \
  -G \
  --data-urlencode "activityLevel=高活跃" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 按价值等级筛选用户
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" \
  -G \
  --data-urlencode "valueLevel=核心用户" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v

# 组合筛选条件
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" \
  -G \
  --data-urlencode "page=1" \
  --data-urlencode "pageSize=5" \
  --data-urlencode "activityLevel=高活跃" \
  --data-urlencode "valueLevel=核心用户" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v
```

#### 13. 批量更新用户画像测试
```bash
# 触发批量更新
curl -X POST "http://120.48.95.51:7001/api/user-profile/update-all" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -v

# 检查更新状态 (如果有状态查询接口)
curl -X GET "http://120.48.95.51:7001/api/user-profile/update-status" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json" \
  -v
```

#### 14. 更新单个用户画像测试
```bash
# 更新指定用户画像
curl -X POST "http://120.48.95.51:7001/api/user-profile/update/2" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -v

# 更新不存在的用户
curl -X POST "http://120.48.95.51:7001/api/user-profile/update/999999" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -v
```

### 数据埋点接口测试

#### 15. 事件上报测试
```bash
# 单个事件上报
curl -X POST "http://120.48.95.51:7001/api/analytics/events" \
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
        "event_name": "page_view",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "page": "login",
          "duration": 1500
        }
      }
    ]
  }' \
  -v

# 批量事件上报
curl -X POST "http://120.48.95.51:7001/api/analytics/events" \
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
        "event_name": "login",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "method": "email",
          "success": true
        }
      },
      {
        "event_name": "page_view",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "page": "dashboard",
          "referrer": "login"
        }
      }
    ]
  }' \
  -v

# 性能事件上报
curl -X POST "http://120.48.95.51:7001/api/analytics/events" \
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
        "event_name": "performance",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "metric": "api_response_time",
          "value": 450,
          "api_endpoint": "/api/user/info",
          "method": "GET"
        }
      }
    ]
  }' \
  -v
```

## 批量测试脚本

### 完整API测试脚本
```bash
#!/bin/bash

# API测试脚本
BASE_URL="http://120.48.95.51:7001"
TEST_EMAIL="test@qq.com"
TEST_PASSWORD="123456"

echo "=== API接口测试开始 ==="

# 1. 登录获取Token
echo "1. 测试用户登录..."
LOGIN_RESPONSE=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d "{\"email\":\"$TEST_EMAIL\",\"password\":\"$TEST_PASSWORD\"}")

echo "登录响应: $LOGIN_RESPONSE"

TOKEN=$(echo $LOGIN_RESPONSE | jq -r '.token // empty')
if [ -z "$TOKEN" ] || [ "$TOKEN" = "null" ]; then
  echo "❌ 登录失败，无法获取Token"
  exit 1
fi

echo "✅ 登录成功，Token: ${TOKEN:0:20}..."

# 2. 测试系统信息
echo -e "\n2. 测试系统信息..."
SYSTEM_INFO=$(curl -s -X GET "$BASE_URL/system/info")
echo "系统信息: $SYSTEM_INFO"

# 3. 测试用户画像
echo -e "\n3. 测试用户画像..."
USER_PROFILE=$(curl -s -X GET "$BASE_URL/api/user-profile/2" \
  -H "Authorization: Bearer $TOKEN")
echo "用户画像: $USER_PROFILE"

# 4. 测试用户列表
echo -e "\n4. 测试用户列表..."
USER_LIST=$(curl -s -X GET "$BASE_URL/api/analytics/user/list?page=1&pageSize=5" \
  -H "Authorization: Bearer $TOKEN")
echo "用户列表: $USER_LIST"

# 5. 测试日志统计
echo -e "\n5. 测试日志统计..."
LOG_STATS=$(curl -s -X GET "$BASE_URL/system/logs/stats")
echo "日志统计: $LOG_STATS"

# 6. 测试事件上报
echo -e "\n6. 测试事件上报..."
EVENT_RESPONSE=$(curl -s -X POST "$BASE_URL/api/analytics/events" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "app_id": "qt-example-app",
    "device_info": {
      "platform": "desktop",
      "os": "test",
      "version": "1.0.0"
    },
    "events": [
      {
        "event_name": "api_test",
        "timestamp": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
        "properties": {
          "test_type": "curl_batch",
          "success": true
        }
      }
    ]
  }')
echo "事件上报: $EVENT_RESPONSE"

echo -e "\n=== API接口测试完成 ==="
```

### 性能测试脚本
```bash
#!/bin/bash

# 性能测试脚本
BASE_URL="http://120.48.95.51:7001"
CONCURRENT_REQUESTS=10
TOTAL_REQUESTS=100

echo "=== API性能测试 ==="

# 先登录获取Token
TOKEN=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d '{"email":"test@qq.com","password":"123456"}' | \
  jq -r '.token')

if [ -z "$TOKEN" ] || [ "$TOKEN" = "null" ]; then
  echo "❌ 无法获取Token，退出测试"
  exit 1
fi

# 测试系统信息接口性能
echo "测试系统信息接口性能..."
time for i in $(seq 1 $TOTAL_REQUESTS); do
  curl -s -X GET "$BASE_URL/system/info" > /dev/null &
  if (( i % $CONCURRENT_REQUESTS == 0 )); then
    wait
  fi
done
wait

# 测试用户画像接口性能
echo "测试用户画像接口性能..."
time for i in $(seq 1 50); do
  USER_ID=$((i % 10 + 1))
  curl -s -X GET "$BASE_URL/api/user-profile/$USER_ID" \
    -H "Authorization: Bearer $TOKEN" > /dev/null &
  if (( i % $CONCURRENT_REQUESTS == 0 )); then
    wait
  fi
done
wait

echo "=== 性能测试完成 ==="
```

### JavaScript示例

```javascript
// 用户登录
const loginResponse = await fetch('http://120.48.95.51:7001/login', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    email: 'test@qq.com',
    password: '123456'
  })
});

const loginData = await loginResponse.json();
const token = loginData.token;

// 获取用户画像
const profileResponse = await fetch('http://120.48.95.51:7001/api/user-profile/2', {
  headers: {
    'Authorization': `Bearer ${token}`
  }
});

const profileData = await profileResponse.json();
```

## 注意事项

1. **认证**: 大部分API需要有效的Token认证
2. **限流**: 接口可能有访问频率限制
3. **CORS**: 跨域请求需要正确配置
4. **超时**: 建议设置30秒请求超时
5. **重试**: 网络错误时建议重试3次
6. **缓存**: 某些数据接口支持缓存，注意Cache-Control头

## 更新日志

- **v1.0.0** (2026-01-18): 初始版本，包含基础认证、用户管理、文件上传、系统信息、用户画像分析等接口