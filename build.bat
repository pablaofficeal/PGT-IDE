@echo off
echo Сборка проекта Pabla IDE...

REM Установка переменной PATH для компилятора
set PATH=C:\gcc2\mingw64\bin;%PATH%

REM Удаление старой папки build если она существует
if exist build rmdir /s /q build

REM Генерация файлов сборки
echo Генерация файлов сборки...
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64"

REM Сборка проекта
echo Сборка проекта...
cmake --build build

if %ERRORLEVEL% EQU 0 (
    echo Сборка завершена успешно!
    echo Исполняемый файл: build\untitled20.exe
    
    REM Копирование файлов тем
    echo Копирование файлов тем...
    python copy_themes.py
) else (
    echo Ошибка при сборке!
    pause
) 