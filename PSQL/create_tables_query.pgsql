---------------------- INCLUDES ----------------------

CREATE EXTENSION IF NOT EXISTS citext;

---------------------- CUSTOM DATA TYPES ----------------------

CREATE DOMAIN custom_email AS citext
    CHECK ( value ~ '^(?!.*[._-]$)[a-zA-Z0-9_.-]+@[a-zA-Z0-9]+(?:[a-zA-Z0-9-]*[a-zA-Z0-9])?(?:\.[a-zA-Z0-9]+(?:[a-zA-Z0-9-]*[a-zA-Z0-9])?)*$');

---------------------- CUSTOM FUNCTIONS ----------------------

CREATE OR REPLACE FUNCTION is_valid_patient_role(patient_id BIGINT) RETURNS BOOLEAN AS $$
BEGIN
    -- ur.role_id = 1, where 1 - is a patient role ID;
    RETURN EXISTS (SELECT 1 FROM users_roles AS ur WHERE ur.user_id = patient_id AND ur.role_id = 1);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION is_valid_doctor_role(doctor_id BIGINT) RETURNS BOOLEAN AS $$
BEGIN
    -- ur.role_id = 2, where 2 - is a doctor role ID;
    RETURN EXISTS (SELECT 1 FROM users_roles AS ur WHERE ur.user_id = doctor_id AND ur.role_id = 2);
END;
$$ LANGUAGE plpgsql;

---------------------- CREATE TABLES ----------------------

CREATE TABLE users (
    id          BIGSERIAL,
    name        VARCHAR(50)     NOT NULL,
    surname     VARCHAR(50)     NOT NULL,
    middle_name VARCHAR(50),
    email       custom_email    NOT NULL,
    PRIMARY KEY (id),
    UNIQUE (email)
);

CREATE TABLE phone_numbers (
    id              BIGSERIAL,
    number          VARCHAR(10) NOT NULL,
    country_code    VARCHAR(3)  NOT NULL,
    user_id         INT         NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (user_id) REFERENCES users(id),
    UNIQUE (number),
    CHECK (CHAR_LENGTH(number) = 10)
);

CREATE TABLE roles (
    id      SMALLSERIAL,
    name    VARCHAR(50)                 NOT NULL,
    salary  DECIMAL(8, 2) DEFAULT NULL,
    PRIMARY KEY (id),
    UNIQUE (name),
    CHECK (salary >= 0)
);

INSERT INTO roles (name, salary)
VALUES ('patient', DEFAULT),
       ('doctor', 23000);

CREATE TABLE users_roles (
    id          BIGSERIAL,
    user_id     BIGINT      NOT NULL,
    role_id     SMALLINT    NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (role_id) REFERENCES roles(id),
    UNIQUE (user_id, role_id)
);

CREATE TABLE messages (
    id  BIGSERIAL,
    user_1_id           BIGINT                                  NOT NULL,
    user_2_id           BIGINT                                  NOT NULL,
    destination_time    TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP   NOT NULL,
    data                VARCHAR(2048)                           NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (user_1_id) REFERENCES users(id),
    FOREIGN KEY (user_2_id) REFERENCES users(id)
);

-- Make optimization for ofter filtering by user_1 and user_2 IDs;
CREATE INDEX idx_user_1_id ON messages (user_1_id);
CREATE INDEX idx_user_2_id ON messages (user_2_id);

CREATE TABLE allergens (
    id      SMALLSERIAL,
    name    VARCHAR(32)     NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE users_allergen (
    id          BIGSERIAL,
    user_id     BIGINT      NOT NULL,
    allergen_id SMALLINT    NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY (allergen_id) REFERENCES allergens(id)
);

CREATE TABLE booked_doctors (
    id                  BIGSERIAL,
    patient_id          BIGINT                                  NOT NULL,
    doctor_id           BIGINT                                  NOT NULL,
    book_time           TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP   NOT NULL,
    appointment_time    TIMESTAMPTZ                             NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (patient_id) REFERENCES users(id),
    FOREIGN KEY (doctor_id) REFERENCES users(id),
    UNIQUE(patient_id, doctor_id, appointment_time),
    CONSTRAINT valid_patient_role CHECK (is_valid_patient_role(patient_id)),
    CONSTRAINT valid_doctor_role CHECK (is_valid_doctor_role(doctor_id))
);

CREATE TABLE doctor_appointments (
    id                  BIGSERIAL,
    booked_doctor_id    BIGINT      NOT NULL,
    complaint           VARCHAR(2048),
    PRIMARY KEY (id),
    FOREIGN KEY (booked_doctor_id) REFERENCES booked_doctors(id)
);

CREATE TABLE analyses (
    id      SMALLSERIAL,
    name    VARCHAR(128)    NOT NULL,
    PRIMARY KEY (id),
    UNIQUE (name)
);

CREATE TABLE analyse_appointments (
    id                      BIGSERIAL,
    analyse_id              SMALLINT    NOT NULL,
    doctor_appointment_id   BIGINT      NOT NULL,
    is_completed            BOOLEAN,
    PRIMARY KEY (id),
    FOREIGN KEY (analyse_id) REFERENCES analyses(id),
    FOREIGN KEY (doctor_appointment_id) REFERENCES doctor_appointments(id)
);

CREATE TABLE patient_sick_leave (
    id                      BIGSERIAL,
    doctor_appointment_id   BIGINT                      NOT NULL,
    start_date              DATE DEFAULT CURRENT_DATE   NOT NULL,
    end_date                DATE DEFAULT CURRENT_DATE   NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (doctor_appointment_id) REFERENCES doctor_appointments(id)
);

CREATE TABLE drugs (
    id          BIGSERIAL,
    name        varchar(64)         NOT NULL,
    description varchar(2048)   NOT NULL,
    PRIMARY KEY (id),
    UNIQUE (name)
);

CREATE TABLE prescription_drugs (
    id                      BIGSERIAL,
    drug_id                 BIGINT      NOT NULL,
    doctor_appointment_id   BIGINT      NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (drug_id) REFERENCES drugs(id),
    FOREIGN KEY (doctor_appointment_id) REFERENCES doctor_appointments(id)
);

---------------------- OTHER SCRIPTS ----------------------

-- drop all tables from db;
DO $$
DECLARE
    rec record;
BEGIN
    FOR rec IN (SELECT tablename FROM pg_tables WHERE schemaname = 'public') LOOP
        EXECUTE 'DROP TABLE IF EXISTS ' || quote_ident(rec.tablename) || ' CASCADE';
    END LOOP;
END $$;

-- DROP TABLE phone_numbers; DROP TABLE users_roles; DROP TABLE users; DROP TABLE roles; DROP DOMAIN custom_email; DROP EXTENSION citext;
