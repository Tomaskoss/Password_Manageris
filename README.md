
# Password manageris
Viac platformový správca hesiel s viac faktorovou autentizáciou. Pre bezpečné uchovávanie hlavného hesla je možné využiť KDF (Argon2id,Scrypt,PBKDF2) a pre šifrovanie hesiel v zázname využíva AES-256-GCM móde, ChaCha20.

# Spustenie
## Pre rekvizity 
*Qt prostredie
* MySql Databaza (Server)
* Windows 10 / Linux 

# Windows a Linux 
Pre spustenie je možné využiť docker-compose pre spustenie docker compose súboru ktorý výtvory počiatočné nastavenie MySQL serveru 
Použijeme príkaz v terminály Docker compose up -d  pre spustenie.
Následne spustíme Qt Creator kde otvoríme projekt.
Stlačíme tlačidlo build a následne run. 

# Štruktúra repozitára
zložka Video obsahuje video aplikácie s jeho možnostami.
zložka Libs obsahuje potrebné externé knižnice pre spustenie aplikácie.
zložka Deploy obsahuje aplikáciu pripravenú na použitie bez potreby buildu, je potrebné nastaviť iba mysql server.
zložka pictures obsahuje obrázky správcu hesiel
V štruktúre vidíme .cpp, .h a .ui súbory, ktoré obsahujú kód a grafické rozhranie aplikácie

# Licencia
Aplikácia je distribuovaná pod licenciou LGPL.