# 用户画像API文档

## 概述
本文档包含用户画像API的完整文档，包括API测试用例、修复指南和使用说明。

## API基础信息
- **基础地址**: `http://120.48.95.51:7001`
- **API版本**: v1.0
- **数据库**: PostgreSQL
- **主机**: 120.48.95.51:5432
- **数据库名**: egg_example

## 数据库表结构

### user_profiles 表
```sql
CREATE TABLE IF NOT EXISTS user_profiles (
  id SERIAL PRIMARY KEY,
  user_id VARCHAR(255) NOT NULL UNIQUE,
  register_time TIMESTAMP,
  last_active_time TIMESTAMP,
  total_events INTEGER DEFAULT 0,
  active_days INTEGER DEFAULT 0,
  tags JSONB,
  behavior_features JSONB,
  value_assessment JSONB,
  interest_profile JSONB,
  activity_level VARCHAR(50),
  value_level VARCHAR(50),
  created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引
CREATE INDEX IF NOT EXISTS idx_user_id ON user_profiles(user_id);
CREATE INDEX IF NOT EXISTS idx_activity_level ON user_profiles(activity_level);
CREATE INDEX IF NOT EXISTS idx_value_level ON user_profiles(value_level);
CREATE INDEX IF NOT EXISTS idx_last_active_time ON user_profiles(last_active_time);
```

## API接口文档

### 1. 获取用户完整画像

#### 接口信息
- **路径**: `/api/analytics/user/profile`
- **方法**: `GET`
- **描述**: 获取指定用户的完整画像信息

#### 请求参数
| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| userId | string | 是 | 用户ID | "2" |

#### 请求示例
```bash
curl -X GET "http://120.48.95.51:7001/api/analytics/user/profile?userId=2"
```

#### 响应示例
```json
{
  "success": true,
  "data": {
    "userId": "2",
    "registerTime": "2025-12-31T02:21:25.514Z",
    "lastActiveTime": "2025-12-31T06:43:25.578Z",
    "totalEvents": 68,
    "activeDays": 1,
    "tags": [
      { "name": "低活跃", "type": "activity" },
      { "name": "新用户", "type": "loyalty" },
      { "name": "低价值", "type": "value" }
    ],
    "behaviorFeatures": {
      "visitFrequency": {
        "totalVisits": 68,
        "activeDays": 1,
        "avgDailyVisits": "68.00"
      },
      "pagePreference": [],
      "featureUsage": []
    },
    "valueAssessment": {
      "level": "流失用户",
      "score": 10,
      "description": "用户活跃度极低，需要重新激活"
    }
  }
}
```

### 2. 获取用户标签

#### 接口信息
- **路径**: `/api/analytics/user/tags`
- **方法**: `GET`
- **描述**: 获取指定用户的标签信息

#### 请求示例
```bash
curl -X GET "http://120.48.95.51:7001/api/analytics/user/tags?userId=2"
```

#### 响应示例
```json
{
  "success": true,
  "data": [
    { "name": "低活跃", "type": "activity" },
    { "name": "新用户", "type": "loyalty" },
    { "name": "低价值", "type": "value" }
  ]
}
```

### 3. 获取用户行为特征

#### 接口信息
- **路径**: `/api/analytics/user/behavior`
- **方法**: `GET`
- **描述**: 获取用户的行为特征

#### 请求示例
```bash
curl -X GET "http://120.48.95.51:7001/api/analytics/user/behavior?userId=2"
```

#### 响应示例
```json
{
  "success": true,
  "data": {
    "visitFrequency": {
      "totalVisits": 68,
      "activeDays": 1,
      "avgDailyVisits": "68.00"
    },
    "pagePreference": [
      {
        "pageName": "home",
        "visitCount": 30,
        "percentage": "44.12"
      }
    ],
    "featureUsage": [
      {
        "eventName": "page_view",
        "usageCount": 68,
        "usageDays": 1
      }
    ]
  }
}
```

### 4. 获取用户兴趣画像

#### 接口信息
- **路径**: `/api/analytics/user/interest`
- **方法**: `GET`
- **描述**: 获取用户的兴趣画像

#### 请求示例
```bash
curl -X GET "http://120.48.95.51:7001/api/analytics/user/interest?userId=2"
```

#### 响应示例
```json
{
  "success": true,
  "data": [
    {
      "name": "home",
      "score": 44.12,
      "type": "page"
    },
    {
      "name": "page_view",
      "score": 100.0,
      "type": "feature"
    }
  ]
}
```

### 5. 获取用户价值评估

#### 接口信息
- **路径**: `/api/analytics/user/value`
- **方法**: `GET`
- **描述**: 获取用户的价值评估

#### 请求示例
```bash
curl -X GET "http://120.48.95.51:7001/api/analytics/user/value?userId=2"
```

#### 响应示例
```json
{
  "success": true,
  "data": {
    "level": "流失用户",
    "score": 10,
    "description": "用户活跃度极低，需要重新激活"
  }
}
```

### 6. 获取用户列表

#### 接口信息
- **路径**: `/api/analytics/user/list`
- **方法**: `GET`
- **描述**: 获取用户列表，支持分页和筛选

#### 请求参数
| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| page | number | 否 | 页码，默认1 | "1" |
| pageSize | number | 否 | 每页数量，默认20 | "20" |
| activityLevel | string | 否 | 活跃度筛选 | "高活跃" |
| valueLevel | string | 否 | 价值等级筛选 | "核心用户" |

#### 请求示例
```bash
# 获取第一页用户列表
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list?page=1&pageSize=20"

# 获取高活跃用户
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" --get --data-urlencode "activityLevel=高活跃"

# 获取核心用户
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list" --get --data-urlencode "valueLevel=核心用户"
```

