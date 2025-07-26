# Инструкции по сборке Pabla IDE

## Требования

- CMake 3.30 или выше
- Qt 6.9.1 (установлен в C:/Qt/6.9.1/mingw_64)
- MinGW-w64 компилятор (установлен в C:/gcc2/mingw64)
- Ninja (установлен через pip: `pip install ninja`)

## Способы сборки

### 1. Использование batch файла (рекомендуется для Windows)

```bash
build.bat
```

### 2. Использование Python скрипта

```bash
python build.py
```

### 3. Ручная сборка

```bash
# Установка переменной PATH для компилятора
$env:PATH = "C:/gcc2/mingw64/bin;$env:PATH"

# Генерация файлов сборки
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64"

# Сборка проекта
cmake --build build
```

## Результат сборки

После успешной сборки в папке `build/` будут созданы:
- `untitled20.exe` - исполняемый файл приложения
- `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll` - необходимые библиотеки Qt
- `plugins/platforms/qwindows.dll` - плагин для Windows

## Запуск приложения

```bash
cd build
./untitled20.exe
```

## Устранение проблем

### Ошибка "could not load cache"
Удалите папку `build/` и повторите сборку.

### Ошибка компилятора
Убедитесь, что MinGW-w64 установлен в `C:/gcc2/mingw64/` и добавлен в PATH.

### Ошибка Qt
Убедитесь, что Qt 6.9.1 установлен в `C:/Qt/6.9.1/mingw_64/`. 