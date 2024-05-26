#!/bin/bash

# Пути к исходной папке и файлу
FRONTEND_SRC="frontend_web"
SERVER_SRC="build/server/polyclinic_server"

# Путь к целевой папке
DESTINATION="app"

# Создание целевой папки, если она не существует
mkdir -p "$DESTINATION"

# Копирование папки frontend_web
cp -r "$FRONTEND_SRC" "$DESTINATION"

# Копирование бинарного файла polyclinic_server
cp "$SERVER_SRC" "$DESTINATION/polyclinic_server"

echo "copy end."
