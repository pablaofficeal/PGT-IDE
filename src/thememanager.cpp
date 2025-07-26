#include "thememanager.h"
#include <QDebug>
#include <QJsonParseError>
#include <QDir>
#include <QCoreApplication>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
    loadThemes();
}

bool ThemeManager::loadThemes(const QString &filePath)
{
    QString actualPath = filePath;
    
    // Если путь не указан, ищем файл в нескольких местах
    if (filePath.isEmpty() || filePath == "src/assets/themes/themes.json") {
        QStringList searchPaths;
        
        // 1. Относительно исполняемого файла
        QString exeDir = QCoreApplication::applicationDirPath();
        searchPaths << exeDir + "/../src/assets/themes/themes.json";
        searchPaths << exeDir + "/../../src/assets/themes/themes.json";
        searchPaths << exeDir + "/../../../src/assets/themes/themes.json";
        
        // 2. Относительно рабочей директории
        QString currentDir = QDir::currentPath();
        searchPaths << currentDir + "/src/assets/themes/themes.json";
        searchPaths << currentDir + "/../src/assets/themes/themes.json";
        
        // 3. Абсолютный путь к проекту (если запускаем из CLion)
        searchPaths << "C:/Users/pavlo/CLionProjects/untitled20/src/assets/themes/themes.json";
        
        // Ищем первый существующий файл
        for (const QString &path : searchPaths) {
            if (QFile::exists(path)) {
                actualPath = path;
                qDebug() << "Найден файл тем:" << actualPath;
                break;
            }
        }
    }
    
    QFile file(actualPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл тем:" << actualPath;
        qWarning() << "Попробуйте запустить приложение из корневой папки проекта";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Ошибка парсинга JSON:" << error.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("themes")) {
        qWarning() << "Файл не содержит секцию 'themes'";
        return false;
    }

    QJsonObject themesObj = root["themes"].toObject();
    themes.clear();

    for (auto it = themesObj.begin(); it != themesObj.end(); ++it) {
        QString themeName = it.key();
        QJsonObject themeData = it.value().toObject();
        themes[themeName] = themeData;
    }

    qDebug() << "Загружено тем:" << themes.size();
    return true;
}

bool ThemeManager::applyTheme(const QString &themeName)
{
    if (!themes.contains(themeName)) {
        qWarning() << "Тема не найдена:" << themeName;
        return false;
    }

    QJsonObject themeData = themes[themeName];
    if (!themeData.contains("styles")) {
        qWarning() << "Тема не содержит стили:" << themeName;
        return false;
    }

    QJsonObject styles = themeData["styles"].toObject();
    QString css = stylesToCSS(styles);
    
    qApp->setStyleSheet(css);
    
    qDebug() << "Применена тема:" << themeName;
    return true;
}

QStringList ThemeManager::getAvailableThemes() const
{
    return themes.keys();
}

QString ThemeManager::getThemeDescription(const QString &themeName) const
{
    if (!themes.contains(themeName)) {
        return QString();
    }

    QJsonObject themeData = themes[themeName];
    if (themeData.contains("description")) {
        return themeData["description"].toString();
    }
    
    return QString();
}

bool ThemeManager::themeExists(const QString &themeName) const
{
    return themes.contains(themeName);
}

QString ThemeManager::stylesToCSS(const QJsonObject &styles) const
{
    QString css;
    
    for (auto it = styles.begin(); it != styles.end(); ++it) {
        QString selector = it.key();
        QJsonObject properties = it.value().toObject();
        
        css += selector + " {\n";
        
        for (auto propIt = properties.begin(); propIt != properties.end(); ++propIt) {
            QString property = propIt.key();
            QString value = propIt.value().toString();
            css += "    " + property + ": " + value + ";\n";
        }
        
        css += "}\n";
    }
    
    return css;
} 