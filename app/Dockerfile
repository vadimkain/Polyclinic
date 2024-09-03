# Используем образ с установленным CMake и необходимыми инструментами
FROM ubuntu:22.04

# Обновляем список пакетов и устанавливаем libpqxx-dev
RUN apt-get update && \
    apt-get install -y libpqxx-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Копируем исходный код в контейнер
COPY . /app

# Переходим в рабочую директорию
WORKDIR /app

# Указываем команду для запуска
CMD ["./polyclinic_server"]
