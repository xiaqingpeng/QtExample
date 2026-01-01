#include "theme_manager.h"
#include <QSettings>

ThemeManager* ThemeManager::m_instance = nullptr;

// 阴影定义
const QString ThemeManager::Shadow::SM = "0 1px 2px 0 rgba(0, 0, 0, 0.05)";
const QString ThemeManager::Shadow::MD = "0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06)";
const QString ThemeManager::Shadow::LG = "0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05)";
const QString ThemeManager::Shadow::XL = "0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04)";

// 字体定义
const QString ThemeManager::Typography::FONT_FAMILY = "SF Pro Text";

ThemeManager::ThemeManager(QObject *parent) : QObject(parent), m_currentTheme(LIGHT)
{
    // 从设置中加载主题
    QSettings settings;
    int themeIndex = settings.value("theme", static_cast<int>(LIGHT)).toInt();
    m_currentTheme = static_cast<ThemeType>(themeIndex);
    updateColors();
}

ThemeManager* ThemeManager::instance()
{
    if (!m_instance) {
        m_instance = new ThemeManager();
    }
    return m_instance;
}

void ThemeManager::setTheme(ThemeType theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        updateColors();
        
        // 保存主题设置
        QSettings settings;
        settings.setValue("theme", static_cast<int>(theme));
        
        emit themeChanged(theme);
    }
}

QString ThemeManager::getThemeName(ThemeType theme) const
{
    switch (theme) {
        case LIGHT: return "浅色主题";
        case DARK: return "深色主题";
        case BLUE: return "蓝色主题";
        case GREEN: return "绿色主题";
        default: return "浅色主题";
    }
}

void ThemeManager::updateColors()
{
    switch (m_currentTheme) {
        case LIGHT:
            loadLightTheme();
            break;
        case DARK:
            loadDarkTheme();
            break;
        case BLUE:
            loadBlueTheme();
            break;
        case GREEN:
            loadGreenTheme();
            break;
    }
}

void ThemeManager::loadLightTheme()
{
    m_colors.PRIMARY = "#3b82f6";
    m_colors.PRIMARY_HOVER = "#2563eb";
    m_colors.PRIMARY_LIGHT = "#dbeafe";
    
    m_colors.GRAY_50 = "#f9fafb";
    m_colors.GRAY_100 = "#f3f4f6";
    m_colors.GRAY_200 = "#e5e7eb";
    m_colors.GRAY_300 = "#d1d5db";
    m_colors.GRAY_400 = "#9ca3af";
    m_colors.GRAY_500 = "#6b7280";
    m_colors.GRAY_600 = "#4b5563";
    m_colors.GRAY_700 = "#374151";
    m_colors.GRAY_800 = "#1f2937";
    m_colors.GRAY_900 = "#111827";
    
    m_colors.SUCCESS = "#10b981";
    m_colors.WARNING = "#f59e0b";
    m_colors.ERROR = "#ef4444";
    m_colors.INFO = "#3b82f6";
    
    m_colors.BACKGROUND = "#ffffff";
    m_colors.SURFACE = "#f8fafc";
    m_colors.CARD = "#ffffff";
    
    m_colors.TEXT_PRIMARY = "#1f2937";
    m_colors.TEXT_SECONDARY = "#6b7280";
    m_colors.TEXT_DISABLED = "#9ca3af";
    
    m_colors.BORDER = "#e5e7eb";
    m_colors.DIVIDER = "#f3f4f6";
}

void ThemeManager::loadDarkTheme()
{
    m_colors.PRIMARY = "#60a5fa";
    m_colors.PRIMARY_HOVER = "#3b82f6";
    m_colors.PRIMARY_LIGHT = "#1e3a8a";
    
    m_colors.GRAY_50 = "#18181b";
    m_colors.GRAY_100 = "#27272a";
    m_colors.GRAY_200 = "#3f3f46";
    m_colors.GRAY_300 = "#52525b";
    m_colors.GRAY_400 = "#71717a";
    m_colors.GRAY_500 = "#a1a1aa";
    m_colors.GRAY_600 = "#d4d4d8";
    m_colors.GRAY_700 = "#e4e4e7";
    m_colors.GRAY_800 = "#f4f4f5";
    m_colors.GRAY_900 = "#fafafa";
    
    m_colors.SUCCESS = "#22c55e";
    m_colors.WARNING = "#eab308";
    m_colors.ERROR = "#f87171";
    m_colors.INFO = "#60a5fa";
    
    m_colors.BACKGROUND = "#09090b";
    m_colors.SURFACE = "#18181b";
    m_colors.CARD = "#27272a";
    
    m_colors.TEXT_PRIMARY = "#fafafa";
    m_colors.TEXT_SECONDARY = "#a1a1aa";
    m_colors.TEXT_DISABLED = "#71717a";
    
    m_colors.BORDER = "#3f3f46";
    m_colors.DIVIDER = "#27272a";
}

