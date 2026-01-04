#ifndef MODERN_THEME_H
#define MODERN_THEME_H

#include <QString>

namespace ModernTheme {
    // 颜色定义
    namespace Colors {
        // 主色调
        const QString PRIMARY_BLUE = "#3b82f6";
        const QString PRIMARY_BLUE_DARK = "#1d4ed8";
        const QString PRIMARY_BLUE_LIGHT = "#dbeafe";
        
        // 中性色
        const QString GRAY_50 = "#f8fafc";
        const QString GRAY_100 = "#f1f5f9";
        const QString GRAY_200 = "#e2e8f0";
        const QString GRAY_300 = "#cbd5e1";
        const QString GRAY_400 = "#94a3b8";
        const QString GRAY_500 = "#64748b";
        const QString GRAY_600 = "#475569";
        const QString GRAY_700 = "#334155";
        const QString GRAY_800 = "#1e293b";
        const QString GRAY_900 = "#0f172a";
        
        // 状态色
        const QString SUCCESS = "#10b981";
        const QString WARNING = "#f59e0b";
        const QString ERROR = "#ef4444";
        const QString INFO = "#06b6d4";
        
        // 背景色
        const QString WHITE = "#ffffff";
        const QString BACKGROUND = "#f8fafc";
    }
    
    // 字体大小
    namespace FontSizes {
        const int SMALL = 12;
        const int MEDIUM = 14;
        const int LARGE = 16;
        const int XLARGE = 18;
    }
    
    // 间距
    namespace Spacing {
        const int XS = 4;
        const int SM = 8;
        const int MD = 12;
        const int LG = 16;
        const int XL = 20;
        const int XXL = 24;
    }
    
    // 圆角
    namespace BorderRadius {
        const int SM = 6;
        const int MD = 8;
        const int LG = 10;
        const int XL = 12;
        const int FULL = 9999;
    }
    
    // 阴影
    namespace Shadows {
        const QString SM = "0 1px 2px rgba(0, 0, 0, 0.05)";
        const QString MD = "0 4px 6px rgba(0, 0, 0, 0.1)";
        const QString LG = "0 10px 15px rgba(0, 0, 0, 0.1)";
        const QString XL = "0 20px 25px rgba(0, 0, 0, 0.1)";
    }
}

#endif // MODERN_THEME_H