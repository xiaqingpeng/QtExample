#ifndef USERPROFILETAB_H
#define USERPROFILETAB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QProgressBar>
#include <QWebEngineView>
#include <QTimer>

class UserProfileTab : public QWidget
{
    Q_OBJECT

public:
    explicit UserProfileTab(QWidget *parent = nullptr);
    ~UserProfileTab();

    // 设置用户ID
    void setUserId(const QString &userId);

private slots:
    // 刷新用户画像数据
    void refreshUserProfile();
    // 导出用户画像
    void exportUserProfile();

private:
    // UI初始化方法
    void setupUI();
    void setupUserInfoCard();
    void setupUserTags();
    void setupBehaviorStats();
    void setupInterestAnalysis();
    void setupValueAssessment();
    void setupToolbar();

    // 数据加载方法
    void loadUserList();
    void loadUserProfileData();
    void updateUserInfoDisplay(const QJsonObject &userInfo);
    void updateUserTagsDisplay(const QJsonArray &tags);
    void updateBehaviorStatsDisplay(const QJsonObject &behaviorStats);
    void updateInterestAnalysisDisplay(const QJsonArray &interests);
    void updateValueAssessmentDisplay(const QJsonObject &valueAssessment);

    // 图表渲染方法
    void renderValueRadarChart(const QJsonObject &valueData);
    
    // 辅助方法
    QWidget *createToolbarWidget();
    QWidget *createUserInfoWidget();
    QWidget *createUserTagsWidget();
    QWidget *createBehaviorStatsWidget();
    QWidget *createInterestAnalysisWidget();
    QWidget *createValueAssessmentWidget();
    QGroupBox *createGroupBox(const QString &title, QWidget *content);

    // UI组件
    QLabel *m_userIdLabel;
    QLabel *m_registerTimeLabel;
    QLabel *m_lastActiveTimeLabel;
    QLabel *m_totalEventsLabel;
    QLabel *m_activeDaysLabel;
    
    QLabel *m_activityTagLabel;
    QLabel *m_loyaltyTagLabel;
    QLabel *m_valueTagLabel;
    
    QLabel *m_visitCountLabel;
    QLabel *m_avgStayTimeLabel;
    QLabel *m_topPagesLabel;
    QLabel *m_topFeaturesLabel;
    
    QWebEngineView *m_interestChartView;
    QWebEngineView *m_valueRadarView;
    
    QPushButton *m_refreshButton;
    QPushButton *m_exportButton;
    QPushButton *m_prevUserButton;
    QPushButton *m_nextUserButton;
    
    QString m_currentUserId;
    QStringList m_userIdList;
    int m_currentUserIndex;
    
    // 定时刷新定时器
    QTimer *m_refreshTimer;
};

#endif // USERPROFILETAB_H