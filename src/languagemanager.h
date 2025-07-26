#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QApplication>

class LanguageManager : public QObject
{
    Q_OBJECT

public:
    explicit LanguageManager(QObject *parent = nullptr);
    
    // Загрузка языков из JSON файлов
    bool loadLanguages(const QString &languageCode = "ru");
    
    // Применение языка
    bool applyLanguage(const QString &languageCode);
    
    // Получение перевода по ключу
    QString tr(const QString &key, const QString &section = "interface") const;
    
    // Получение перевода с подстановкой параметров
    QString tr(const QString &key, const QStringList &params, const QString &section = "messages") const;
    
    // Получение списка доступных языков
    QStringList getAvailableLanguages() const;
    
    // Получение названия языка
    QString getLanguageName(const QString &languageCode) const;
    
    // Проверка существования языка
    bool languageExists(const QString &languageCode) const;
    
    // Получение текущего языка
    QString getCurrentLanguage() const;

private:
    QMap<QString, QJsonObject> languages;
    QString currentLanguage;
    
    // Поиск файла языка
    QString findLanguageFile(const QString &languageCode) const;
    
    // Подстановка параметров в строку
    QString replaceParams(const QString &text, const QStringList &params) const;
};

#endif // LANGUAGEMANAGER_H 