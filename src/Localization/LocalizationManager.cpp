#include "LocalizationManager.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QLocale>
#include <QDebug>

LocalizationManager* LocalizationManager::s_instance = nullptr;

LocalizationManager::LocalizationManager(QObject *parent)
    : QObject(parent)
    , m_translator(nullptr)
    , m_initialized(false)
{
}

LocalizationManager::~LocalizationManager()
{
    if (m_translator) {
        QApplication::removeTranslator(m_translator);
        delete m_translator;
    }
}

LocalizationManager* LocalizationManager::instance()
{
    if (!s_instance) {
        s_instance = new LocalizationManager();
    }
    return s_instance;
}

void LocalizationManager::initialize()
{
    if (m_initialized) {
        return;
    }

    qDebug() << "Initializing LocalizationManager...";

    loadLanguageInfo();
    loadTranslations();

    // 从设置中加载保存的语言
    QSettings settings;
    QString savedLanguage = settings.value("language", QLocale::system().name()).toString();
    
    if (isLanguageAvailable(savedLanguage)) {
        setLanguage(savedLanguage);
    } else {
        // 如果保存的语言不可用，使用系统默认语言或英语
        QString systemLanguage = QLocale::system().name();
        if (isLanguageAvailable(systemLanguage)) {
            setLanguage(systemLanguage);
        } else {
            setLanguage("en_US"); // 默认英语
        }
    }

    m_initialized = true;
    qDebug() << "LocalizationManager initialized, current language:" << m_currentLanguage;
}

void LocalizationManager::setLanguage(const QString& languageCode)
{
    if (m_currentLanguage == languageCode) {
        return;
    }

    if (!isLanguageAvailable(languageCode)) {
        qWarning() << "Language not available:" << languageCode;
        return;
    }

    qDebug() << "Changing language from" << m_currentLanguage << "to" << languageCode;

    // 移除旧的翻译器
    if (m_translator) {
        QApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    // 加载新的翻译文件
    QString translationFile = getTranslationFilePath(languageCode);
    if (QFile::exists(translationFile)) {
        m_translator = new QTranslator(this);
        if (m_translator->load(translationFile)) {
            QApplication::installTranslator(m_translator);
            qDebug() << "Translation loaded successfully:" << translationFile;
        } else {
            qWarning() << "Failed to load translation:" << translationFile;
            delete m_translator;
            m_translator = nullptr;
        }
    } else {
        qDebug() << "Translation file not found:" << translationFile;
    }

    m_currentLanguage = languageCode;

    // 保存语言设置
    QSettings settings;
    settings.setValue("language", languageCode);

    emit languageChanged(languageCode);
}

QString LocalizationManager::currentLanguage() const
{
    return m_currentLanguage;
}

QStringList LocalizationManager::availableLanguages() const
{
    return m_availableLanguages;
}

QString LocalizationManager::getLanguageDisplayName(const QString& languageCode) const
{
    return m_languageDisplayNames.value(languageCode, languageCode);
}

QString LocalizationManager::translate(const QString& key, const QString& context) const
{
    if (m_translator) {
        QString translated = m_translator->translate(context.toUtf8().constData(), key.toUtf8().constData());
        if (!translated.isEmpty()) {
            return translated;
        }
    }
    
    // 如果没有找到翻译，返回原始键值
    return key;
}

QString LocalizationManager::translate(const QString& key, const QStringList& args, const QString& context) const
{
    QString translated = translate(key, context);
    
    // 替换参数占位符 %1, %2, %3...
    for (int i = 0; i < args.size(); ++i) {
        translated = translated.arg(args[i]);
    }
    
    return translated;
}

bool LocalizationManager::isLanguageAvailable(const QString& languageCode) const
{
    return m_availableLanguages.contains(languageCode);
}

void LocalizationManager::loadTranslations()
{
    // 扫描翻译文件目录
    QString translationsDir = QApplication::applicationDirPath() + "/translations";
    QDir dir(translationsDir);
    
    if (!dir.exists()) {
        // 尝试资源文件中的翻译
        translationsDir = ":/translations";
        dir.setPath(translationsDir);
    }

    qDebug() << "Loading translations from:" << translationsDir;

    QStringList filters;
    filters << "app_*.qm";
    QStringList translationFiles = dir.entryList(filters, QDir::Files);

    m_availableLanguages.clear();
    for (const QString& file : translationFiles) {
        // 从文件名提取语言代码：app_zh_CN.qm -> zh_CN
        QString baseName = QFileInfo(file).baseName(); // app_zh_CN
        QString languageCode = baseName.mid(4); // zh_CN
        
        if (!languageCode.isEmpty()) {
            m_availableLanguages.append(languageCode);
            qDebug() << "Found translation for language:" << languageCode;
        }
    }

    // 总是添加英语作为默认语言
    if (!m_availableLanguages.contains("en_US")) {
        m_availableLanguages.append("en_US");
    }
}

void LocalizationManager::loadLanguageInfo()
{
    // 设置语言显示名称
    m_languageDisplayNames.clear();
    m_languageDisplayNames["zh_CN"] = "简体中文";
    m_languageDisplayNames["zh_TW"] = "繁體中文";
    m_languageDisplayNames["en_US"] = "English";
    m_languageDisplayNames["ja_JP"] = "日本語";
    m_languageDisplayNames["ko_KR"] = "한국어";
    m_languageDisplayNames["fr_FR"] = "Français";
    m_languageDisplayNames["de_DE"] = "Deutsch";
    m_languageDisplayNames["es_ES"] = "Español";
    m_languageDisplayNames["ru_RU"] = "Русский";
}

QString LocalizationManager::getTranslationFilePath(const QString& languageCode) const
{
    QString fileName = QString("app_%1.qm").arg(languageCode);
    
    // 首先尝试应用程序目录
    QString appDirPath = QApplication::applicationDirPath() + "/translations/" + fileName;
    if (QFile::exists(appDirPath)) {
        return appDirPath;
    }
    
    // 然后尝试资源文件
    QString resourcePath = QString(":/translations/%1").arg(fileName);
    if (QFile::exists(resourcePath)) {
        return resourcePath;
    }
    
    return QString();
}