#include "basiccontrolstab.h"

BasicControlsTab::BasicControlsTab(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    
    createButtonGroup();
    createInputGroup();
    createSelectionGroup();
}

void BasicControlsTab::createButtonGroup()
{
    QGroupBox *buttonGroup = new QGroupBox("按钮控件");
    QGridLayout *buttonLayout = new QGridLayout(buttonGroup);
    
    QPushButton *standardBtn = new QPushButton("标准按钮");
    QPushButton *iconBtn = new QPushButton("图标按钮");
    iconBtn->setIcon(QIcon::fromTheme("document-open"));
    QPushButton *flatBtn = new QPushButton("扁平按钮");
    flatBtn->setFlat(true);
    QToolButton *toolBtn = new QToolButton();
    toolBtn->setText("工具按钮");
    toolBtn->setIcon(QIcon::fromTheme("edit-copy"));
    toolBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    buttonLayout->addWidget(standardBtn, 0, 0);
    buttonLayout->addWidget(iconBtn, 0, 1);
    buttonLayout->addWidget(flatBtn, 1, 0);
    buttonLayout->addWidget(toolBtn, 1, 1);
    mainLayout->addWidget(buttonGroup);
}

void BasicControlsTab::createInputGroup()
{
    QGroupBox *inputGroup = new QGroupBox("输入控件");
    QFormLayout *inputLayout = new QFormLayout(inputGroup);
    
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("单行文本输入...");
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setPlaceholderText("多行富文本输入...");
    QSpinBox *spinBox = new QSpinBox();
    spinBox->setRange(0, 100);
    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setRange(0.0, 10.0);
    doubleSpinBox->setSingleStep(0.1);
    
    inputLayout->addRow("文本输入框:", lineEdit);
    inputLayout->addRow("富文本编辑:", textEdit);
    inputLayout->addRow("整数微调框:", spinBox);
    inputLayout->addRow("浮点微调框:", doubleSpinBox);
    mainLayout->addWidget(inputGroup);
}

void BasicControlsTab::createSelectionGroup()
{
    QGroupBox *selectionGroup = new QGroupBox("选择控件");
    QGridLayout *selectionLayout = new QGridLayout(selectionGroup);
    
    QCheckBox *checkBox1 = new QCheckBox("选项1");
    QCheckBox *checkBox2 = new QCheckBox("选项2");
    QRadioButton *radio1 = new QRadioButton("单选1");
    QRadioButton *radio2 = new QRadioButton("单选2");
    radio1->setChecked(true);
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems({"选项A", "选项B", "选项C"});
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    
    selectionLayout->addWidget(checkBox1, 0, 0);
    selectionLayout->addWidget(checkBox2, 0, 1);
    selectionLayout->addWidget(radio1, 1, 0);
    selectionLayout->addWidget(radio2, 1, 1);
    selectionLayout->addWidget(new QLabel("下拉框:"), 2, 0);
    selectionLayout->addWidget(comboBox, 2, 1);
    selectionLayout->addWidget(new QLabel("滑块:"), 3, 0);
    selectionLayout->addWidget(slider, 3, 1);
    mainLayout->addWidget(selectionGroup);
}