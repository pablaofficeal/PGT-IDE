# Pabla IDE

Pabla IDE — это простая среда разработки на Qt для редактирования, подсветки синтаксиса, управления файлами и сборки CMake-проектов.

## Возможности

- Редактирование текста с подсветкой синтаксиса
- Открытие/сохранение файлов и папок
- Дерево файлов проекта
- Терминал для ввода команд
- Быстрая сборка и запуск CMake-проекта
- Гибкая система тем через JSON файлы (5 встроенных тем + возможность добавления новых)
- Поддержка языков интерфейса (русский и английский)
- Горячие клавиши:  
  - <kbd>Ctrl+S</kbd> — сохранить  
  - <kbd>Ctrl+Z</kbd> — отмена

## Быстрый старт

1. **Склонируйте репозиторий:**
   ```sh
   git clone https://github.com/pablaofficeal/PGT-IDEA
   cd untitled20
   ```

2. **Установите зависимости:**
   - Qt 6.9.1 (или совместимую версию)
   - CMake 3.30+
   - Ninja (или другой генератор CMake)
   - Компилятор MinGW (или другой совместимый)

3. **Сборка:**
   
   **Для Windows (рекомендуется):**
   ```cmd
   build.bat
   ```
   
   **Или используя Python:**
   ```sh
   python build.py
   ```
   
   **Или вручную:**
   ```sh
   # Установка переменной PATH для компилятора
   $env:PATH = "C:/gcc2/mingw64/bin;$env:PATH"
   
   # Генерация файлов сборки
   cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64"
   
   # Сборка проекта
   cmake --build build
   ```
   
   Подробные инструкции см. в файле [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md).

4. **Запуск:**
   ```cmd
   run.bat
   ```
   Или вручную:
   ```cmd
   cd build
   untitled20.exe
   ```
   Или просто дважды кликните на `build/untitled20.exe`.

## Использование

- Открывайте файлы и папки через меню "Файл".
- Используйте терминал для команд, например:
  - `help` — список команд
  - `themes` — показать доступные темы
  - `start theme <название>` — применить тему
  - `languages` — показать доступные языки
  - `language <код>` — изменить язык (ru/en)
- Для сборки и запуска используйте кнопки на панели инструментов.
- Темами и языками можно управлять через меню "Файл" → "Настройки" (выпадающие списки с предварительным просмотром).

## Управление темами

### Через терминал
```bash
themes                    # показать список тем
start theme "Тёмная тема" # применить тему
languages                 # показать доступные языки
language en               # переключиться на английский
```

### Через скрипт
```bash
python add_theme.py list  # показать существующие темы
python add_theme.py add   # добавить новую тему
python add_language.py list  # показать существующие языки
python add_language.py add   # добавить новый язык
```

### Через меню
Файл → Настройки → Выберите тему и язык из выпадающих списков с предварительным просмотром

## Структура проекта

- `main.cpp` — основной код редактора
- [`syntaxhighlighter.cpp`](syntaxhighlighter.cpp) — подсветка синтаксиса
- [`keypresshandler.cpp`](keypresshandler.cpp) — обработка горячих клавиш
- [`thememanager.cpp`](src/thememanager.cpp) — система управления темами
- [`languagemanager.cpp`](src/languagemanager.cpp) — система управления языками
- [`codeeditorwindow.cpp`](codeeditorwindow.cpp) — (альтернативная реализация окна редактора)
- `build.py` — скрипт для сборки
- `add_theme.py` — скрипт для управления темами
- `add_language.py` — скрипт для управления языками
- [`src/assets/themes/themes.json`](src/assets/themes/themes.json) — файл с темами
- [`src/assets/language/language_ru.json`](src/assets/language/language_ru.json) — русские переводы
- [`src/assets/language/language_en.json`](src/assets/language/language_en.json) — английские переводы

## Лицензия

MIT License

---

**Pabla IDE** — учебный проект. Добро пожаловать к участию