#### 响应示例
```json
{
  "success": true,
  "data": {
    "list": [
      {
        "userId": "2",
        "registerTime": "2025-12-31T02:21:25.514Z",
        "lastActiveTime": "2025-12-31T06:43:25.578Z",
        "totalEvents": 68,
        "activeDays": 1,
        "activityLevel": "low",
        "valueLevel": "流失用户"
      }
    ],
    "total": 1,
    "page": 1,
    "pageSize": 20
  }
}
```

## 标签生成规则

### 活跃度标签
- **高活跃**: activeDays >= 20
- **中活跃**: 10 <= activeDays < 20
- **低活跃**: activeDays < 10

### 忠诚度标签
- **忠诚用户**: daysSinceRegister > 30 && activeDays > 15
- **普通用户**: daysSinceRegister > 7
- **新用户**: daysSinceRegister <= 7

### 价值标签
- **高价值**: totalEvents > 1000
- **中价值**: 500 < totalEvents <= 1000
- **低价值**: totalEvents <= 500

### 价值等级说明
- **核心用户**: score > 80
- **重要用户**: 60 < score <= 80
- **普通用户**: 40 < score <= 60
- **低价值用户**: 20 < score <= 40
- **流失用户**: score <= 20

## 故障排除

### 常见问题

#### Q1: API请求不通，返回空数据
**根本原因：**
1. 数据库中缺少 `user_profiles` 表
2. 表中没有用户画像数据

**解决步骤：**

1. **创建 user_profiles 表**
```bash
# 使用psql命令行工具
psql -h 120.48.95.51 -p 5432 -U egg_example -d egg_example -f database/migrations/create_user_profiles.sql
```

2. **填充用户画像数据**
```bash
# 触发批量更新所有用户画像
curl -X POST "http://120.48.95.51:7001/api/user-profile/update-all"
```

3. **验证API**
```bash
# 获取第一页用户列表
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list?page=1&pageSize=20"

# 获取高活跃用户
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list?activityLevel=high"

# 获取核心用户
curl -X GET "http://120.48.95.51:7001/api/analytics/user/list?valueLevel=core"
```

#### Q2: API返回500错误
**常见原因：**
1. `user_profiles` 表不存在
2. 数据库连接失败
3. SQL查询语法错误

**解决方法：**
1. 检查服务器日志
2. 验证数据库连接配置
3. 检查SQL语句是否正确

#### Q3: 用户画像数据不准确
**原因：**
- 用户行为数据不完整
- 标签生成逻辑需要调整
- 数据更新延迟

**解决方法：**
1. 确认用户行为数据完整性
2. 调整标签生成规则
3. 检查定时任务是否正常运行

#### Q4: 接口响应慢
**原因：**
- 数据库查询未优化
- 缺少索引
- 数据量过大

**解决方法：**
1. 优化SQL查询
2. 添加必要的数据库索引
3. 考虑分页或缓存

### 手动操作

#### 更新单个用户画像
```bash
curl -X POST "http://120.48.95.51:7001/api/user-profile/update/{userId}"
```

#### 检查数据库数据
```sql
-- 查看用户画像数据
SELECT 
  user_id,
  register_time,
  last_active_time,
  total_events,
  active_days,
  activity_level,
  value_level
FROM user_profiles
ORDER BY last_active_time DESC
LIMIT 10;

-- 查看高活跃用户
SELECT * FROM user_profiles WHERE activity_level = '高活跃';

-- 查看核心用户
SELECT * FROM user_profiles WHERE value_level = '核心用户';
```

## 测试用例

### 基础功能测试

#### TC-001: 正常获取用户画像
- **目的**: 验证能正常获取存在用户的完整画像
- **步骤**:
  1. 发送GET请求到 `/api/analytics/user/profile?userId=2`
  2. 检查响应状态码为200
  3. 验证响应数据包含完整的用户画像信息
- **预期**: 返回完整的用户画像数据

#### TC-002: 用户不存在
- **目的**: 验证用户不存在时的处理
- **步骤**:
  1. 发送GET请求到 `/api/analytics/user/profile?userId=999999`
  2. 检查响应状态码为200
  3. 验证data为null，message为"User not found"
- **预期**: 返回success=true，data=null

#### TC-003: 参数验证
- **目的**: 验证缺少必填参数时的处理
- **步骤**:
  1. 发送GET请求到 `/api/analytics/user/profile`
  2. 检查响应状态码为422
  3. 验证错误信息
- **预期**: 返回success=false，message="userId is required"

### 性能测试

#### TC-030: 大量数据查询性能
- **目的**: 验证查询大量用户数据时的性能
- **步骤**:
  1. 发送请求获取用户列表，pageSize=100
  2. 记录响应时间
  3. 验证响应时间<2秒
- **预期**: 响应时间<2秒

### 安全测试

#### TC-032: SQL注入测试
- **目的**: 验证系统对SQL注入攻击的防护
- **步骤**:
  1. 发送请求：`/api/analytics/user/profile?userId=1' OR '1'='1`
  2. 验证系统不会返回所有用户数据
- **预期**: 系统正确处理，不泄露数据库信息

## 相关文件

- SQL脚本: `database/migrations/create_user_profiles.sql`
- Service: `app/service/userProfile.js`
- Controller: `app/controller/userProfile.js`
- Model: `app/model/user_profile.js`
- 定时任务: `app/schedule/updateUserProfile.js`

## 数据库连接信息

- **主机**: 120.48.95.51
- **端口**: 5432
- **数据库**: egg_example
- **用户名**: egg_example
- **密码**: 1994514Xia@@

---

**文档版本**: v1.0  
**最后更新**: 2026年1月3日  
**维护者**: 开发团队