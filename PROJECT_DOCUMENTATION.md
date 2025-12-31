# 埋点事件收集与用户画像系统 - 完整文档

## 📋 目录

- [项目概述](#项目概述)
- [项目结构](#项目结构)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [核心功能](#核心功能)
- [API接口文档](#api接口文档)
  - [埋点事件接口](#埋点事件接口)
  - [统计分析接口](#统计分析接口)
  - [用户画像接口](#用户画像接口)
- [数据库设计](#数据库设计)
- [配置说明](#配置说明)
- [部署指南](#部署指南)
- [使用示例](#使用示例)
- [最佳实践](#最佳实践)
- [测试文档](#测试文档)
- [常见问题](#常见问题)
- [前端开发计划](#前端开发计划)

---

## 项目概述

本项目是基于 Egg.js 框架开发的埋点事件收集和分析系统，用于接收、存储和分析前端应用（如 Qt 应用）上报的用户行为数据，并提供用户画像功能。

### 主要特性

✅ **高性能** - 基于 Egg.js 企业级框架，支持高并发请求
✅ **易扩展** - 模块化设计，易于添加新功能
✅ **数据持久化** - 支持 PostgreSQL 数据库存储
✅ **批量处理** - 支持单个和批量事件上报
✅ **统计分析** - 提供事件统计和趋势分析
✅ **用户画像** - 自动生成用户标签、行为特征和价值评估
✅ **完整日志** - 详细的请求日志和错误追踪
✅ **CORS 支持** - 支持跨域请求
✅ **安全可靠** - 完善的错误处理和数据验证

### 技术栈

- **框架**: Egg.js 3.x
- **数据库**: PostgreSQL 12+
- **ORM**: egg-sequelize
- **跨域**: egg-cors
- **Node.js**: 16.0+

### 系统架构

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Qt 前端    │────▶│ Egg.js 后端 │────▶│ PostgreSQL  │
│  应用       │     │  API 服务   │     │  数据库     │
└─────────────┘     └─────────────┘     └─────────────┘
                           │
                           ▼
                    ┌─────────────┐
                    │ 用户画像    │
                    │ 分析引擎    │
                    └─────────────┘
```

---

## 项目结构

```
egg-analytics/
├── app/
│   ├── controller/
│   │   ├── analytics.js          # 埋点事件控制器
│   │   └── userProfile.js         # 用户画像控制器
│   ├── service/
│   │   ├── analytics.js          # 埋点业务逻辑服务
│   │   └── userProfile.js         # 用户画像业务逻辑服务
│   ├── model/
│   │   ├── analytics_event.js    # 埋点事件模型
│   │   └── user_profile.js       # 用户画像模型
│   ├── middleware/
│   │   └── request_logger.js     # 请求日志中间件
│   ├── schedule/
│   │   └── updateUserProfile.js  # 定时更新用户画像
│   └── router.js                  # 路由配置
├── config/
│   ├── config.default.js          # 默认配置
│   ├── config.prod.js             # 生产环境配置
│   ├── plugin.js                  # 插件配置
│   └── database.js                # 数据库配置
├── database/
│   └── migrations/
│       ├── init.sql               # 数据库初始化脚本
│       └── create_user_profiles.sql # 用户画像表脚本
├── logs/                          # 日志目录
│   └── analytics/                 # 埋点事件日志
├── test/                          # 测试目录
├── .gitignore
├── app.js                         # 应用入口
└── package.json                   # 项目依赖
```

---

## 环境要求

### 必需环境

- **Node.js**: >= 16.0.0
- **npm**: >= 8.0.0
- **PostgreSQL**: >= 12.0
- **操作系统**: Linux / macOS / Windows

### 可选环境

- **Redis**: >= 5.0 (用于缓存和队列)
- **Docker**: >= 20.0 (用于容器化部署)

### 安装 Node.js

```bash
# 使用 nvm 安装（推荐）
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
source ~/.bashrc
nvm install 16
nvm use 16

# 验证安装
node --version
npm --version
```

---

## 快速开始

### 1. 创建项目

```bash
# 使用 egg-init 脚手架创建项目
npm install -g egg-init
egg-init egg-analytics --type=simple
cd egg-analytics
```

### 2. 安装依赖

```bash
npm install
```

### 3. 配置数据库

```bash
# 连接到 PostgreSQL
psql -U postgres
```

```sql
-- 创建数据库
CREATE DATABASE egg_example 
  WITH ENCODING='UTF8'
  LC_COLLATE='en_US.UTF-8'
  LC_CTYPE='en_US.UTF-8'
  TEMPLATE=template0;

-- 创建用户
CREATE USER egg_example WITH PASSWORD '1994514Xia@@';

-- 授权
GRANT ALL PRIVILEGES ON DATABASE egg_example TO egg_example;
```

### 4. 执行数据库迁移

```bash
# 执行初始化脚本
psql -h 120.48.95.51 -p 5432 -U egg_example -d egg_example -f database/migrations/init.sql

# 执行用户画像表脚本
psql -h 120.48.95.51 -p 5432 -U egg_example -d egg_example -f database/migrations/create_user_profiles.sql
```

### 5. 配置应用

编辑 `config/config.default.js`，修改数据库配置：

```javascript
config.sequelize = {
  dialect: 'postgres',
  host: '120.48.95.51',
  port: 5432,
  username: 'egg_example',
  password: '1994514Xia@@',
  database: 'egg_example',
  timezone: '+08:00',
  define: {
    freezeTableName: true,
    underscored: true,
    timestamps: true
  }
};
```

### 6. 启动开发服务器

```bash
npm run dev
```

服务器将在 `http://localhost:7001` 启动。

---

## 核心功能

### 1. 事件接收

- 支持单个事件上报
- 支持批量事件上报
- 自动添加元数据（IP、User-Agent、时间戳）
- 数据验证和错误处理

### 2. 数据存储

- PostgreSQL 数据库持久化存储
- JSONB 格式存储事件属性
- 数据库索引优化

### 3. 统计分析

- 按事件类型统计
- 时间范围统计
- 趋势分析
- 分页查询
- 留存率分析
- 用户活跃度分析

### 4. 用户画像

- 自动生成用户标签（活跃度、忠诚度、价值）
- 用户行为特征分析
- 用户兴趣画像
- 用户价值评估
- 定时更新用户画像### 5. 日志记录

- ✅ 请求日志
- ✅ 错误日志
- ✅ 事件日志
- ✅ 性能日志

---
## 数据库设计

### analytics_events 表结构

| 字段名 | 类型 | 说明 | 索引 |
|--------|------|------|------|
| id | SERIAL | 主键，自增 | PRIMARY KEY |
| event_name | VARCHAR(255) | 事件名称 | idx_event_name |
| event_type | VARCHAR(100) | 事件类型 (page_view, click, custom, performance, error) | idx_event_type |
| properties | JSONB | 事件属性，存储自定义字段 | GIN idx_properties |
| user_id | VARCHAR(255) | 用户ID | idx_user_id, idx_event_user |
| session_id | VARCHAR(255) | 会话ID | - |
| duration | INTEGER | 持续时间（毫秒） | - |
| error_message | TEXT | 错误信息 | - |
| created_at | TIMESTAMP | 创建时间 | idx_created_at, idx_event_user |

### 索引设计

```sql
-- 基础索引
CREATE INDEX idx_event_name ON analytics_events(event_name);
CREATE INDEX idx_event_type ON analytics_events(event_type);
CREATE INDEX idx_user_id ON analytics_events(user_id);
CREATE INDEX idx_created_at ON analytics_events(created_at);

-- 复合索引（用户查询优化）
CREATE INDEX idx_event_user ON analytics_events(user_id, created_at);

-- GIN 索引（JSONB 查询优化）
CREATE INDEX idx_properties ON analytics_events USING GIN(properties);
```

### 性能优化视图

```sql
-- 创建事件统计视图
CREATE VIEW v_event_stats AS
SELECT 
  event_name,
  event_type,
  COUNT(*) as event_count,
  COUNT(DISTINCT user_id) as unique_users,
  AVG(duration) as avg_duration,
  MIN(created_at) as first_seen,
  MAX(created_at) as last_seen
FROM analytics_events
GROUP BY event_name, event_type;

-- 创建用户活跃度视图
CREATE VIEW v_user_activity AS
SELECT 
  user_id,
  COUNT(*) as total_events,
  COUNT(DISTINCT DATE(created_at)) as active_days,
  MIN(created_at) as first_event,
  MAX(created_at) as last_event
FROM analytics_events
WHERE user_id IS NOT NULL
GROUP BY user_id;
```

### 表注释

```sql
-- 表注释
COMMENT ON TABLE analytics_events IS '埋点事件数据表';

-- 字段注释
COMMENT ON COLUMN analytics_events.id IS '主键ID';
COMMENT ON COLUMN analytics_events.event_name IS '事件名称';
COMMENT ON COLUMN analytics_events.event_type IS '事件类型：page_view(页面浏览)、click(点击)、custom(自定义)、performance(性能)、error(错误)';
COMMENT ON COLUMN analytics_events.properties IS '事件属性，JSONB格式存储';
COMMENT ON COLUMN analytics_events.user_id IS '用户ID';
COMMENT ON COLUMN analytics_events.session_id IS '会话ID';
COMMENT ON COLUMN analytics_events.duration IS '持续时间（毫秒）';
COMMENT ON COLUMN analytics_events.error_message IS '错误信息';
COMMENT ON COLUMN analytics_events.created_at IS '创建时间';
```

---
## 用户画像接口### 9. 获取用户完整画像

**接口地址**: `GET /api/analytics/user/profile`

**功能描述**: 获取指定用户的完整画像信息，包括基础信息、标签、行为特征、兴趣画像和价值评估。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| userId | string | 是 | 用户ID | `user123` |

**请求示例**:

```bash
curl -s 'http://120.48.95.51:7001/api/analytics/user/profile?userId=user123'
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "userId": "user123",
    "registerTime": "2025-01-01T00:00:00.000Z",
    "lastActiveTime": "2025-12-30T12:00:00.000Z",
    "totalEvents": 100,
    "activeDays": 30,
    "tags": {
      "activityLevel": "高活跃",
      "loyaltyLevel": "忠诚用户",
      "valueLevel": "核心用户"
    },
    "behaviorFeatures": {
      "avgSessionDuration": 300,
      "pageViewsPerSession": 5,
      "mostActiveTime": "14:00-16:00",
      "devicePreference": "desktop"
    },
    "interestProfile": {
      "pageInterests": [
        {"page": "home", "score": 0.8},
        {"page": "profile", "score": 0.6}
      ],
      "featureInterests": [
        {"feature": "search", "score": 0.7},
        {"feature": "export", "score": 0.5}
      ]
    },
    "valueAssessment": {
      "activityScore": 85,
      "loyaltyScore": 90,
      "valueScore": 88,
      "overallScore": 87.7,
      "valueLevel": "核心用户"
    }
  }
}
```

### 10. 获取用户标签

**接口地址**: `GET /api/analytics/user/tags`

**功能描述**: 获取指定用户的标签信息。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| userId | string | 是 | 用户ID | `user123` |

**请求示例**:

```bash
curl -s 'http://120.48.95.51:7001/api/analytics/user/tags?userId=user123'
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "userId": "user123",
    "tags": {
      "activityLevel": "高活跃",
      "loyaltyLevel": "忠诚用户",
      "valueLevel": "核心用户"
    },
    "updatedAt": "2025-12-30T12:00:00.000Z"
  }
}
```

### 11. 获取用户行为特征

**接口地址**: `GET /api/analytics/user/behavior`

**功能描述**: 获取指定用户的行为特征分析。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| userId | string | 是 | 用户ID | `user123` |

**请求示例**:

```bash
curl -s 'http://120.48.95.51:7001/api/analytics/user/behavior?userId=user123'
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "userId": "user123",
    "behaviorFeatures": {
      "avgSessionDuration": 300,
      "pageViewsPerSession": 5,
      "mostActiveTime": "14:00-16:00",
      "devicePreference": "desktop",
      "topPages": [
        {"page": "home", "count": 50},
        {"page": "profile", "count": 30}
      ],
      "topFeatures": [
        {"feature": "search", "count": 20},
        {"feature": "export", "count": 15}
      ]
    },
    "updatedAt": "2025-12-30T12:00:00.000Z"
  }
}
```

### 12. 获取用户兴趣画像

**接口地址**: `GET /api/analytics/user/interest`

**功能描述**: 获取指定用户的兴趣画像。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| userId | string | 是 | 用户ID | `user123` |

**请求示例**:

```bash
curl -s 'http://120.48.95.51:7001/api/analytics/user/interest?userId=user123'
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "userId": "user123",
    "interestProfile": {
      "pageInterests": [
        {"page": "home", "score": 0.8},
        {"page": "profile", "score": 0.6}
      ],
      "featureInterests": [
        {"feature": "search", "score": 0.7},
        {"feature": "export", "score": 0.5}
      ]
    },
    "updatedAt": "2025-12-30T12:00:00.000Z"
  }
}
```

### 13. 获取用户价值评估

**接口地址**: `GET /api/analytics/user/value`

**功能描述**: 获取指定用户的价值评估。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| userId | string | 是 | 用户ID | `user123` |

**请求示例**:

```bash
curl -s 'http://120.48.95.51:7001/api/analytics/user/value?userId=user123'
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "userId": "user123",
    "valueAssessment": {
      "activityScore": 85,
      "loyaltyScore": 90,
      "valueScore": 88,
      "overallScore": 87.7,
      "valueLevel": "核心用户"
    },
    "updatedAt": "2025-12-30T12:00:00.000Z"
  }
}
```

### 14. 获取用户列表

**接口地址**: `GET /api/analytics/user/list`

**功能描述**: 分页获取用户列表，支持按活跃度和价值等级筛选。

**请求参数**:

| 参数名 | 类型 | 必填 | 默认值 | 说明 |
|--------|------|------|--------|------|
| page | number | 否 | 1 | 页码 |
| pageSize | number | 否 | 20 | 每页数量 |
| activityLevel | string | 否 | - | 活跃度筛选 (high/medium/low) |
| valueLevel | string | 否 | - | 价值等级筛选 (core/important/ordinary) |

**请求示例**:

```bash
# 获取第一页用户列表
curl -s 'http://120.48.95.51:7001/api/analytics/user/list?page=1&pageSize=20'

# 获取高活跃用户
curl -s 'http://120.48.95.51:7001/api/analytics/user/list?activityLevel=high'

# 获取核心用户
curl -s 'http://120.48.95.51:7001/api/analytics/user/list?valueLevel=core'
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "users": [
      {
        "userId": "user123",
        "registerTime": "2025-01-01T00:00:00.000Z",
        "lastActiveTime": "2025-12-30T12:00:00.000Z",
        "totalEvents": 100,
        "activeDays": 30,
        "activityLevel": "高活跃",
        "valueLevel": "核心用户"
      }
    ],
    "total": 1,
    "page": 1,
    "pageSize": 20,
    "totalPages": 1
  }
}
```

### 15. 更新用户画像

**接口地址**: `POST /api/user-profile/update/{userId}`

**功能描述**: 手动触发更新指定用户的画像。

**请求示例**:

```bash
curl -X POST "http://120.48.95.51:7001/api/user-profile/update/user123"
```

**响应示例**:

```json
{
  "success": true,
  "message": "User profile updated successfully"
}
```

### 16. 批量更新用户画像

**接口地址**: `POST /api/user-profile/update-all`

**功能描述**: 批量更新所有用户的画像。

**请求示例**:

```bash
curl -X POST "http://120.48.95.51:7001/api/user-profile/update-all"
```

**响应示例**:

```json
{
  "success": true,
  "message": "All user profiles updated successfully",
  "count": 100
}
```# 埋点事件收集与用户画像系统 - 完整文档

## 📋 目录

- [项目概述](#项目概述)
- [项目结构](#项目结构)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [核心功能](#核心功能)
- [API接口文档](#api接口文档)
  - [埋点事件接口](#埋点事件接口)
  - [统计分析接口](#统计分析接口)
  - [用户画像接口](#用户画像接口)
- [数据库设计](#数据库设计)
- [配置说明](#配置说明)
- [部署指南](#部署指南)
- [使用示例](#使用示例)
- [最佳实践](#最佳实践)
- [测试文档](#测试文档)
- [常见问题](#常见问题)
- [前端开发计划](#前端开发计划)

---

## 项目概述

本项目是基于 Egg.js 框架开发的埋点事件收集和分析系统，用于接收、存储和分析前端应用（如 Qt 应用）上报的用户行为数据，并提供用户画像功能。

### 主要特性

✅ **高性能** - 基于 Egg.js 企业级框架，支持高并发请求
✅ **易扩展** - 模块化设计，易于添加新功能
✅ **数据持久化** - 支持 PostgreSQL 数据库存储
✅ **批量处理** - 支持单个和批量事件上报
✅ **统计分析** - 提供事件统计和趋势分析
✅ **用户画像** - 自动生成用户标签、行为特征和价值评估
✅ **完整日志** - 详细的请求日志和错误追踪
✅ **CORS 支持** - 支持跨域请求
✅ **安全可靠** - 完善的错误处理和数据验证

### 技术栈

- **框架**: Egg.js 3.x
- **数据库**: PostgreSQL 12+
- **ORM**: egg-sequelize
- **跨域**: egg-cors
- **Node.js**: 16.0+

---

## 项目结构

```
egg-analytics-backend/
├── app/
│   ├── controller/
│   │   ├── analytics.js          # 埋点事件控制器
│   │   └── user-profile.js        # 用户画像控制器
│   ├── model/
│   │   ├── analytics_event.js     # 事件数据模型
│   │   └── index.js               # 模型加载和关联
│   ├── service/
│   │   ├── analytics.js           # 埋点事件服务
│   │   └── user-profile.js        # 用户画像服务
│   └── middleware/
│       └── error_handler.js        # 错误处理中间件
├── config/
│   ├── config.default.js          # 默认配置
│   ├── config.prod.js              # 生产环境配置
│   └── plugin.js                   # 插件配置
├── migrations/                     # 数据库迁移文件
├── test/                           # 测试文件
├── package.json                    # 项目依赖
├── README.md                       # 项目说明
└── .gitignore                      # Git忽略配置
```

---

## 环境要求

### 必需软件

- **Node.js**: 16.0 或更高版本
- **PostgreSQL**: 12.0 或更高版本
- **npm**: 8.0 或更高版本

### 可选软件

- **Docker**: 20.10+ (用于容器化部署)
- **Nginx**: 1.18+ (用于反向代理)

### 操作系统

- Linux (推荐 Ubuntu 20.04+)
- macOS 10.15+
- Windows 10/11 (使用 WSL2)

---

## 快速开始

### 1. 创建项目

```bash
# 使用 Egg.js CLI 创建项目
npm init egg --type=simple
npm install
```

### 2. 安装依赖

```bash
# 安装核心依赖
npm install --save egg-sequelize egg-pg egg-cors egg-redis
npm install --save pg

# 安装开发依赖
npm install --save-dev egg-bin
```

### 3. 数据库配置

#### 3.1 创建数据库

```sql
-- 创建数据库
CREATE DATABASE analytics_db;

-- 创建用户
CREATE USER analytics_user WITH PASSWORD 'your_password';

-- 授权
GRANT ALL PRIVILEGES ON DATABASE analytics_db TO analytics_user;
```

#### 3.2 创建数据表

```sql
-- 创建 analytics_events 表
CREATE TABLE analytics_events (
  id SERIAL PRIMARY KEY,
  event_name VARCHAR(255) NOT NULL,
  event_type VARCHAR(100) DEFAULT 'custom',
  properties JSONB,
  user_id VARCHAR(255),
  session_id VARCHAR(255),
  duration INTEGER,
  error_message TEXT,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引
CREATE INDEX idx_event_name ON analytics_events(event_name);
CREATE INDEX idx_event_type ON analytics_events(event_type);
CREATE INDEX idx_user_id ON analytics_events(user_id);
CREATE INDEX idx_created_at ON analytics_events(created_at);
CREATE INDEX idx_event_user ON analytics_events(user_id, created_at);

-- 创建 GIN 索引用于 JSONB 查询
CREATE INDEX idx_properties ON analytics_events USING GIN(properties);
```

#### 3.3 配置数据库连接

编辑 `config/config.default.js`:

```javascript
module.exports = appInfo => {
  return {
    sequelize: {
      dialect: 'postgres',
      host: 'localhost',
      port: 5432,
      database: 'analytics_db',
      username: 'analytics_user',
      password: 'your_password',
      timezone: '+08:00',
      define: {
        timestamps: true,
        underscored: true,
      },
    },
  };
};
```

### 4. 应用配置

#### 4.1 启用插件

编辑 `config/plugin.js`:

```javascript
module.exports = {
  sequelize: {
    enable: true,
    package: 'egg-sequelize',
  },
  cors: {
    enable: true,
    package: 'egg-cors',
  },
};
```

#### 4.2 配置 CORS

编辑 `config/config.default.js`:

```javascript
module.exports = appInfo => {
  return {
    cors: {
      origin: '*',
      allowMethods: 'GET,HEAD,PUT,POST,DELETE,PATCH',
    },
  };
};
```

### 5. 启动开发服务器

```bash
# 开发模式
npm run dev

# 生产模式
npm start

# 停止服务
npm stop
```

服务将在 `http://localhost:7001` 启动。

---

## 核心功能

### 1. 事件接收

- ✅ 支持单个事件上报
- ✅ 支持批量事件上报
- ✅ 自动生成事件ID
- ✅ 完整的错误处理

### 2. 数据存储

- ✅ PostgreSQL 数据持久化
- ✅ JSONB 格式存储事件属性
- ✅ 自动时间戳
- ✅ 索引优化查询性能

### 3. 统计分析

- ✅ 事件总数统计
- ✅ 按类型分组统计
- ✅ 时间趋势分析
- ✅ 页面浏览统计 (PV/UV)
- ✅ 用户活跃度统计 (DAU/MAU)

### 4. 用户画像

- ✅ 用户标签生成
- ✅ 行为特征分析
- ✅ 兴趣画像分析
- ✅ 价值评估
- ✅ 用户列表查询

### 5. 日志记录

- ✅ 请求日志
- ✅ 错误日志
- ✅ 性能监控

---


## API接口文档

### 基础信息

- **Base URL**: `http://120.48.95.51:7001`
- **Content-Type**: `application/json`
- **字符编码**: `UTF-8`

---

## 埋点事件接口

### 1. 接收单个事件

**接口地址**: `POST /api/analytics/events`

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 |
|--------|------|------|------|
| event | string | 是 | 事件名称 |
| eventType | string | 否 | 事件类型，默认 'custom' |
| properties | object | 否 | 事件属性 |
| userId | string | 否 | 用户ID |
| sessionId | string | 否 | 会话ID |
| duration | number | 否 | 持续时间（毫秒） |
| errorMessage | string | 否 | 错误信息 |

**请求示例**:

```bash
curl -X POST http://120.48.95.51:7001/api/analytics/events \
  -H "Content-Type: application/json" \
  -d '{
    "event": "login_success",
    "eventType": "custom",
    "properties": {
      "email": "user@example.com",
      "login_time": 1234
    },
    "userId": "user123",
    "sessionId": "session456"
  }'
```

**响应示例**:

```json
{
  "success": true,
  "message": "Event recorded successfully",
  "eventId": "request-id-12345"
}
```

### 2. 批量接收事件

**接口地址**: `POST /api/analytics/events/batch`

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 |
|--------|------|------|------|
| events | array | 是 | 事件数组 |

**请求示例**:

```bash
curl -X POST http://120.48.95.51:7001/api/analytics/events/batch \
  -H "Content-Type: application/json" \
  -d '{
    "events": [
      {
        "event": "page_view",
        "properties": {"page": "login"}
      },
      {
        "event": "button_click",
        "properties": {"button": "submit"}
      }
    ]
  }'
```

**响应示例**:

```json
{
  "success": true,
  "message": "Successfully recorded 2 events",
  "count": 2
}
```

---

## 统计分析接口

### 3. 查询事件统计

**接口地址**: `GET /api/analytics/stats`

**查询参数**:

| 参数名 | 类型 | 必填 | 说明 |
|--------|------|------|------|
| eventType | string | 否 | 事件类型 |
| startDate | string | 否 | 开始日期 (YYYY-MM-DD) |
| endDate | string | 否 | 结束日期 (YYYY-MM-DD) |

**请求示例**:

```bash
curl "http://120.48.95.51:7001/api/analytics/stats?eventType=login_success&startDate=2024-01-01&endDate=2024-12-31"
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "total": 1000,
    "byEventType": [
      {
        "event_name": "login_success",
        "count": 500
      },
      {
        "event_name": "page_view",
        "count": 300
      }
    ],
    "recentTrend": [
      {
        "hour": "2024-01-01 10:00:00",
        "count": 50
      }
    ]
  }
}
```

### 4. 查询事件列表

**接口地址**: `GET /api/analytics/events`

**查询参数**:

| 参数名 | 类型 | 必填 | 默认值 | 说明 |
|--------|------|------|--------|------|
| eventType | string | 否 | - | 事件类型 |
| page | number | 否 | 1 | 页码 |
| pageSize | number | 否 | 50 | 每页数量 |
| startDate | string | 否 | - | 开始日期 |
| endDate | string | 否 | - | 结束日期 |

**请求示例**:

```bash
curl "http://120.48.95.51:7001/api/analytics/events?page=1&pageSize=20&eventType=login_success"
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "events": [
      {
        "id": 1,
        "event_name": "login_success",
        "event_type": "custom",
        "properties": {
          "email": "user@example.com",
          "login_time": 1234
        },
        "user_id": "user123",
        "duration": null,
        "error_message": null,
        "created_at": "2024-01-01T10:00:00.000Z"
      }
    ],
    "total": 100,
    "page": 1,
    "pageSize": 20,
    "totalPages": 5
  }
}
```

### 5. 页面浏览统计

**接口地址**: `GET /api/analytics/page-views`

**查询参数**:

| 参数名 | 类型 | 必填 | 说明 |
|--------|------|------|------|
| startDate | string | 否 | 开始日期 (YYYY-MM-DD) |
| endDate | string | 否 | 结束日期 (YYYY-MM-DD) |

**请求示例**:

```bash
curl "http://120.48.95.51:7001/api/analytics/page-views?startDate=2025-12-30&endDate=2025-12-30"
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "pageStats": [
      {
        "page_url": "/",
        "pv": 38,
        "uv": 5
      },
      {
        "page_url": "/login",
        "pv": 38,
        "uv": 5
      }
    ],
    "totalPV": 76,
    "totalUV": 5
  }
}
```

### 6. 用户活跃度统计

**接口地址**: `GET /api/analytics/user-activity`

**查询参数**:

| 参数名 | 类型 | 必填 | 说明 |
|--------|------|------|------|
| startDate | string | 否 | 开始日期 (YYYY-MM-DD) |
| endDate | string | 否 | 结束日期 (YYYY-MM-DD) |

**请求示例**:

```bash
curl "http://120.48.95.51:7001/api/analytics/user-activity?startDate=2025-12-30&endDate=2025-12-30"
```

**响应示例**:

```json
{
  "success": true,
  "data": {
    "dauStats": [
      {
        "date": "2025-12-30",
        "dau": 5
      }
    ],
    "mauStats": [
      {
        "month": "2025-11-30T16:00:00.000Z",
        "mau": 5
      }
    ]
  }
}
```

### 7. 事件列表接口

**接口地址**: `GET /api/analytics/events`

**功能描述**: 分页获取指定日期范围内的事件列表。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| startDate | string | 是 | 开始日期 | `2025-12-30` |
| endDate | string | 是 | 结束日期 | `2025-12-30` |
| eventType | string | 否 | 事件类型 | `page_view` |
| page | number | 否 | 页码 | `1` (默认) |
| pageSize | number | 否 | 每页数量 | `50` (默认) |

**测试命令**:

```bash
curl -s 'http://localhost:7001/api/analytics/events?startDate=2025-12-30&endDate=2025-12-30'
```

**预期结果**:

```json
{
  "success": true,
  "data": {
    "events": [],
    "total": 0,
    "page": 1,
    "pageSize": 50,
    "totalPages": 0
  }
}
```

### 8. 留存率统计接口

**接口地址**: `GET /api/analytics/retention`

**功能描述**: 获取用户留存率统计数据。

**请求参数**:

| 参数名 | 类型 | 必填 | 说明 | 示例值 |
|--------|------|------|------|--------|
| days | number | 否 | 统计天数 | `7` (默认) |

**测试命令**:

```bash
curl -s 'http://localhost:7001/api/analytics/retention?days=7'
```

**预期结果**:

```json
{
  "success": true,
  "data": {
    "retentionRate": 0,
    "totalUsers": 0,
    "retainedUsers": 0
  }
}
```

---

## 最佳实践

### 事件命名规范

1. **使用小写字母和下划线**
   - ✅ `page_view`
   - ✅ `button_click`
   - ❌ `PageView`
   - ❌ `buttonClick`

2. **使用动词+名词结构**
   - ✅ `user_login`
   - ✅ `file_upload`
   - ❌ `login_user`
   - ❌ `upload_file`

3. **保持简洁明了**
   - ✅ `purchase`
   - ✅ `add_to_cart`
   - ❌ `user_makes_a_purchase`
   - ❌ `user_adds_item_to_shopping_cart`

4. **使用一致的命名约定**
   - 页面浏览：`page_view`
   - 点击事件：`button_click`
   - 表单提交：`form_submit`
   - 错误事件：`error`

### 属性设计原则

1. **使用驼峰命名法**
   - ✅ `userId`
   - ✅ `pageTitle`
   - ❌ `user_id`
   - ❌ `page_title`

2. **使用有意义的数据类型**
   - 数值：`duration: 1500` (毫秒)
   - 布尔：`success: true`
   - 字符串：`email: "user@example.com"`
   - 数组：`tags: ["tag1", "tag2"]`
   - 对象：`metadata: { key: "value" }`

3. **避免收集敏感信息**
   - ❌ 密码
   - ❌ 信用卡号
   - ❌ 个人身份证号
   - ❌ 详细的地理位置

4. **保持属性简洁**
   - ✅ `email: "user@example.com"`
   - ❌ `email: { address: "user@example.com", verified: true, type: "personal" }`

### 性能优化方案

1. **批量上报**
   - 收集多个事件后批量发送
   - 减少网络请求次数
   - 建议批量大小：10-50 个事件

2. **异步发送**
   - 使用异步方式发送事件
   - 不阻塞主线程
   - 使用队列机制缓存事件

3. **本地缓存**
   - 在本地缓存未发送的事件
   - 网络恢复后自动重试
   - 防止数据丢失

4. **压缩数据**
   - 对大型 JSON 数据进行压缩
   - 减少网络传输量
   - 使用 gzip 压缩

5. **错误处理**
   - 捕获并记录发送错误
   - 实现重试机制
   - 设置最大重试次数

---

## 使用示例

### Qt 前端集成

#### 1. Analytics SDK 初始化

```cpp
// AnalyticsManager.h
#ifndef ANALYTICS_MANAGER_H
#define ANALYTICS_MANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class AnalyticsManager : public QObject
{
    Q_OBJECT

public:
    explicit AnalyticsManager(QObject *parent = nullptr);
    
    void initialize(const QString &serverUrl);
    void setUserId(const QString &userId);
    void setSessionId(const QString &sessionId);
    
    void trackEvent(const QString &eventName, const QJsonObject &properties = QJsonObject());
    void trackPageView(const QString &pageUrl);
    void trackButtonClick(const QString &buttonId);
    void trackPerformance(const QString &feature, qint64 duration);
    void trackError(const QString &errorMessage);
    
    void flush();

private:
    QNetworkAccessManager *m_networkManager;
    QString m_serverUrl;
    QString m_userId;
    QString m_sessionId;
    QList<QJsonObject> m_eventQueue;
    
    void sendEvent(const QJsonObject &event);
    void sendBatchEvents();
};

#endif // ANALYTICS_MANAGER_H
```

#### 2. Analytics SDK 实现

```cpp
// AnalyticsManager.cpp
#include "AnalyticsManager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QUuid>
#include <QDateTime>

AnalyticsManager::AnalyticsManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

void AnalyticsManager::initialize(const QString &serverUrl)
{
    m_serverUrl = serverUrl;
}

void AnalyticsManager::setUserId(const QString &userId)
{
    m_userId = userId;
}

void AnalyticsManager::setSessionId(const QString &sessionId)
{
    m_sessionId = sessionId;
}

void AnalyticsManager::trackEvent(const QString &eventName, const QJsonObject &properties)
{
    QJsonObject event;
    event["event"] = eventName;
    event["eventType"] = "custom";
    event["properties"] = properties;
    event["userId"] = m_userId;
    event["sessionId"] = m_sessionId;
    
    m_eventQueue.append(event);
    
    if (m_eventQueue.size() >= 10) {
        flush();
    }
}

void AnalyticsManager::trackPageView(const QString &pageUrl)
{
    QJsonObject properties;
    properties["page_url"] = pageUrl;
    properties["page_title"] = pageUrl;
    
    QJsonObject event;
    event["event"] = "page_view";
    event["eventType"] = "page_view";
    event["properties"] = properties;
    event["userId"] = m_userId;
    event["sessionId"] = m_sessionId;
    
    m_eventQueue.append(event);
}

void AnalyticsManager::trackButtonClick(const QString &buttonId)
{
    QJsonObject properties;
    properties["button_id"] = buttonId;
    
    QJsonObject event;
    event["event"] = "button_click";
    event["eventType"] = "click";
    event["properties"] = properties;
    event["userId"] = m_userId;
    event["sessionId"] = m_sessionId;
    
    m_eventQueue.append(event);
}

void AnalyticsManager::trackPerformance(const QString &feature, qint64 duration)
{
    QJsonObject properties;
    properties["feature"] = feature;
    
    QJsonObject event;
    event["event"] = "performance";
    event["eventType"] = "performance";
    event["properties"] = properties;
    event["userId"] = m_userId;
    event["sessionId"] = m_sessionId;
    event["duration"] = duration;
    
    m_eventQueue.append(event);
}

void AnalyticsManager::trackError(const QString &errorMessage)
{
    QJsonObject event;
    event["event"] = "error";
    event["eventType"] = "error";
    event["userId"] = m_userId;
    event["sessionId"] = m_sessionId;
    event["errorMessage"] = errorMessage;
    
    m_eventQueue.append(event);
}

void AnalyticsManager::flush()
{
    if (m_eventQueue.isEmpty()) {
        return;
    }
    
    sendBatchEvents();
}

void AnalyticsManager::sendEvent(const QJsonObject &event)
{
    QJsonDocument doc(event);
    QByteArray data = doc.toJson();
    
    QNetworkRequest request(m_serverUrl + "/api/analytics/events");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, [reply]() {
        reply->deleteLater();
    });
}

void AnalyticsManager::sendBatchEvents()
{
    QJsonObject batchData;
    QJsonArray eventsArray;
    
    for (const auto &event : m_eventQueue) {
        eventsArray.append(event);
    }
    
    batchData["events"] = eventsArray;
    
    QJsonDocument doc(batchData);
    QByteArray data = doc.toJson();
    
    QNetworkRequest request(m_serverUrl + "/api/analytics/events/batch");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            m_eventQueue.clear();
        }
        reply->deleteLater();
    });
}
```

#### 3. 使用示例

```cpp
// 在应用启动时初始化
AnalyticsManager *analytics = new AnalyticsManager(this);
analytics->initialize("http://120.48.95.51:7001");
analytics->setUserId("user123");
analytics->setSessionId("session456");

// 追踪页面浏览
analytics->trackPageView("/home");

// 追踪按钮点击
analytics->trackButtonClick("submit_button");

// 追踪自定义事件
QJsonObject properties;
properties["email"] = "user@example.com";
properties["login_time"] = 1234;
analytics->trackEvent("login_success", properties);

// 追踪性能
analytics->trackPerformance("data_load", 1500);

// 追踪错误
analytics->trackError("Failed to load data");

// 刷新队列
analytics->flush();
```

### JavaScript 示例

```javascript
// 使用 fetch API
const serverUrl = 'http://120.48.95.51:7001';

// 发送单个事件
async function sendEvent(eventName, properties = {}) {
  const response = await fetch(`${serverUrl}/api/analytics/events`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      event: eventName,
      eventType: 'custom',
      properties: properties,
      userId: 'user123',
      sessionId: 'session456',
    }),
  });
  
  return response.json();
}

// 批量发送事件
async function sendBatchEvents(events) {
  const response = await fetch(`${serverUrl}/api/analytics/events/batch`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({ events }),
  });
  
  return response.json();
}

// 使用示例
sendEvent('page_view', { page_url: '/home' });
sendEvent('button_click', { button_id: 'submit' });

sendBatchEvents([
  { event: 'page_view', properties: { page_url: '/home' } },
  { event: 'button_click', properties: { button_id: 'submit' } },
]);
```

---

## 部署指南

### 本地部署

#### 开发环境

```bash
# 1. 克隆项目
git clone <repository-url>
cd egg-analytics-backend

# 2. 安装依赖
npm install

# 3. 配置数据库
# 编辑 config/config.default.js，修改数据库连接信息

# 4. 创建数据库表
psql -U postgres -d analytics_db -f migrations/init.sql

# 5. 启动开发服务器
npm run dev
```

#### 生产环境

```bash
# 1. 设置环境变量
export NODE_ENV=production

# 2. 启动应用
npm start

# 3. 查看日志
tail -f logs/egg-web.log

# 4. 停止应用
npm stop
```

### Docker 部署

#### Dockerfile

```dockerfile
FROM node:16-alpine

WORKDIR /app

COPY package*.json ./
RUN npm install --production

COPY . .

RUN npm run build

EXPOSE 7001

CMD ["npm", "start"]
```

#### docker-compose.yml

```yaml
version: '3.8'

services:
  postgres:
    image: postgres:12
    environment:
      POSTGRES_DB: analytics_db
      POSTGRES_USER: analytics_user
      POSTGRES_PASSWORD: your_password
    volumes:
      - postgres_data:/var/lib/postgresql/data
    ports:
      - "5432:5432"

  app:
    build: .
    ports:
      - "7001:7001"
    depends_on:
      - postgres
    environment:
      NODE_ENV: production
      DB_HOST: postgres
      DB_PORT: 5432
      DB_NAME: analytics_db
      DB_USER: analytics_user
      DB_PASSWORD: your_password

volumes:
  postgres_data:
```

#### 启动 Docker 服务

```bash
# 构建并启动服务
docker-compose up -d

# 查看日志
docker-compose logs -f

# 停止服务
docker-compose down
```

### Nginx 反向代理配置

```nginx
upstream analytics_backend {
    server 127.0.0.1:7001;
}

server {
    listen 80;
    server_name yourdomain.com;

    # 限制请求体大小
    client_max_body_size 10M;

    # 代理配置
    location /api/ {
        proxy_pass http://analytics_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # 超时设置
        proxy_connect_timeout 60s;
        proxy_send_timeout 60s;
        proxy_read_timeout 60s;
    }

    # 日志配置
    access_log /var/log/nginx/analytics_access.log;
    error_log /var/log/nginx/analytics_error.log;
}
```

---

## 配置说明

### config.default.js

```javascript
module.exports = appInfo => {
  return {
    // 中间件配置
    middleware: ['errorHandler'],
    
    // 安全配置
    security: {
      csrf: {
        enable: false,
      },
      domainWhiteList: ['*'],
    },
    
    // CORS 配置
    cors: {
      origin: '*',
      allowMethods: 'GET,HEAD,PUT,POST,DELETE,PATCH',
      credentials: true,
    },
    
    // PostgreSQL 配置
    sequelize: {
      dialect: 'postgres',
      host: 'localhost',
      port: 5432,
      database: 'analytics_db',
      username: 'analytics_user',
      password: 'your_password',
      timezone: '+08:00',
      define: {
        timestamps: true,
        underscored: true,
        createdAt: 'created_at',
        updatedAt: false,
      },
      pool: {
        max: 10,
        min: 0,
        acquire: 30000,
        idle: 10000,
      },
    },
    
    // 日志配置
    logger: {
      level: 'INFO',
      dir: 'logs',
      encoding: 'utf-8',
      outputJSON: false,
    },
    
    // 自定义配置
    analytics: {
      batchSize: 100,  // 批量处理大小
      maxRetry: 3,     // 最大重试次数
    },
  };
};
```

### config.prod.js

```javascript
module.exports = () => {
  return {
    // 生产环境日志级别
    logger: {
      level: 'WARN',
    },
    
    // 生产环境数据库连接池
    sequelize: {
      pool: {
        max: 20,
        min: 5,
        acquire: 60000,
        idle: 30000,
      },
    },
    
    // 生产环境 CORS 限制
    cors: {
      origin: ['https://yourdomain.com'],
    },
  };
};
```

### plugin.js

```javascript
module.exports = {
  // Sequelize ORM
  sequelize: {
    enable: true,
    package: 'egg-sequelize',
  },
  
  // PostgreSQL 驱动
  pg: {
    enable: true,
    package: 'egg-pg',
  },
  
  // CORS 支持
  cors: {
    enable: true,
    package: 'egg-cors',
  },
  
  // Redis 缓存
  redis: {
    enable: true,
    package: 'egg-redis',
  },
};
```

---

## 测试文档

### 埋点事件接口测试

#### 测试环境

- **服务器地址**: `http://120.48.95.51:7001`
- **测试时间**: 2025-12-30
- **测试工具**: curl

#### 测试用例

**TC-001: 接收单个事件**

```bash
curl -X POST http://120.48.95.51:7001/api/analytics/events \
  -H "Content-Type: application/json" \
  -d '{
    "event": "test_event",
    "eventType": "custom",
    "properties": {"test": "data"},
    "userId": "test_user_001",
    "sessionId": "test_session_001"
  }'
```

**预期结果**:
```json
{
  "success": true,
  "message": "Event recorded successfully",
  "eventId": "request-id-xxx"
}
```

**TC-002: 批量接收事件**

```bash
curl -X POST http://120.48.95.51:7001/api/analytics/events/batch \
  -H "Content-Type: application/json" \
  -d '{
    "events": [
      {"event": "page_view", "properties": {"page": "/home"}},
      {"event": "button_click", "properties": {"button": "submit"}}
    ]
  }'
```

**预期结果**:
```json
{
  "success": true,
  "message": "Successfully recorded 2 events",
  "count": 2
}
```

### 统计分析接口测试

**TC-003: 查询事件统计**

```bash
curl "http://120.48.95.51:7001/api/analytics/stats?startDate=2025-12-30&endDate=2025-12-30"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "total": 76,
    "byEventType": [...],
    "recentTrend": [...]
  }
}
```

**TC-004: 页面浏览统计**

```bash
curl "http://120.48.95.51:7001/api/analytics/page-views?startDate=2025-12-30&endDate=2025-12-30"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "pageStats": [
      {"page_url": "/", "pv": 38, "uv": 5},
      {"page_url": "/login", "pv": 38, "uv": 5}
    ],
    "totalPV": 76,
    "totalUV": 5
  }
}
```

**TC-005: 用户活跃度统计**

```bash
curl "http://120.48.95.51:7001/api/analytics/user-activity?startDate=2025-12-30&endDate=2025-12-30"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "dauStats": [{"date": "2025-12-30", "dau": 5}],
    "mauStats": [{"month": "2025-11-30T16:00:00.000Z", "mau": 5}]
  }
}
```

### 用户画像接口测试

**TC-006: 获取用户完整画像**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/profile?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "registerTime": "...",
    "lastActiveTime": "...",
    "totalEvents": 10,
    "activeDays": 1,
    "tags": {...},
    "behaviorFeatures": {...},
    "interestProfile": {...},
    "valueAssessment": {...}
  }
}
```

**TC-007: 获取用户标签**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/tags?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "tags": {
      "activityLevel": "高活跃",
      "loyaltyLevel": "忠诚用户",
      "valueLevel": "核心用户"
    },
    "updatedAt": "..."
  }
}
```

**TC-008: 获取用户行为特征**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/behavior?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "behaviorFeatures": {
      "avgSessionDuration": 300,
      "pageViewsPerSession": 5,
      "mostActiveTime": "14:00-16:00",
      "devicePreference": "desktop",
      "topPages": [...],
      "topFeatures": [...]
    },
    "updatedAt": "..."
  }
}
```

**TC-009: 获取用户兴趣画像**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/interest?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "interestProfile": {
      "pageInterests": [...],
      "featureInterests": [...]
    },
    "updatedAt": "..."
  }
}
```

**TC-010: 获取用户价值评估**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/value?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "valueAssessment": {
      "activityScore": 85,
      "loyaltyScore": 90,
      "valueScore": 88,
      "overallScore": 87.7,
      "valueLevel": "核心用户"
    },
    "updatedAt": "..."
  }
}
```

**TC-011: 获取用户列表**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/list?page=1&pageSize=20"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "users": [...],
    "total": 5,
    "page": 1,
    "pageSize": 20,
    "totalPages": 1
  }
}
```

### 测试结论

✅ 所有接口测试通过
✅ 事件总数：76 次
✅ 独立用户：5 人
✅ 页面浏览：76 次 PV / 5 人 UV
✅ 用户活跃度：DAU=5, MAU=5
✅ 用户画像功能正常
✅ 数据统计准确

---

## 常见问题

### 1. 并发请求处理

**问题**: 如何处理高并发的事件上报请求？

**解决方案**:
- 使用 Redis 队列缓冲事件
- 配置连接池大小
- 实现批量处理机制

**配置示例**:
```javascript
// config/config.default.js
config.redis = {
  client: {
    port: 6379,
    host: '127.0.0.1',
    password: '',
    db: 0,
  },
};

config.analytics = {
  batchSize: 100,
  maxRetry: 3,
  queueSize: 1000,
};
```

### 2. 数据不丢失保障

**问题**: 如何确保事件数据不丢失？

**解决方案**:
- 前端实现本地缓存和重试机制
- 使用数据库事务
- 实现失败重试逻辑

**前端重试示例**:
```javascript
async function sendEventWithRetry(event, maxRetries = 3) {
  for (let i = 0; i < maxRetries; i++) {
    try {
      const response = await fetch('/api/analytics/events', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(event),
      });
      if (response.ok) return await response.json();
    } catch (error) {
      console.error(`Retry ${i + 1}:`, error);
      await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1)));
    }
  }
  throw new Error('Failed after retries');
}
```

### 3. 查询性能优化

**问题**: 如何提高大数据量下的查询性能？

**解决方案**:
- 使用数据库索引
- 实现分区表
- 添加缓存层
- 使用物化视图

**分区表示例**:
```sql
-- 按日期分区
CREATE TABLE analytics_events (
  id BIGSERIAL PRIMARY KEY,
  event VARCHAR(100) NOT NULL,
  event_type VARCHAR(50),
  properties JSONB,
  user_id VARCHAR(100),
  session_id VARCHAR(100),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) PARTITION BY RANGE (created_at);

-- 创建分区
CREATE TABLE analytics_events_2025_12 PARTITION OF analytics_events
  FOR VALUES FROM ('2025-12-01') TO ('2026-01-01');
```

### 4. 时区处理

**问题**: 如何处理不同时区的时间数据？

**解决方案**:
- 数据库使用 UTC 存储
- 前端转换时区
- 使用标准时间格式

**配置示例**:
```javascript
// config/config.default.js
config.sequelize = {
  timezone: '+00:00',  // UTC
};
```

### 5. 实时数据分析

**问题**: 如何实现实时数据分析？

**解决方案**:
- 使用 WebSocket 推送
- 实现流式处理
- 使用 Redis Pub/Sub

### 6. 恶意请求防范

**问题**: 如何防止恶意请求？

**解决方案**:
- 实现请求频率限制
- 添加 IP 白名单
- 使用验证码

**频率限制配置**:
```javascript
// app/middleware/rateLimiter.js
module.exports = (options, app) => {
  return async function rateLimiter(ctx, next) {
    const key = `rate:${ctx.ip}`;
    const count = await app.redis.get(key);
    
    if (count && parseInt(count) > 100) {
      ctx.status = 429;
      ctx.body = { success: false, message: 'Too many requests' };
      return;
    }
    
    await app.redis.incr(key);
    await app.redis.expire(key, 60);
    
    await next();
  };
};
```

---

## 前端开发计划

### 待完成功能

1. **用户画像展示**
   - [ ] 用户标签可视化
   - [ ] 行为特征图表
   - [ ] 兴趣画像雷达图
   - [ ] 价值评估仪表盘

2. **统计分析图表**
   - [ ] 事件趋势折线图
   - [ ] 页面浏览热力图
   - [ ] 用户活跃度柱状图
   - [ ] 留存率曲线图

3. **数据导出功能**
   - [ ] CSV 导出
   - [ ] Excel 导出
   - [ ] PDF 报告生成

4. **实时监控**
   - [ ] 实时事件流
   - [ ] 系统状态监控
   - [ ] 异常告警

### 优化建议

1. **性能优化**
   - 实现数据分页加载
   - 添加虚拟滚动
   - 优化查询语句

2. **用户体验优化**
   - 添加加载动画
   - 优化错误提示
   - 改进响应式设计

3. **安全增强**
   - 添加用户认证
   - 实现权限控制
   - 加强数据加密

---

## 附录

### 相关文档

- [Egg.js 官方文档](https://eggjs.org/)
- [Sequelize 文档](https://sequelize.org/)
- [PostgreSQL 文档](https://www.postgresql.org/docs/)

### 更新记录

- 2025-12-30: 初始版本创建
- 2025-12-30: 添加测试文档和常见问题

---

**文档结束**

---

## 测试文档

### 埋点事件接口测试

#### 测试环境

- **服务器地址**: `http://120.48.95.51:7001`
- **测试时间**: 2025-12-30
- **测试工具**: curl

#### 测试用例

**TC-001: 接收单个事件**

```bash
curl -X POST http://120.48.95.51:7001/api/analytics/events \
  -H "Content-Type: application/json" \
  -d '{
    "event": "test_event",
    "eventType": "custom",
    "properties": {"test": "data"},
    "userId": "test_user_001",
    "sessionId": "test_session_001"
  }'
```

**预期结果**:
```json
{
  "success": true,
  "message": "Event recorded successfully",
  "eventId": "request-id-xxx"
}
```

**TC-002: 批量接收事件**

```bash
curl -X POST http://120.48.95.51:7001/api/analytics/events/batch \
  -H "Content-Type: application/json" \
  -d '{
    "events": [
      {"event": "page_view", "properties": {"page": "/home"}},
      {"event": "button_click", "properties": {"button": "submit"}}
    ]
  }'
```

**预期结果**:
```json
{
  "success": true,
  "message": "Successfully recorded 2 events",
  "count": 2
}
```

### 统计分析接口测试

**TC-003: 查询事件统计**

```bash
curl "http://120.48.95.51:7001/api/analytics/stats?startDate=2025-12-30&endDate=2025-12-30"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "total": 76,
    "byEventType": [...],
    "recentTrend": [...]
  }
}
```

**TC-004: 页面浏览统计**

```bash
curl "http://120.48.95.51:7001/api/analytics/page-views?startDate=2025-12-30&endDate=2025-12-30"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "pageStats": [
      {"page_url": "/", "pv": 38, "uv": 5},
      {"page_url": "/login", "pv": 38, "uv": 5}
    ],
    "totalPV": 76,
    "totalUV": 5
  }
}
```

**TC-005: 用户活跃度统计**

```bash
curl "http://120.48.95.51:7001/api/analytics/user-activity?startDate=2025-12-30&endDate=2025-12-30"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "dauStats": [{"date": "2025-12-30", "dau": 5}],
    "mauStats": [{"month": "2025-11-30T16:00:00.000Z", "mau": 5}]
  }
}
```

### 用户画像接口测试

**TC-006: 获取用户完整画像**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/profile?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "registerTime": "...",
    "lastActiveTime": "...",
    "totalEvents": 10,
    "activeDays": 1,
    "tags": {...},
    "behaviorFeatures": {...},
    "interestProfile": {...},
    "valueAssessment": {...}
  }
}
```

**TC-007: 获取用户标签**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/tags?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "tags": {
      "activityLevel": "高活跃",
      "loyaltyLevel": "忠诚用户",
      "valueLevel": "核心用户"
    },
    "updatedAt": "..."
  }
}
```

**TC-008: 获取用户行为特征**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/behavior?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "behaviorFeatures": {
      "avgSessionDuration": 300,
      "pageViewsPerSession": 5,
      "mostActiveTime": "14:00-16:00",
      "devicePreference": "desktop",
      "topPages": [...],
      "topFeatures": [...]
    },
    "updatedAt": "..."
  }
}
```

**TC-009: 获取用户兴趣画像**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/interest?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "interestProfile": {
      "pageInterests": [...],
      "featureInterests": [...]
    },
    "updatedAt": "..."
  }
}
```

**TC-010: 获取用户价值评估**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/value?userId=test_user_001"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "userId": "test_user_001",
    "valueAssessment": {
      "activityScore": 85,
      "loyaltyScore": 90,
      "valueScore": 88,
      "overallScore": 87.7,
      "valueLevel": "核心用户"
    },
    "updatedAt": "..."
  }
}
```

**TC-011: 获取用户列表**

```bash
curl "http://120.48.95.51:7001/api/analytics/user/list?page=1&pageSize=20"
```

**预期结果**:
```json
{
  "success": true,
  "data": {
    "users": [...],
    "total": 5,
    "page": 1,
    "pageSize": 20,
    "totalPages": 1
  }
}
```

### 测试结论

✅ 所有接口测试通过
✅ 事件总数：76 次
✅ 独立用户：5 人
✅ 页面浏览：76 次 PV / 5 人 UV
✅ 用户活跃度：DAU=5, MAU=5
✅ 用户画像功能正常
✅ 数据统计准确

---

## 常见问题

### 1. 并发请求处理

**问题**: 如何处理高并发的事件上报请求？

**解决方案**:
- 使用 Redis 队列缓冲事件
- 配置连接池大小
- 实现批量处理机制

**配置示例**:
```javascript
// config/config.default.js
config.redis = {
  client: {
    port: 6379,
    host: '127.0.0.1',
    password: '',
    db: 0,
  },
};

config.analytics = {
  batchSize: 100,
  maxRetry: 3,
  queueSize: 1000,
};
```

### 2. 数据不丢失保障

**问题**: 如何确保事件数据不丢失？

**解决方案**:
- 前端实现本地缓存和重试机制
- 使用数据库事务
- 实现失败重试逻辑

**前端重试示例**:
```javascript
async function sendEventWithRetry(event, maxRetries = 3) {
  for (let i = 0; i < maxRetries; i++) {
    try {
      const response = await fetch('/api/analytics/events', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(event),
      });
      if (response.ok) return await response.json();
    } catch (error) {
      console.error(`Retry ${i + 1}:`, error);
      await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1)));
    }
  }
  throw new Error('Failed after retries');
}
```

### 3. 查询性能优化

**问题**: 如何提高大数据量下的查询性能？

**解决方案**:
- 使用数据库索引
- 实现分区表
- 添加缓存层
- 使用物化视图

**分区表示例**:
```sql
-- 按日期分区
CREATE TABLE analytics_events (
  id BIGSERIAL PRIMARY KEY,
  event VARCHAR(100) NOT NULL,
  event_type VARCHAR(50),
  properties JSONB,
  user_id VARCHAR(100),
  session_id VARCHAR(100),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) PARTITION BY RANGE (created_at);

-- 创建分区
CREATE TABLE analytics_events_2025_12 PARTITION OF analytics_events
  FOR VALUES FROM ('2025-12-01') TO ('2026-01-01');
```

### 4. 时区处理

**问题**: 如何处理不同时区的时间数据？

**解决方案**:
- 数据库使用 UTC 存储
- 前端转换时区
- 使用标准时间格式

**配置示例**:
```javascript
// config/config.default.js
config.sequelize = {
  timezone: '+00:00',  // UTC
};
```

### 5. 实时数据分析

**问题**: 如何实现实时数据分析？

**解决方案**:
- 使用 WebSocket 推送
- 实现流式处理
- 使用 Redis Pub/Sub

### 6. 恶意请求防范

**问题**: 如何防止恶意请求？

**解决方案**:
- 实现请求频率限制
- 添加 IP 白名单
- 使用验证码

**频率限制配置**:
```javascript
// app/middleware/rateLimiter.js
module.exports = (options, app) => {
  return async function rateLimiter(ctx, next) {
    const key = `rate:${ctx.ip}`;
    const count = await app.redis.get(key);
    
    if (count && parseInt(count) > 100) {
      ctx.status = 429;
      ctx.body = { success: false, message: 'Too many requests' };
      return;
    }
    
    await app.redis.incr(key);
    await app.redis.expire(key, 60);
    
    await next();
  };
};
```

---

## 前端开发计划

### 待完成功能

1. **用户画像展示**
   - [ ] 用户标签可视化
   - [ ] 行为特征图表
   - [ ] 兴趣画像雷达图
   - [ ] 价值评估仪表盘

2. **统计分析图表**
   - [ ] 事件趋势折线图
   - [ ] 页面浏览热力图
   - [ ] 用户活跃度柱状图
   - [ ] 留存率曲线图

3. **数据导出功能**
   - [ ] CSV 导出
   - [ ] Excel 导出
   - [ ] PDF 报告生成

4. **实时监控**
   - [ ] 实时事件流
   - [ ] 系统状态监控
   - [ ] 异常告警

### 优化建议

1. **性能优化**
   - 实现数据分页加载
   - 添加虚拟滚动
   - 优化查询语句

2. **用户体验优化**
   - 添加加载动画
   - 优化错误提示
   - 改进响应式设计

3. **安全增强**
   - 添加用户认证
   - 实现权限控制
   - 加强数据加密

---

## 附录

### 相关文档

- [Egg.js 官方文档](https://eggjs.org/)
- [Sequelize 文档](https://sequelize.org/)
- [PostgreSQL 文档](https://www.postgresql.org/docs/)

### 更新记录

- 2025-12-30: 初始版本创建
- 2025-12-30: 添加测试文档和常见问题

---

**文档结束**

---