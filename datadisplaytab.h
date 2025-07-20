#ifndef DATADISPLAYTAB_H
#define DATADISPLAYTAB_H

#include <QWidget>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QHeaderView>

class DataDisplayTab : public QWidget
{
    Q_OBJECT
public:
    explicit DataDisplayTab(QWidget *parent = nullptr);

private:
    void createListGroup();
    void createTableGroup();
    void createTreeGroup();
    
    QHBoxLayout *mainLayout;
};

#endif // DATADISPLAYTAB_H