#ifndef LAYOUTEXAMPLESTAB3_H
#define LAYOUTEXAMPLESTAB3_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QScrollArea>
#include <QLabel>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QUrl>

class LayoutExamplesTab3 : public QWidget
{
    Q_OBJECT
public:
    explicit LayoutExamplesTab3(QWidget *parent = nullptr);

private slots:
    void onApiDataReceived(QNetworkReply *reply);

private:
    void createDashboardLayout();
    void createMultiPanelLayout();
    void createCollapsiblePanelLayout();
    void createResponsiveFormLayout();
    void createApiDataTable();
    
    QVBoxLayout *mainLayout;
    QNetworkAccessManager *networkManager;
    QTableWidget *apiDataTable;
};

#endif // LAYOUTEXAMPLESTAB3_H