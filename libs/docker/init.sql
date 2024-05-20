CREATE TABLE IF NOT EXISTS login_information (
    Login_name VARCHAR(32) PRIMARY KEY,
    Login_master_password VARCHAR(256),
    Login_salt BINARY(32),
    Login_iterations INT,
    Login_SALTLEN VARCHAR(256),
    Login_HASHLEN VARCHAR(256),
    Kdf_algorithm VARCHAR(45),
    Login_maxmemory INT,
    Login_r INT,
    Login_p INT,
    Login_email VARCHAR(128),
    Shared_secret BINARY(20),
    Shared_secret_string VARCHAR(64),
    Login_PIN VARCHAR(12)
);
