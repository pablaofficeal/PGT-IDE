# Система тем Pabla IDE

## Описание

Система тем позволяет легко настраивать внешний вид Pabla IDE через JSON файлы. Все темы хранятся в файле `themes.json` и могут быть легко добавлены или изменены.

## Структура JSON файла

```json
{
  "themes": {
    "Название темы": {
      "name": "Отображаемое название",
      "description": "Описание темы",
      "styles": {
        "QWidget": {
          "property": "value"
        }
      }
    }
  }
}
```

## Поддерживаемые виджеты

- `QTextEdit` - основной редактор кода
- `QTreeView` - дерево файлов
- `QPlainTextEdit` - терминал
- `QMainWindow` - главное окно
- `QMenuBar` - панель меню
- `QMenu` - выпадающие меню
- `QToolBar` - панель инструментов
- `QDockWidget` - доки (панели)

## Поддерживаемые CSS свойства

- `background-color` - цвет фона
- `color` - цвет текста
- `border` - границы
- `font-family` - шрифт
- `font-size` - размер шрифта
- `padding` - отступы
- `margin` - поля

## Примеры цветов

### HEX цвета
```json
"background-color": "#2b2b2b"
"color": "#ffffff"
```

### RGB цвета
```json
"background-color": "rgb(14, 0, 86)"
"color": "rgb(255, 255, 255)"
```

### Названия цветов
```json
"background-color": "black"
"color": "white"
```

## Добавление новой темы

1. Откройте файл `themes.json`
2. Добавьте новую секцию в объект `themes`
3. Укажите название, описание и стили
4. Сохраните файл
5. Перезапустите приложение

## Использование в коде

```cpp
// Получение списка тем
QStringList themes = themeManager->getAvailableThemes();

// Применение темы
themeManager->applyTheme("Название темы");

// Проверка существования темы
if (themeManager->themeExists("Название темы")) {
    // Тема существует
}
```

## Команды терминала

- `themes` - показать список доступных тем
- `start theme <название>` - применить тему
- `help` - показать справку по командам 