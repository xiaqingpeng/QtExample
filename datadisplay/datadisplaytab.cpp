#include "datadisplaytab.h"

DataDisplayTab::DataDisplayTab(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QHBoxLayout(this);
    
    createListGroup();
    createTableGroup();
    createTreeGroup();
}

void DataDisplayTab::createListGroup()
{
    QGroupBox *listGroup = new QGroupBox("列表控件");
    QVBoxLayout *listLayout = new QVBoxLayout(listGroup);
    
    QListWidget *listWidget = new QListWidget();
    listWidget->addItems({"项目1", "项目2", "项目3", "项目4"});
    listLayout->addWidget(listWidget);
    mainLayout->addWidget(listGroup);
}

void DataDisplayTab::createTableGroup()
{
    QGroupBox *tableGroup = new QGroupBox("表格控件");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    
    QTableWidget *tableWidget = new QTableWidget(5, 3);
    tableWidget->setHorizontalHeaderLabels({"姓名", "年龄", "职业"});
    tableWidget->verticalHeader()->setVisible(false);
    
    QStringList names = {"张三", "李四", "王五", "赵六", "钱七"};
    for (int row = 0; row < 5; ++row) {
        tableWidget->setItem(row, 0, new QTableWidgetItem(names[row]));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(25 + row)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(row % 2 == 0 ? "工程师" : "设计师"));
    }
    tableLayout->addWidget(tableWidget);
    mainLayout->addWidget(tableGroup);
}

void DataDisplayTab::createTreeGroup()
{
    QGroupBox *treeGroup = new QGroupBox("树形控件");
    QVBoxLayout *treeLayout = new QVBoxLayout(treeGroup);
    
    QTreeWidget *treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabels({"名称", "类型"});
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(treeWidget, {"根节点", "文件夹"});
    new QTreeWidgetItem(rootItem, {"文档", "文件夹"});
    new QTreeWidgetItem(rootItem, {"图片", "文件夹"});
    QTreeWidgetItem *childItem = new QTreeWidgetItem(rootItem, {"项目", "文件夹"});
    new QTreeWidgetItem(childItem, {"main.cpp", "源代码"});
    new QTreeWidgetItem(childItem, {"widget.h", "头文件"});
    treeWidget->expandAll();
    treeLayout->addWidget(treeWidget);
    mainLayout->addWidget(treeGroup);
}