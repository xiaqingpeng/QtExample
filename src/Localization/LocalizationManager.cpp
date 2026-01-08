#include "LocalizationManager.h"
#include <QCoreApplication>
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
        QCoreApplication::removeTranslator(m_translator);
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

    // qDebug() << "Initializing LocalizationManager...";

    loadLanguageInfo();
    loadTranslations();

    // 从设置中加载保存的语言
    QSettings settings;
    QString savedLanguage = settings.value("language", QLocale::system().name()).toString();
    
    // 规范化语言代码
    savedLanguage = normalizeLanguageCode(savedLanguage);
    
    if (isLanguageAvailable(savedLanguage)) {
        setLanguage(savedLanguage);
    } else {
        // 如果保存的语言不可用，使用系统默认语言或英语
        QString systemLanguage = normalizeLanguageCode(QLocale::system().name());
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

    // qDebug() << "Changing language from" << m_currentLanguage << "to" << languageCode;

    // 移除旧的翻译器
    if (m_translator) {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    // 加载新的翻译文件
    QString translationFile = getTranslationFilePath(languageCode);
    if (!translationFile.isEmpty() && QFile::exists(translationFile)) {
        m_translator = new QTranslator(this);
        if (m_translator->load(translationFile)) {
            QCoreApplication::installTranslator(m_translator);
            qDebug() << "Translation loaded successfully:" << translationFile;
        } else {
            qWarning() << "Failed to load translation file:" << translationFile;
            delete m_translator;
            m_translator = nullptr;
        }
    } else {
        if (translationFile.isEmpty()) {
            qWarning() << "Translation file path is empty for language:" << languageCode;
        } else {
            qWarning() << "Translation file not found:" << translationFile;
        }
    }

    m_currentLanguage = languageCode;

    // 保存语言设置
    QSettings settings;
    settings.setValue("language", languageCode);

    // 发送语言变化事件（Qt会自动发送给所有窗口）
    // 各个窗口需要在 changeEvent() 中处理 QEvent::LanguageChange 事件
    QCoreApplication::postEvent(QCoreApplication::instance(), new QEvent(QEvent::LanguageChange));
    
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
    QString translationsDir = QCoreApplication::applicationDirPath() + "/translations";
    QDir dir(translationsDir);
    
    if (!dir.exists()) {
        // 尝试资源文件中的翻译
        translationsDir = ":/translations";
        dir.setPath(translationsDir);
    }

    // qDebug() << "Loading translations from:" << translationsDir;

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
            // qDebug() << "Found translation for language:" << languageCode;
        }
    }

    // 确保常用语言始终可选（即使缺少对应的qm文件，也可以通过代码内字符串实现基础多语言）
    const QStringList defaultLanguages = {
        QStringLiteral("zh_CN"),
        QStringLiteral("en_US"),
        QStringLiteral("ja_JP"),
        QStringLiteral("ko_KR")
    };

    for (const QString &code : defaultLanguages) {
        if (!m_availableLanguages.contains(code)) {
            m_availableLanguages.append(code);
        }
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
    QString appDir = QCoreApplication::applicationDirPath();
    
    qDebug() << "Looking for translation file:" << fileName;
    qDebug() << "Application directory:" << appDir;
    
    // 1. 尝试应用程序目录下的 translations 子目录（最常见）
    QString appDirPath = appDir + "/translations/" + fileName;
    qDebug() << "Checking path 1:" << appDirPath << "exists:" << QFile::exists(appDirPath);
    if (QFile::exists(appDirPath)) {
        qDebug() << "Found translation file at:" << appDirPath;
        return appDirPath;
    }
    
    // 2. 尝试构建目录中的 translations 子目录
    QString buildDirPath = appDir + "/../translations/" + fileName;
    qDebug() << "Checking path 2:" << buildDirPath << "exists:" << QFile::exists(buildDirPath);
    if (QFile::exists(buildDirPath)) {
        qDebug() << "Found translation file at:" << buildDirPath;
        return buildDirPath;
    }
    
    // 3. 尝试可执行文件同级目录
    QString sameDirPath = appDir + "/" + fileName;
    qDebug() << "Checking path 3:" << sameDirPath << "exists:" << QFile::exists(sameDirPath);
    if (QFile::exists(sameDirPath)) {
        qDebug() << "Found translation file at:" << sameDirPath;
        return sameDirPath;
    }
    
    // 4. 尝试资源文件
    QString resourcePath = QString(":/translations/%1").arg(fileName);
    qDebug() << "Checking path 4:" << resourcePath << "exists:" << QFile::exists(resourcePath);
    if (QFile::exists(resourcePath)) {
        qDebug() << "Found translation file at:" << resourcePath;
        return resourcePath;
    }
    
    // 5. 尝试源代码目录（开发时可能用到）
    QString sourcePath = QCoreApplication::applicationDirPath() + "/../../src/Localization/translations/" + fileName;
    qDebug() << "Checking path 5:" << sourcePath << "exists:" << QFile::exists(sourcePath);
    if (QFile::exists(sourcePath)) {
        qDebug() << "Found translation file at:" << sourcePath;
        return sourcePath;
    }
    
    qWarning() << "Translation file not found:" << fileName;
    return QString();
}

QString LocalizationManager::normalizeLanguageCode(const QString& languageCode) const
{
    QString code = languageCode.trimmed();
    
    // 处理空字符串
    if (code.isEmpty()) {
        return "en_US";
    }
    
    // 处理常见的无效语言代码
    if (code.startsWith("en")) {
        // 所有英语变体都映射到 en_US（包括 en_CN 这样的错误代码）
        if (code == "en_US" || code == "en_GB" || code == "en_CA" || code == "en_AU") {
            return "en_US";
        }
        // 处理错误的英语代码如 en_CN
        return "en_US";
    }
    
    if (code.startsWith("zh")) {
        // 中文变体处理
        if (code.contains("TW") || code.contains("HK") || code.contains("MO")) {
            return "zh_TW"; // 繁体中文
        } else if (code.contains("CN") || code == "zh") {
            return "zh_CN"; // 简体中文
        } else {
            return "zh_CN"; // 默认简体中文
        }
    }
    
    if (code.startsWith("ja")) {
        return "ja_JP";
    }
    
    if (code.startsWith("ko")) {
        return "ko_KR";
    }
    
    // 如果是已知的完整语言代码，直接返回
    const QStringList knownCodes = {"zh_CN", "zh_TW", "en_US", "ja_JP", "ko_KR"};
    if (knownCodes.contains(code)) {
        return code;
    }
    
    // 默认返回英语
    return "en_US";
}