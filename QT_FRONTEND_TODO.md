# Qt前端需要做的事情

---

## 阶段一：数据展示界面开发

### 📊 图表展示页面（已有基础，需扩展）

**EChartsTab扩展：基于现有的echartstab.cpp，添加更多图表类型**

- [ ] 添加图表类型切换（折线图、柱状图、饼图、雷达图等）
- [ ] 添加图表配置面板（颜色、标题、图例等）
- [ ] 添加图表导出功能（PNG、PDF）
- [ ] 添加图表数据刷新功能

### 👤 用户画像展示页面（新建）

**UserProfileTab：展示单个用户的详细画像**

- [ ] 用户基础信息卡片（用户ID、注册时间、最后活跃时间）
- [ ] 用户标签展示（活跃度、忠诚度、价值标签）
- [ ] 用户行为统计（访问次数、停留时长、常用功能）
- [ ] 用户兴趣分析（页面偏好、功能偏好）
- [ ] 用户价值评估雷达图

### 📈 统计报表页面（新建）

**DashboardTab：综合数据看板**

- [ ] 关键指标卡片（DAU、MAU、留存率、转化率）
- [ ] 实时数据监控（当前在线用户、今日事件数）
- [ ] 趋势图表（用户增长、活跃度趋势）
- [ ] Top排行榜（热门页面、热门事件、活跃用户）

---

## 阶段二：数据请求功能

### 🌐 API调用封装

```cpp
// 在NetworkManager中添加统计分析API
class NetworkManager {
public:
    // 获取用户活跃度统计
    void getUserActivityStats(const QDate& startDate, const QDate& endDate);
    
    // 获取用户画像数据
    void getUserProfile(const QString& userId);
    
    // 获取留存率统计
    void getRetentionStats(int days);
    
    // 获取页面访问统计
    void getPageViewStats(const QDate& startDate, const QDate& endDate);
    
    // 获取事件统计
    void getEventStats(const QString& eventType, const QDate& startDate, const QDate& endDate);
    
    // 获取用户列表
    void getUserList(int page, int pageSize, const QString& filter);
};
```

### 🔍 数据筛选功能

- [ ] 时间范围筛选（日期选择器）
- [ ] 事件类型筛选（下拉框）
- [ ] 用户群体筛选（活跃度、价值等级）
- [ ] 设备类型筛选（操作系统、平台）
- [ ] 地域筛选（IP地址、地区）

---

## 阶段三：图表渲染功能

### 📉 ECharts图表配置

```javascript
// 在chart.html中添加图表类型配置
const chartTypes = {
    // 折线图 - 趋势分析
    line: {
        title: '用户活跃度趋势',
        xAxis: '时间',
        yAxis: '用户数',
        series: ['DAU', 'MAU']
    },
    
    // 柱状图 - 对比分析
    bar: {
        title: '页面访问量统计',
        xAxis: '页面名称',
        yAxis: '访问次数',
        series: ['PV', 'UV']
    },
    
    // 饼图 - 占比分析
    pie: {
        title: '设备类型分布',
        data: ['macOS', 'Windows', 'Linux']
    },
    
    // 雷达图 - 多维度分析
    radar: {
        title: '用户价值评估',
        indicators: ['活跃度', '忠诚度', '价值', '影响力', '贡献度']
    },
    
    // 桑基图 - 用户路径
    sankey: {
        title: '用户页面流转路径',
        nodes: ['登录页', '首页', '个人中心', '设置页'],
        links: []
    },
    
    // 漏斗图 - 转化分析
    funnel: {
        title: '登录流程转化',
        data: ['访问登录页', '输入账号', '点击登录', '登录成功']
    }
};
```

### 🎨 图表交互功能

- [ ] 图表缩放和平移
- [ ] 数据点点击查看详情
- [ ] 图例切换显示/隐藏
- [ ] 图表类型切换
- [ ] 数据导出（CSV、Excel）

---

## 阶段四：用户画像展示

### 📋 用户信息卡片

```cpp
// UserProfileTab实现
class UserProfileTab : public QMainWindow {
private:
    void setupUserInfoCard() {
        // 用户ID
        // 注册时间
        // 最后活跃时间
        // 用户生命周期状态
    }
    
    void setupUserTags() {
        // 活跃度标签（高活跃/中活跃/低活跃）
        // 忠诚度标签（忠诚/普通/新用户）
        // 价值标签（高价值/中价值/低价值）
    }
    
    void setupBehaviorStats() {
        // 访问次数统计
        // 平均会话时长
        // 常用页面TOP10
        // 常用功能TOP5
    }
    
    void setupInterestAnalysis() {
        // 页面兴趣分布饼图
        // 功能偏好柱状图
        // 兴趣变化趋势图
    }
    
    void setupValueAssessment() {
        // 用户价值雷达图
        // 价值评分展示
        // 价值等级展示
    }
};
```

---

## 阶段五：实时数据更新

### ⏱️ 定时刷新功能

```cpp
// 在DashboardTab中添加定时刷新
QTimer *refreshTimer = new QTimer(this);
connect(refreshTimer, &QTimer::timeout, this, &DashboardTab::refreshData);
refreshTimer->start(30000); // 每30秒刷新一次

void DashboardTab::refreshData() {
    // 刷新关键指标
    // 刷新实时数据
    // 刷新趋势图表
}
```

### 🔄 数据推送（可选）

- [ ] 使用WebSocket实现实时数据推送
- [ ] 实现订阅机制，只推送变化的数据