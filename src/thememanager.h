#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QApplication>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(QObject *parent = nullptr);
    
    // Загрузка тем из JSON файла
    bool loadThemes(const QString &filePath = "");
    
    // Применение темы по имени
    bool applyTheme(const QString &themeName);
    
    // Получение списка доступных тем
    QStringList getAvailableThemes() const;
    
    // Получение описания темы
    QString getThemeDescription(const QString &themeName) const;
    
    // Проверка существования темы
    bool themeExists(const QString &themeName) const;

private:
    QMap<QString, QJsonObject> themes;
    
    // Преобразование JSON стилей в CSS строку
    QString stylesToCSS(const QJsonObject &styles) const;
};

#endif // THEMEMANAGER_H 