void ThemeManager::loadBlueTheme()
{
    m_colors.PRIMARY = "#0ea5e9";
    m_colors.PRIMARY_HOVER = "#0284c7";
    m_colors.PRIMARY_LIGHT = "#e0f2fe";
    
    m_colors.GRAY_50 = "#f0f9ff";
    m_colors.GRAY_100 = "#e0f2fe";
    m_colors.GRAY_200 = "#bae6fd";
    m_colors.GRAY_300 = "#7dd3fc";
    m_colors.GRAY_400 = "#38bdf8";
    m_colors.GRAY_500 = "#0ea5e9";
    m_colors.GRAY_600 = "#0284c7";
    m_colors.GRAY_700 = "#0369a1";
    m_colors.GRAY_800 = "#075985";
    m_colors.GRAY_900 = "#0c4a6e";
    
    m_colors.SUCCESS = "#10b981";
    m_colors.WARNING = "#f59e0b";
    m_colors.ERROR = "#ef4444";
    m_colors.INFO = "#0ea5e9";
    
    m_colors.BACKGROUND = "#ffffff";
    m_colors.SURFACE = "#f0f9ff";
    m_colors.CARD = "#ffffff";
    
    m_colors.TEXT_PRIMARY = "#0c4a6e";
    m_colors.TEXT_SECONDARY = "#0369a1";
    m_colors.TEXT_DISABLED = "#7dd3fc";
    
    m_colors.BORDER = "#bae6fd";
    m_colors.DIVIDER = "#e0f2fe";
}

void ThemeManager::loadGreenTheme()
{
    m_colors.PRIMARY = "#10b981";
    m_colors.PRIMARY_HOVER = "#059669";
    m_colors.PRIMARY_LIGHT = "#d1fae5";
    
    m_colors.GRAY_50 = "#f0fdf4";
    m_colors.GRAY_100 = "#dcfce7";
    m_colors.GRAY_200 = "#bbf7d0";
    m_colors.GRAY_300 = "#86efac";
    m_colors.GRAY_400 = "#4ade80";
    m_colors.GRAY_500 = "#22c55e";
    m_colors.GRAY_600 = "#16a34a";
    m_colors.GRAY_700 = "#15803d";
    m_colors.GRAY_800 = "#166534";
    m_colors.GRAY_900 = "#14532d";
    
    m_colors.SUCCESS = "#10b981";
    m_colors.WARNING = "#f59e0b";
    m_colors.ERROR = "#ef4444";
    m_colors.INFO = "#3b82f6";
    
    m_colors.BACKGROUND = "#ffffff";
    m_colors.SURFACE = "#f0fdf4";
    m_colors.CARD = "#ffffff";
    
    m_colors.TEXT_PRIMARY = "#14532d";
    m_colors.TEXT_SECONDARY = "#15803d";
    m_colors.TEXT_DISABLED = "#86efac";
    
    m_colors.BORDER = "#bbf7d0";
    m_colors.DIVIDER = "#dcfce7";
}

QString ThemeManager::getButtonStyle(const QString& variant) const
{
    QString baseStyle = QString(
        "QPushButton { "
        "    font-family: %1; "
        "    font-size: %2px; "
        "    font-weight: 500; "
        "    padding: %3px %4px; "
        "    border-radius: %5px; "
        "    border: 1px solid transparent; "
        "} "
        "QPushButton:disabled { "
        "    opacity: 0.5; "
        "}"
    ).arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_SM)
     .arg(Spacing::SM)
     .arg(Spacing::MD)
     .arg(BorderRadius::MD);
    
    if (variant == "primary") {
        return baseStyle + QString(
            "QPushButton { "
            "    background-color: %1; "
            "    color: white; "
            "} "
            "QPushButton:hover { "
            "    background-color: %2; "
            "} "
            "QPushButton:pressed { "
            "    background-color: %3; "
            "}"
        ).arg(m_colors.PRIMARY)
         .arg(m_colors.PRIMARY_HOVER)
         .arg(m_colors.PRIMARY_HOVER);
    } else if (variant == "secondary") {
        return baseStyle + QString(
            "QPushButton { "
            "    background-color: %1; "
            "    color: %2; "
            "    border: 1px solid %3; "
            "} "
            "QPushButton:hover { "
            "    background-color: %4; "
            "} "
            "QPushButton:pressed { "
            "    background-color: %5; "
            "}"
        ).arg(m_colors.BACKGROUND)
         .arg(m_colors.TEXT_PRIMARY)
         .arg(m_colors.BORDER)
         .arg(m_colors.GRAY_50)
         .arg(m_colors.GRAY_100);
    } else if (variant == "ghost") {
        return baseStyle + QString(
            "QPushButton { "
            "    background-color: transparent; "
            "    color: %1; "
            "} "
            "QPushButton:hover { "
            "    background-color: %2; "
            "} "
            "QPushButton:pressed { "
            "    background-color: %3; "
            "}"
        ).arg(m_colors.TEXT_PRIMARY)
         .arg(m_colors.GRAY_100)
         .arg(m_colors.GRAY_200);
    }
    
    return baseStyle;
}

