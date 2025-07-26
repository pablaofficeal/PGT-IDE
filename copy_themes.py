#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Скрипт для копирования файлов тем в папку сборки
"""

import os
import shutil
import sys

def copy_themes_to_build():
    """Копирует файлы тем и языков в папку build"""
    
    # Пути к файлам
    themes_source_dir = "src/assets/themes"
    language_source_dir = "src/assets/language"
    build_dir = "build"
    
    success = True
    
    # Копируем файлы тем
    if os.path.exists(themes_source_dir):
        build_themes_dir = os.path.join(build_dir, "assets", "themes")
        os.makedirs(build_themes_dir, exist_ok=True)
        
        try:
            for file_name in os.listdir(themes_source_dir):
                if file_name.endswith('.json') or file_name.endswith('.md'):
                    source_file = os.path.join(themes_source_dir, file_name)
                    dest_file = os.path.join(build_themes_dir, file_name)
                    shutil.copy2(source_file, dest_file)
                    print(f"✅ Скопирован файл темы: {file_name}")
            
            print(f"✅ Файлы тем скопированы в {build_themes_dir}")
        except Exception as e:
            print(f"❌ Ошибка копирования тем: {e}")
            success = False
    else:
        print(f"❌ Папка {themes_source_dir} не найдена!")
        success = False
    
    # Копируем файлы языков
    if os.path.exists(language_source_dir):
        build_language_dir = os.path.join(build_dir, "assets", "language")
        os.makedirs(build_language_dir, exist_ok=True)
        
        try:
            for file_name in os.listdir(language_source_dir):
                if file_name.endswith('.json') or file_name.endswith('.md'):
                    source_file = os.path.join(language_source_dir, file_name)
                    dest_file = os.path.join(build_language_dir, file_name)
                    shutil.copy2(source_file, dest_file)
                    print(f"✅ Скопирован файл языка: {file_name}")
            
            print(f"✅ Файлы языков скопированы в {build_language_dir}")
        except Exception as e:
            print(f"❌ Ошибка копирования языков: {e}")
            success = False
    else:
        print(f"❌ Папка {language_source_dir} не найдена!")
        success = False
    
    return success

def main():
    print("=== Копирование файлов тем в папку сборки ===")
    
    if copy_themes_to_build():
        print("\n🎉 Готово! Теперь приложение сможет найти файлы тем.")
        print("💡 Запустите приложение из папки build/")
    else:
        print("\n❌ Не удалось скопировать файлы тем.")
        sys.exit(1)

if __name__ == "__main__":
    main() 