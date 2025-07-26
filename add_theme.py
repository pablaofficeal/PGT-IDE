#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Скрипт для добавления новых тем в Pabla IDE
"""

import json
import os
import sys

def load_themes(file_path):
    """Загружает темы из JSON файла"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Файл {file_path} не найден!")
        return None
    except json.JSONDecodeError as e:
        print(f"Ошибка парсинга JSON: {e}")
        return None

def save_themes(themes, file_path):
    """Сохраняет темы в JSON файл"""
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(themes, f, ensure_ascii=False, indent=2)
        print(f"Темы сохранены в {file_path}")
        return True
    except Exception as e:
        print(f"Ошибка сохранения: {e}")
        return False

def add_theme():
    """Интерактивное добавление новой темы"""
    themes_file = "src/assets/themes/themes.json"
    
    # Загружаем существующие темы
    themes_data = load_themes(themes_file)
    if not themes_data:
        return False
    
    print("=== Добавление новой темы в Pabla IDE ===\n")
    
    # Получаем информацию о теме
    theme_name = input("Введите название темы: ").strip()
    if not theme_name:
        print("Название темы не может быть пустым!")
        return False
    
    if theme_name in themes_data["themes"]:
        print(f"Тема '{theme_name}' уже существует!")
        return False
    
    description = input("Введите описание темы: ").strip()
    
    print("\nВведите цвета для виджетов (оставьте пустым для пропуска):")
    
    # Базовые виджеты
    widgets = {
        "QTextEdit": "основной редактор кода",
        "QTreeView": "дерево файлов", 
        "QPlainTextEdit": "терминал",
        "QMainWindow": "главное окно",
        "QMenuBar": "панель меню",
        "QMenu": "выпадающие меню",
        "QToolBar": "панель инструментов",
        "QDockWidget": "доки (панели)"
    }
    
    styles = {}
    
    for widget, desc in widgets.items():
        print(f"\n{widget} ({desc}):")
        bg_color = input("  Цвет фона (например, #2b2b2b): ").strip()
        text_color = input("  Цвет текста (например, #ffffff): ").strip()
        
        if bg_color or text_color:
            styles[widget] = {}
            if bg_color:
                styles[widget]["background-color"] = bg_color
            if text_color:
                styles[widget]["color"] = text_color
    
    if not styles:
        print("Не указано ни одного стиля!")
        return False
    
    # Создаем новую тему
    new_theme = {
        "name": theme_name,
        "description": description,
        "styles": styles
    }
    
    # Добавляем в существующие темы
    themes_data["themes"][theme_name] = new_theme
    
    # Сохраняем
    if save_themes(themes_data, themes_file):
        print(f"\n✅ Тема '{theme_name}' успешно добавлена!")
        print("Перезапустите Pabla IDE для применения изменений.")
        return True
    
    return False

def list_themes():
    """Показывает список существующих тем"""
    themes_file = "src/assets/themes/themes.json"
    
    themes_data = load_themes(themes_file)
    if not themes_data:
        return False
    
    print("=== Существующие темы ===\n")
    
    for name, theme in themes_data["themes"].items():
        print(f"📁 {name}")
        if "description" in theme:
            print(f"   Описание: {theme['description']}")
        if "styles" in theme:
            print(f"   Виджетов: {len(theme['styles'])}")
        print()

def main():
    if len(sys.argv) > 1:
        command = sys.argv[1]
        if command == "list":
            list_themes()
        elif command == "add":
            add_theme()
        else:
            print("Использование:")
            print("  python add_theme.py add    - добавить новую тему")
            print("  python add_theme.py list   - показать существующие темы")
    else:
        print("Скрипт для управления темами Pabla IDE")
        print("\nКоманды:")
        print("  add  - добавить новую тему")
        print("  list - показать существующие темы")
        print("\nПример: python add_theme.py add")

if __name__ == "__main__":
    main() 