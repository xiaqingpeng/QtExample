#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QString>
#include <QColor>
#include <QObject>

class ThemeManager : public QObject
{
    Q_OBJECT
    
public:
    enum ThemeType {
        LIGHT,
        DARK,
        BLUE,
        GREEN
    };
    
    static ThemeManager* instance();
    
    // 主题切换
    void setTheme(ThemeType theme);
    ThemeType getCurrentTheme() const { return m_currentTheme; }
    QString getThemeName(ThemeType theme) const;
    
    // 颜色系统
    struct Colors {
        QString PRIMARY;
        QString PRIMARY_HOVER;
        QString PRIMARY_LIGHT;
        
        QString GRAY_50;
        QString GRAY_100;
        QString GRAY_200;
        QString GRAY_300;
        QString GRAY_400;
        QString GRAY_500;
        QString GRAY_600;
        QString GRAY_700;
        QString GRAY_800;
        QString GRAY_900;
        
        QString SUCCESS;
        QString WARNING;
        QString ERROR;
        QString INFO;
        
        QString BACKGROUND;
        QString SURFACE;
        QString CARD;
        
        QString TEXT_PRIMARY;
        QString TEXT_SECONDARY;
        QString TEXT_DISABLED;
        
        QString BORDER;
        QString DIVIDER;
    };
    
    // 间距系统
    struct Spacing {
        static const int XS = 4;
        static const int SM = 8;
        static const int MD = 16;
        static const int LG = 24;
        static const int XL = 32;
        static const int XXL = 48;
    };
    
    // 圆角系统
    struct BorderRadius {
        static const int SM = 4;
        static const int MD = 8;
        static const int LG = 12;
        static const int XL = 16;
        static const int FULL = 9999;
    };
    
    // 阴影系统
    struct Shadow {
        static const QString SM;
        static const QString MD;
        static const QString LG;
        static const QString XL;
    };
    
    // 字体系统
    struct Typography {
        static const QString FONT_FAMILY;
        static const int FONT_SIZE_XS = 12;
        static const int FONT_SIZE_SM = 14;
        static const int FONT_SIZE_MD = 16;
        static const int FONT_SIZE_LG = 18;
        static const int FONT_SIZE_XL = 20;
        static const int FONT_SIZE_XXL = 24;
        static const int FONT_SIZE_XXXL = 32;
    };
    
    // 获取当前主题颜色
    const Colors& colors() const { return m_colors; }
    
    // 组件样式
    QString getButtonStyle(const QString& variant = "primary") const;
    QString getCardStyle() const;
    QString getInputStyle() const;
    QString getListStyle() const;
    QString getScrollBarStyle() const;
    QString getTabStyle() const;
    QString getMenuStyle() const;
    QString getToolbarStyle() const;
    QString getStatusBarStyle() const;
    QString getDialogStyle() const;
    QString getNavigationStyle() const;
    QString getThemeSwitcherStyle() const;
    
signals:
    void themeChanged(ThemeType theme);
    
private:
    ThemeManager(QObject *parent = nullptr);
    void updateColors();
    void loadLightTheme();
    void loadDarkTheme();
    void loadBlueTheme();
    void loadGreenTheme();
    
    static ThemeManager* m_instance;
    ThemeType m_currentTheme;
    Colors m_colors;
};

#endif // THEME_MANAGER_H