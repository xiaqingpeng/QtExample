#ifndef LOCALIZATIONMANAGER_H
#define LOCALIZATIONMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QHash>
#include <QString>
#include <QStringList>

/**
 * @brief 本地化管理器
 * 
 * 负责应用程序的多语言支持和本地化功能
 */
class LocalizationManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
    static LocalizationManager* instance();

    /**
     * @brief 初始化本地化管理器
     */
    void initialize();

    /**
     * @brief 设置语言
     * @param languageCode 语言代码（如：zh_CN, en_US）
     */
    void setLanguage(const QString& languageCode);

    /**
     * @brief 获取当前语言
     * @return 当前语言代码
     */
    QString currentLanguage() const;

    /**
     * @brief 获取可用语言列表
     * @return 语言代码列表
     */
    QStringList availableLanguages() const;

    /**
     * @brief 获取语言显示名称
     * @param languageCode 语言代码
     * @return 显示名称
     */
    QString getLanguageDisplayName(const QString& languageCode) const;

    /**
     * @brief 翻译文本
     * @param key 翻译键
     * @param context 上下文（可选）
     * @return 翻译后的文本
     */
    QString translate(const QString& key, const QString& context = QString()) const;

    /**
     * @brief 翻译文本（带参数）
     * @param key 翻译键
     * @param args 参数列表
     * @param context 上下文（可选）
     * @return 翻译后的文本
     */
    QString translate(const QString& key, const QStringList& args, const QString& context = QString()) const;

    /**
     * @brief 检查语言是否可用
     * @param languageCode 语言代码
     * @return 如果可用返回true
     */
    bool isLanguageAvailable(const QString& languageCode) const;

signals:
    void languageChanged(const QString& languageCode);

private:
    explicit LocalizationManager(QObject *parent = nullptr);
    ~LocalizationManager();

    void loadTranslations();
    void loadLanguageInfo();
    QString getTranslationFilePath(const QString& languageCode) const;
    QString normalizeLanguageCode(const QString& languageCode) const;

private:
    static LocalizationManager* s_instance;
    QTranslator* m_translator;
    QString m_currentLanguage;
    QStringList m_availableLanguages;
    QHash<QString, QString> m_languageDisplayNames;
    bool m_initialized;
};

// 便捷的翻译宏 - 使用不同的名称避免与Qt内置tr冲突
#define TR(key) LocalizationManager::instance()->translate(key)
#define TRN(key, args) LocalizationManager::instance()->translate(key, args)
#define TRC(key, context) LocalizationManager::instance()->translate(key, context)

#endif // LOCALIZATIONMANAGER_H