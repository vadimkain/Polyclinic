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


# Проверяем, что переменная DESTINATION задана
if [ -z "$DESTINATION" ]; then
  echo "Ошибка: переменная DESTINATION не задана."
  exit 1
fi

# Проверяем, существует ли директория назначения
if [ ! -d "$DESTINATION" ]; then
  echo "Создаем директорию $DESTINATION."
  mkdir -p "$DESTINATION"
fi

# Копируем файлы и директорию
cp Dockerfile "$DESTINATION/"
cp docker-compose.yml "$DESTINATION/"
cp -r database "$DESTINATION/"

echo "copy end."
