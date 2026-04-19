# 🔐 Password Manageris

A cross-platform password manager with multi-factor authentication, strong encryption,
and peer-to-peer synchronization between devices. Developed as a Bachelor's thesis project.

---

## ✨ Features

- 🔒 **Strong Encryption** – Passwords are encrypted using **AES-256-GCM** or **ChaCha20**
- 🔑 **Key Derivation Functions (KDF)** – Master password protection via **Argon2id**, **Scrypt**, or **PBKDF2**
- 🔐 **Multi-Factor Authentication (MFA)** – Additional layer of security on login
- 🔄 **Peer-to-Peer Sync** – Synchronize your password vault across multiple devices
- 🎲 **Password Generator** – Generate strong, customizable passwords
- 📤 **Export / Import** – Backup and restore your password database
- 🗄️ **MySQL Backend** – Persistent storage powered by MySQL (via Docker)
- 🖥️ **Cross-Platform** – Runs on **Windows 10** and **Linux**

---

## ⚠️ Security Notice

> **Recommended cipher: AES-256-GCM**
>
> ChaCha20 is available as an option but is **not recommended for use** in this application.
> It was implemented solely as part of the Bachelor's thesis assignment requirements
> and has not been fully validated for production use within this project.
> Always prefer **AES-256-GCM** for encrypting your password vault.

---

## 🛠️ Tech Stack

| Component        | Technology               |
|------------------|--------------------------|
| Language         | C++                      |
| GUI Framework    | Qt (Qt Creator)          |
| Database         | MySQL (Docker)           |
| Encryption       | AES-256-GCM, ChaCha20    |
| KDF              | Argon2id, Scrypt, PBKDF2 |
| Containerization | Docker / Docker Compose  |

---

## 📋 Prerequisites

- [Qt Creator](https://www.qt.io/product/development-tools)
- [Docker](https://www.docker.com/) & Docker Compose
- Windows 10 or Linux

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/Tomaskoss/Password_Manageris.git
cd Password_Manageris
```

### 2. Start the MySQL server via Docker

```bash
docker compose up -d
```

This will automatically set up and initialize the MySQL server.

### 3. Build and run the application

1. Open **Qt Creator**
2. Open the project (`CMakeLists.txt`)
3. Click **Build**, then **Run**

---

## 📦 Deploy Version (No Build Required)

A pre-built version is available in the `Deploy/` folder.
You only need to have **Docker** installed and configure the MySQL server.
No Qt environment or build step is required.

---

## 📁 Repository Structure

```txt
Password_Manageris/
├── Deploy/              # Pre-built application ready to use
├── libs/                # External libraries required by the application
├── pictures/            # Application screenshots and images
├── Video/               # Demo video showcasing application features
├── mainwindow.*         # Main application window (login screen)
├── managerwindow.*      # Password manager main window
├── dialog_generator.*   # Password generator dialog
├── dialog_server.*      # Server/sync configuration dialog
├── dialog_client_site.* # Client-side sync dialog
├── dialog_change.*      # Password change dialog
├── CMakeLists.txt       # CMake build configuration
└── README.md
```

---

## 🔐 Security Overview

Password Manageris takes security seriously:

- The **master password** is never stored in plain text — it is processed through a chosen KDF (Argon2id recommended)
- All stored passwords are encrypted with **AES-256-GCM** providing authenticated encryption (recommended)
- Peer-to-peer sync uses encrypted channels to prevent data leakage during transfer

---

## 📄 License

This project is distributed under the **[LGPL-3.0 License](LICENSE)**.