QString ThemeManager::getCardStyle() const
{
    return QString(
        "QWidget { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: %3px; "
        "    padding: %4px; "
        "}"
    ).arg(m_colors.CARD)
     .arg(m_colors.BORDER)
     .arg(BorderRadius::LG)
     .arg(Spacing::LG);
}

QString ThemeManager::getInputStyle() const
{
    return QString(
        "QLineEdit, QTextEdit, QPlainTextEdit { "
        "    font-family: %1; "
        "    font-size: %2px; "
        "    padding: %3px %4px; "
        "    border: 1px solid %5; "
        "    border-radius: %6px; "
        "    background-color: %7; "
        "    color: %8; "
        "} "
        "QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus { "
        "    border-color: %9; "
        "    outline: none; "
        "} "
        "QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled { "
        "    background-color: %10; "
        "    color: %11; "
        "}"
    ).arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_SM)
     .arg(Spacing::SM)
     .arg(Spacing::MD)
     .arg(m_colors.BORDER)
     .arg(BorderRadius::MD)
     .arg(m_colors.BACKGROUND)
     .arg(m_colors.TEXT_PRIMARY)
     .arg(m_colors.PRIMARY)
     .arg(m_colors.GRAY_100)
     .arg(m_colors.TEXT_DISABLED);
}

QString ThemeManager::getListStyle() const
{
    return QString(
        "QListWidget { "
        "    background-color: %1; "
        "    border: none; "
        "    border-radius: %2px; "
        "    padding: %3px; "
        "} "
        "QListWidget::item { "
        "    padding: %4px %5px; "
        "    margin: %6px 0; "
        "    border-radius: %7px; "
        "    color: %8; "
        "    font-family: %9; "
        "    font-size: %10px; "
        "} "
        "QListWidget::item:hover { "
        "    background-color: %11; "
        "} "
        "QListWidget::item:selected { "
        "    background-color: %12; "
        "    color: white; "
        "    font-weight: 500; "
        "}"
    ).arg(m_colors.BACKGROUND)
     .arg(BorderRadius::LG)
     .arg(Spacing::SM)
     .arg(Spacing::MD)
     .arg(Spacing::LG)
     .arg(2)
     .arg(BorderRadius::MD)
     .arg(m_colors.TEXT_SECONDARY)
     .arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_SM)
     .arg(m_colors.GRAY_100)
     .arg(m_colors.PRIMARY);
}

QString ThemeManager::getScrollBarStyle() const
{
    return QString(
        "QScrollBar:vertical { "
        "    background: %1; "
        "    width: 8px; "
        "    border-radius: 4px; "
        "    margin: 0; "
        "} "
        "QScrollBar::handle:vertical { "
        "    background: %2; "
        "    border-radius: 4px; "
        "    min-height: 20px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "    background: %3; "
        "} "
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "    height: 0px; "
        "} "
        "QScrollBar:horizontal { "
        "    background: %4; "
        "    height: 8px; "
        "    border-radius: 4px; "
        "    margin: 0; "
        "} "
        "QScrollBar::handle:horizontal { "
        "    background: %5; "
        "    border-radius: 4px; "
        "    min-width: 20px; "
        "} "
        "QScrollBar::handle:horizontal:hover { "
        "    background: %6; "
        "} "
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { "
        "    width: 0px; "
        "}"
    ).arg(m_colors.GRAY_100)
     .arg(m_colors.GRAY_400)
     .arg(m_colors.GRAY_500)
     .arg(m_colors.GRAY_100)
     .arg(m_colors.GRAY_400)
     .arg(m_colors.GRAY_500);
}

QString ThemeManager::getNavigationStyle() const
{
    return QString(
        "QWidget#navigationBar { "
        "    background-color: %1; "
        "    border-bottom: 1px solid %2; "
        "    padding: %3px %4px; "
        "} "
        "QLabel#usernameLabel { "
        "    color: %5; "
        "    font-family: %6; "
        "    font-size: %7px; "
        "    font-weight: 600; "
        "    padding: 0 %8px; "
        "} "
        "QLabel#avatarLabel { "
        "    border: 2px solid %9; "
        "    border-radius: 20px; "
        "}"
    ).arg(m_colors.SURFACE)
     .arg(m_colors.BORDER)
     .arg(Spacing::SM)
     .arg(Spacing::LG)
     .arg(m_colors.TEXT_PRIMARY)
     .arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_MD)
     .arg(Spacing::MD)
     .arg(m_colors.PRIMARY);
}

