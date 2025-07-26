#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Скрипт для управления языками интерфейса Pabla IDE
"""

import json
import os
import sys

def list_languages():
    """Показать существующие языки"""
    language_dir = "src/assets/language"
    
    if not os.path.exists(language_dir):
        print("❌ Папка с языками не найдена!")
        return
    
    print("=== Доступные языки ===")
    for file_name in os.listdir(language_dir):
        if file_name.startswith("language_") and file_name.endswith(".json"):
            lang_code = file_name[9:-5]  # Убираем "language_" и ".json"
            file_path = os.path.join(language_dir, file_name)
            
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    lang_name = data.get("languages", {}).get(lang_code, lang_code)
                    print(f"✅ {lang_code}: {lang_name}")
            except Exception as e:
                print(f"❌ {lang_code}: ошибка чтения файла - {e}")

def add_language():
    """Добавить новый язык"""
    print("=== Добавление нового языка ===")
    
    # Запрашиваем код языка
    lang_code = input("Введите код языка (например, 'de' для немецкого): ").strip().lower()
    if not lang_code:
        print("❌ Код языка не может быть пустым!")
        return
    
    # Запрашиваем название языка
    lang_name = input("Введите название языка: ").strip()
    if not lang_name:
        print("❌ Название языка не может быть пустым!")
        return
    
    # Проверяем, не существует ли уже такой язык
    file_path = f"src/assets/language/language_{lang_code}.json"
    if os.path.exists(file_path):
        print(f"❌ Язык с кодом '{lang_code}' уже существует!")
        return
    
    # Создаем базовую структуру для нового языка
    base_language = {
        "interface": {
            "window_title": "Pabla IDE",
            "file_menu": "File",
            "new_file": "New File",
            "open_file": "Open File",
            "save_file": "Save File",
            "open_folder": "Open Folder",
            "settings": "Settings",
            "exit": "Exit",
            "edit_menu": "Edit",
            "undo": "Undo",
            "redo": "Redo",
            "cut": "Cut",
            "copy": "Copy",
            "paste": "Paste",
            "build_menu": "Build",
            "build": "Build",
            "run": "Run",
            "help_menu": "Help",
            "about": "About",
            "terminal_placeholder": "Enter command...",
            "file_tree_title": "Project Files"
        },
        "dialogs": {
            "settings_title": "Pabla IDE Settings",
            "theme_label": "Select interface theme:",
            "language_label": "Select interface language:",
            "ok_button": "OK",
            "cancel_button": "Cancel",
            "error_title": "Error",
            "warning_title": "Warning",
            "info_title": "Information"
        },
        "messages": {
            "file_open_error": "Failed to open file.",
            "folder_required": "Please open a project folder first.",
            "project_building": "Building project...",
            "project_running": "Running project...",
            "project_built": "Project built.",
            "project_started": "Project started.",
            "theme_applied": "Theme applied: ",
            "theme_activated": "Theme '{0}' activated.",
            "theme_not_found": "Theme '{0}' not found. Use 'themes' command for available themes list.",
            "settings_saved": "Settings saved.",
            "unknown_command": "Unknown command. Type 'help' for command list.",
            "no_themes": "No themes available.",
            "available_themes": "Available themes:",
            "language_changed": "Language changed to: "
        },
        "commands": {
            "help_title": "Available commands:",
            "help_help": "help - show command list",
            "help_themes": "themes - show available themes",
            "help_start_theme": "start theme <name> - apply theme",
            "help_language": "language <code> - change language (ru/en)",
            "help_build": "build - build project",
            "help_run": "run - run project",
            "help_clear": "clear - clear terminal",
            "available_themes_list": "Available themes:",
            "available_languages": "Available languages:",
            "languages": "languages - show available languages"
        },
        "languages": {
            "ru": "Русский",
            "en": "English",
            lang_code: lang_name
        }
    }
    
    # Создаем папку если её нет
    os.makedirs("src/assets/language", exist_ok=True)
    
    # Сохраняем файл
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(base_language, f, ensure_ascii=False, indent=2)
        
        print(f"✅ Язык '{lang_name}' ({lang_code}) успешно создан!")
        print(f"📁 Файл: {file_path}")
        print("💡 Отредактируйте файл, чтобы добавить переводы для вашего языка.")
        
    except Exception as e:
        print(f"❌ Ошибка создания файла: {e}")

def main():
    if len(sys.argv) < 2:
        print("Использование:")
        print("  python add_language.py list  # показать существующие языки")
        print("  python add_language.py add   # добавить новый язык")
        return
    
    command = sys.argv[1].lower()
    
    if command == "list":
        list_languages()
    elif command == "add":
        add_language()
    else:
        print(f"❌ Неизвестная команда: {command}")
        print("Доступные команды: list, add")

if __name__ == "__main__":
    main() 