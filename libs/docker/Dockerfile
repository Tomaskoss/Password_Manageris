# Použití oficiálního obrazu MySQL
FROM mysql:latest

# Nastavení promìnných prostøedí pro MySQL
ENV MYSQL_ROOT_PASSWORD %;`jXQG|(l8I]KDWXm,V58yQ<Oi<hq3P
ENV MYSQL_DATABASE passwordmanager

# Pøidání inicializaèního SQL skriptu do adresáøe, který se automaticky spustí pøi inicializaci kontejneru
COPY init.sql /docker-entrypoint-initdb.d/

# Exponování portu 3306 pro komunikaci s databází
EXPOSE 3306