QString ThemeManager::getThemeSwitcherStyle() const
{
    return QString(
        "QComboBox { "
        "    font-family: %1; "
        "    font-size: %2px; "
        "    padding: %3px %4px; "
        "    border: 1px solid %5; "
        "    border-radius: %6px; "
        "    background-color: %7; "
        "    color: %8; "
        "    min-width: 100px; "
        "} "
        "QComboBox:hover { "
        "    border-color: %9; "
        "} "
        "QComboBox::drop-down { "
        "    border: none; "
        "    width: 20px; "
        "} "
        "QComboBox::down-arrow { "
        "    width: 12px; "
        "    height: 12px; "
        "} "
        "QComboBox QAbstractItemView { "
        "    background-color: %10; "
        "    border: 1px solid %11; "
        "    border-radius: %12px; "
        "    selection-background-color: %13; "
        "    color: %14; "
        "}"
    ).arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_SM)
     .arg(Spacing::SM)
     .arg(Spacing::MD)
     .arg(m_colors.BORDER)
     .arg(BorderRadius::MD)
     .arg(m_colors.BACKGROUND)
     .arg(m_colors.TEXT_PRIMARY)
     .arg(m_colors.PRIMARY)
     .arg(m_colors.CARD)
     .arg(m_colors.BORDER)
     .arg(BorderRadius::MD)
     .arg(m_colors.PRIMARY_LIGHT)
     .arg(m_colors.TEXT_PRIMARY);
}

QString ThemeManager::getMenuStyle() const
{
    return QString(
        "QListWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 %1, stop:1 %2); "
        "    border: none; "
        "    border-right: 1px solid %3; "
        "    padding: %4px %5px; "
        "} "
        "QListWidget::item { "
        "    padding: %6px %7px; "
        "    margin: %8px 0; "
        "    color: %9; "
        "    border-radius: %10px; "
        "    font-size: %11px; "
        "    font-weight: 500; "
        "    background-color: transparent; "
        "    border: 1px solid transparent; "
        "} "
        "QListWidget::item:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %12, stop:1 %13); "
        "    color: %14; "
        "    border: 1px solid %15; "
        "} "
        "QListWidget::item:selected { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %16, stop:1 %17); "
        "    color: white; "
        "    font-weight: 600; "
        "    border: 1px solid %18; "
        "}"
    ).arg(m_colors.SURFACE)
     .arg(m_colors.DIVIDER)
     .arg(m_colors.BORDER)
     .arg(Spacing::MD)
     .arg(Spacing::SM)
     .arg(Spacing::MD)
     .arg(Spacing::LG)
     .arg(2)
     .arg(m_colors.TEXT_SECONDARY)
     .arg(BorderRadius::LG)
     .arg(Typography::FONT_SIZE_SM)
     .arg(m_colors.GRAY_100)
     .arg(m_colors.DIVIDER)
     .arg(m_colors.TEXT_PRIMARY)
     .arg(m_colors.BORDER)
     .arg(m_colors.PRIMARY)
     .arg(m_colors.PRIMARY_HOVER)
     .arg(m_colors.PRIMARY);
}

QString ThemeManager::getStatusBarStyle() const
{
    return QString(
        "QWidget#statusBar { "
        "    background-color: %1; "
        "    border-top: 1px solid %2; "
        "    padding: %3px %4px; "
        "} "
        "QLabel#statusIndicator { "
        "    border-radius: 4px; "
        "} "
        "QLabel#statusText, QLabel#statusMessage { "
        "    color: %5; "
        "    font-family: %6; "
        "    font-size: %7px; "
        "}"
    ).arg(m_colors.SURFACE)
     .arg(m_colors.BORDER)
     .arg(Spacing::SM)
     .arg(Spacing::LG)
     .arg(m_colors.TEXT_SECONDARY)
     .arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_XS);
}

QString ThemeManager::getDialogStyle() const
{
    return QString(
        "QDialog { "
        "    background-color: %1; "
        "    border-radius: %2px; "
        "} "
        "QDialog QLabel { "
        "    color: %3; "
        "    font-family: %4; "
        "    font-size: %5px; "
        "} "
        "QDialog QLabel[class='title'] { "
        "    font-size: %6px; "
        "    font-weight: 600; "
        "    color: %7; "
        "}"
    ).arg(m_colors.BACKGROUND)
     .arg(BorderRadius::LG)
     .arg(m_colors.TEXT_SECONDARY)
     .arg(Typography::FONT_FAMILY)
     .arg(Typography::FONT_SIZE_SM)
     .arg(Typography::FONT_SIZE_LG)
     .arg(m_colors.TEXT_PRIMARY);
}