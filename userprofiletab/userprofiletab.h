#ifndef USERPROFILETAB_H
#define USERPROFILETAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QProgressBar>
#include <QPushButton>
#include <QDateEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>

class UserProfileTab : public QWidget
{
    Q_OBJECT

public:
    explicit UserProfileTab(QWidget *parent = nullptr);

private slots:
    void onRefreshClicked();
    void onDateRangeChanged();
    void onUserSelected(const QString &userId);

private:
    void setupUI();
    void setupUserInfoCard();
    void setupUserTags();
    void setupBehaviorStats();
    void setupInterestAnalysis();
    void setupValueAssessment();
    void loadUserProfile(const QString &userId);
    void updateUserInfoCard();
    void updateUserTags();
    void updateBehaviorStats();
    void updateInterestAnalysis();
    void updateValueAssessment();

    // UI组件
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;

    // 用户信息卡片
    QGroupBox *userInfoGroup;
    QLabel *userIdLabel;
    QLabel *registerTimeLabel;
    QLabel *lastActiveTimeLabel;
    QLabel *lifecycleStatusLabel;

    // 用户标签
    QGroupBox *tagsGroup;
    QLabel *activityTagLabel;
    QLabel *loyaltyTagLabel;
    QLabel *valueTagLabel;

    // 行为统计
    QGroupBox *behaviorGroup;
    QLabel *visitCountLabel;
    QLabel *avgSessionLabel;
    QTableWidget *topPagesTable;
    QTableWidget *topFeaturesTable;

    // 兴趣分析
    QGroupBox *interestGroup;
    QLabel *pageInterestLabel;
    QLabel *featurePreferenceLabel;

    // 价值评估
    QGroupBox *valueGroup;
    QProgressBar *activityProgressBar;
    QProgressBar *loyaltyProgressBar;
    QProgressBar *behaviorProgressBar;
    QProgressBar *timeProgressBar;
    QLabel *totalScoreLabel;
    QLabel *levelLabel;

    // 工具栏
    QHBoxLayout *toolbarLayout;
    QLineEdit *userIdEdit;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QPushButton *refreshButton;
    QPushButton *searchButton;

    QString currentUserId;
};

#endif // USERPROFILETAB_H