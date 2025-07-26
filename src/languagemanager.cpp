#include "languagemanager.h"
#include <QDebug>
#include <QJsonParseError>
#include <QDir>
#include <QCoreApplication>

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent), currentLanguage("ru")
{
    loadLanguages();
}

bool LanguageManager::loadLanguages(const QString &languageCode)
{
    QStringList availableLanguages = {"ru", "en"};
    
    for (const QString &lang : availableLanguages) {
        QString filePath = findLanguageFile(lang);
        if (filePath.isEmpty()) {
            qWarning() << "Не удалось найти файл языка:" << lang;
            continue;
        }
        
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Не удалось открыть файл языка:" << filePath;
            continue;
        }
        
        QByteArray data = file.readAll();
        file.close();
        
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "Ошибка парсинга JSON для языка" << lang << ":" << error.errorString();
            continue;
        }
        
        languages[lang] = doc.object();
        qDebug() << "Загружен язык:" << lang;
    }
    
    if (languages.isEmpty()) {
        qWarning() << "Не удалось загрузить ни одного языка!";
        return false;
    }
    
    // Применяем указанный язык
    if (!languageCode.isEmpty()) {
        applyLanguage(languageCode);
    }
    
    return true;
}

bool LanguageManager::applyLanguage(const QString &languageCode)
{
    if (!languages.contains(languageCode)) {
        qWarning() << "Язык не найден:" << languageCode;
        return false;
    }
    
    currentLanguage = languageCode;
    qDebug() << "Применен язык:" << languageCode;
    return true;
}

QString LanguageManager::tr(const QString &key, const QString &section) const
{
    if (!languages.contains(currentLanguage)) {
        return key; // Возвращаем ключ если язык не загружен
    }
    
    QJsonObject langData = languages[currentLanguage];
    if (!langData.contains(section)) {
        return key;
    }
    
    QJsonObject sectionData = langData[section].toObject();
    if (!sectionData.contains(key)) {
        return key;
    }
    
    return sectionData[key].toString();
}

QString LanguageManager::tr(const QString &key, const QStringList &params, const QString &section) const
{
    QString text = tr(key, section);
    return replaceParams(text, params);
}

QStringList LanguageManager::getAvailableLanguages() const
{
    return languages.keys();
}

QString LanguageManager::getLanguageName(const QString &languageCode) const
{
    if (!languages.contains(languageCode)) {
        return languageCode;
    }
    
    QJsonObject langData = languages[languageCode];
    if (!langData.contains("languages")) {
        return languageCode;
    }
    
    QJsonObject languagesObj = langData["languages"].toObject();
    if (!languagesObj.contains(languageCode)) {
        return languageCode;
    }
    
    return languagesObj[languageCode].toString();
}

bool LanguageManager::languageExists(const QString &languageCode) const
{
    return languages.contains(languageCode);
}

QString LanguageManager::getCurrentLanguage() const
{
    return currentLanguage;
}

QString LanguageManager::findLanguageFile(const QString &languageCode) const
{
    QStringList searchPaths;
    
    // 1. Относительно исполняемого файла
    QString exeDir = QCoreApplication::applicationDirPath();
    searchPaths << exeDir + "/../src/assets/language/language_" + languageCode + ".json";
    searchPaths << exeDir + "/../../src/assets/language/language_" + languageCode + ".json";
    searchPaths << exeDir + "/../../../src/assets/language/language_" + languageCode + ".json";
    searchPaths << exeDir + "/assets/language/language_" + languageCode + ".json";
    
    // 2. Относительно рабочей директории
    QString currentDir = QDir::currentPath();
    searchPaths << currentDir + "/src/assets/language/language_" + languageCode + ".json";
    searchPaths << currentDir + "/../src/assets/language/language_" + languageCode + ".json";
    
    // 3. Абсолютный путь к проекту
    searchPaths << "C:/Users/pavlo/CLionProjects/untitled20/src/assets/language/language_" + languageCode + ".json";
    
    // Ищем первый существующий файл
    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }
    
    return QString();
}

QString LanguageManager::replaceParams(const QString &text, const QStringList &params) const
{
    QString result = text;
    for (int i = 0; i < params.size(); ++i) {
        result.replace(QString("{%1}").arg(i), params[i]);
    }
    return result;
} 