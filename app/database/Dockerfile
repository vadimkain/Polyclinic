FROM postgres:11-alpine
WORKDIR .
COPY "database-init.sql" "/docker-entrypoint-initdb.d"
