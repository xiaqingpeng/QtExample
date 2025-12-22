#ifndef DIALOGSTAB_H
#define DIALOGSTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QScrollArea>
#include <QLabel>


class DialogsTab : public QWidget
{
    Q_OBJECT
public:
    explicit DialogsTab(QWidget *parent = nullptr);

private slots:
    void showMessageDialog();
    void showFileDialog();
    void showColorDialog();
    void showFontDialog();
    void showInputDialog();

private:
    void createDialogButtons();
    void createScrollArea();
    
    QVBoxLayout *mainLayout;
};

#endif // DIALOGSTAB_H