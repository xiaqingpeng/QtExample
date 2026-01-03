# 主题切换按钮颜色同步修复

## 问题描述
切换导航栏主题时，ECharts示例页面和日志统计页面的按钮主题颜色没有同步更新。

## 问题原因
在`applyTheme()`方法中调用`updateButtonHighlight(-2)`时，方法直接返回而没有应用新的主题样式到按钮上。

## 修复内容

### 1. EChartsTab修复
- 在`echartstab.h`中添加了`m_currentHighlightedDays`成员变量来跟踪当前高亮状态
- 在构造函数中初始化`m_currentHighlightedDays = 0`
- 修复`updateButtonHighlight()`方法中对`-2`特殊值的处理，使其能够保持当前高亮状态并应用新主题
- 在每个按钮高亮case中更新`m_currentHighlightedDays`的值

### 2. LogStatsTab修复
- 在`logstatstab.h`中添加了`m_currentHighlightedDays`成员变量
- 在构造函数中初始化`m_currentHighlightedDays = 0`
- 修复`updateButtonHighlight()`方法中对`-2`特殊值的处理
- 在每个按钮高亮条件中更新`m_currentHighlightedDays`的值

## 修复后的行为
1. 主题切换时，`applyTheme()`调用`updateButtonHighlight(-2)`
2. 方法检测到`-2`特殊值，保持当前高亮状态不变
3. 根据`m_currentHighlightedDays`的值重新应用正确的主题样式到所有按钮
4. 当前高亮的按钮使用高亮样式，其他按钮使用普通样式
5. 所有样式都使用新主题的颜色

## 测试方法
1. 编译应用程序
2. 打开ECharts示例页面或日志统计页面
3. 点击任意时间快捷按钮（如"今天"、"昨天"等）
4. 切换主题（亮色/暗色）
5. 验证按钮颜色是否正确更新，高亮状态是否保持

## 相关文件
- `echarts/echartstab.h`
- `echarts/echartstab.cpp`
- `echarts/logstatstab.h`
- `echarts/logstatstab.cpp`