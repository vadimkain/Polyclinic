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

CREATE TABLE passwords (
    user_id BIGINT PRIMARY KEY,
    pass TEXT NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
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

-- Заполнение таблицы users
INSERT INTO users (name, surname, middle_name, email)
VALUES
('Іван', 'Іванов', 'Іванович', 'ivan.ivanov1@example.com'),
('Петро', 'Петров', 'Петрович', 'petro.petrov2@example.com'),
('Сергій', 'Сергеєв', 'Сергійович', 'sergiy.sergeev3@example.com'),
('Олексій', 'Олексієв', 'Олексійович', 'oleksiy.olekseev4@example.com'),
('Дмитро', 'Дмитрієв', 'Дмитрович', 'dmytro.dmitriev5@example.com'),
('Михайло', 'Михайлов', 'Михайлович', 'mykhaylo.mykhaylov6@example.com'),
('Андрій', 'Андреєв', 'Андрійович', 'andriy.andreev7@example.com'),
('Володимир', 'Володимиров', 'Володимирович', 'volodymyr.volodymyrov8@example.com'),
('Антон', 'Антонов', 'Антонович', 'anton.antonov9@example.com'),
('Микола', 'Миколаєв', 'Миколайович', 'mykola.mykolaev10@example.com'),
('Ольга', 'Ольгівна', 'Ольгівна', 'olga.olgina11@example.com'),
('Марія', 'Мариніна', 'Маріївна', 'maria.marina12@example.com'),
('Олена', 'Оленіна', 'Оленівна', 'olena.olenina13@example.com'),
('Тетяна', 'Тетяніна', 'Тетянівна', 'tetyana.tetyanina14@example.com'),
('Світлана', 'Світліна', 'Світланівна', 'svitlana.svitlina15@example.com'),
('Анастасія', 'Анастасіна', 'Анастасівна', 'anastasia.anastasina16@example.com'),
('Катерина', 'Катеринина', 'Катеринівна', 'kateryna.katerynina17@example.com'),
('Наталія', 'Наталієва', 'Наталіївна', 'nataliya.natalieva18@example.com'),
('Юлія', 'Юлієва', 'Юліївна', 'yulia.yulieva19@example.com'),
('Дарина', 'Даріївна', 'Даріївна', 'daryna.daryevna20@example.com'),
('Віктор', 'Вікторов', 'Вікторович', 'viktor.viktorov21@example.com'),
('Роман', 'Романов', 'Романович', 'roman.romanov22@example.com'),
('Ігор', 'Ігоров', 'Ігорович', 'igor.igorev23@example.com'),
('Кирило', 'Кирилов', 'Кирилович', 'kyrylo.kyrylov24@example.com'),
('Максим', 'Максимов', 'Максимович', 'maksym.maksymov25@example.com'),
('Лев', 'Львів', 'Львович', 'lev.lvov26@example.com'),
('Георгій', 'Георгієв', 'Георгійович', 'georgiy.georgiev27@example.com'),
('Павло', 'Павлов', 'Павлович', 'pavlo.pavlov28@example.com'),
('Федір', 'Федоров', 'Федорович', 'fedir.fedorov29@example.com'),
('Артем', 'Артемов', 'Артемович', 'artem.artemov30@example.com'),
('Олександр', 'Олександров', 'Олександрович', 'oleksandr.oleksandrov31@example.com'),
('Валерій', 'Валерійов', 'Валерійович', 'valeriy.valeriyov32@example.com'),
('Борис', 'Борисов', 'Борисович', 'boris.borisov33@example.com'),
('Григорій', 'Григорієв', 'Григорійович', 'grygoriy.grygoryev34@example.com'),
('Денис', 'Денисов', 'Денисович', 'denys.denysov35@example.com'),
('Євген', 'Євгенов', 'Євгенович', 'yevgen.yevgenov36@example.com'),
('Жанна', 'Жаннов', 'Жанівна', 'zhanna.zhannov37@example.com'),
('Зоя', 'Зоєва', 'Зоєвна', 'zoya.zoyeva38@example.com'),
('Інна', 'Іннова', 'Іннівна', 'inna.innova39@example.com'),
('Каріна', 'Карініна', 'Каринівна', 'karina.karinina40@example.com'),
('Ірина', 'Іріїна', 'Іріївна', 'iryna.irina41@example.com'),
('Оксана', 'Оксаніна', 'Оксанівна', 'oksana.oksanina42@example.com'),
('Василь', 'Васильєв', 'Васильович', 'vasyl.vasylov43@example.com'),
('Тимофій', 'Тимофіїв', 'Тимофійович', 'tymoshiy.tymoshiyov44@example.com'),
('Світлана', 'Светланівна', 'Светланівна', 'svitlana.svetlanivna45@example.com'),
('Зіновій', 'Зіновіїв', 'Зіновійович', 'zinoviy.zinoviyov46@example.com'),
('Ілля', 'Ільїнов', 'Ільїнович', 'ilya.ilyinov47@example.com'),
('Лариса', 'Ларисіна', 'Ларисівна', 'larisa.larisina48@example.com'),
('Роксолана', 'Роксоланівна', 'Роксоланівна', 'roxolana.roxolanivna49@example.com'),
('Ярослав', 'Ярославів', 'Ярославович', 'yaroslav.yaroslavov50@example.com');

INSERT INTO passwords (user_id, pass) VALUES 
(1, '123456'),
(2, 'password'),
(3, '123456789'),
(4, '12345678'),
(5, '12345'),
(6, '1234567'),
(7, 'qwerty'),
(8, 'abc123'),
(9, '111111'),
(10, '123123'),
(11, 'admin'),
(12, 'letmein'),
(13, 'welcome'),
(14, 'monkey'),
(15, 'login'),
(16, '1234'),
(17, 'passw0rd'),
(18, 'starwars'),
(19, 'iloveyou'),
(20, '123321'),
(21, '000000'),
(22, 'superman'),
(23, 'asdfgh'),
(24, 'zxcvbn'),
(25, 'password1'),
(26, '1q2w3e4r'),
(27, 'qwertyuiop'),
(28, 'qwerty123'),
(29, 'admin123'),
(30, '654321'),
(31, 'qazwsx'),
(32, '1qaz2wsx'),
(33, 'michael'),
(34, 'football'),
(35, 'sunshine'),
(36, 'princess'),
(37, 'trustno1'),
(38, 'whatever'),
(39, 'master'),
(40, 'qwerty1'),
(41, 'password!'),
(42, '1q2w3e'),
(43, 'mypassword'),
(44, 'letmein123'),
(45, '123qwe'),
(46, 'abc123456'),
(47, 'password2'),
(48, 'asdf1234'),
(49, 'pass1234'),
(50, 'qwert12345');


-- Заполнение таблицы users_roles
-- Врачи
INSERT INTO users_roles (user_id, role_id)
VALUES
(1, 2),
(2, 2),
(3, 2),
(4, 2),
(5, 2),
(6, 2),
(7, 2),
(8, 2),
(9, 2),
(10, 2);

-- Пациенты
INSERT INTO users_roles (user_id, role_id)
VALUES
(11, 1),
(12, 1),
(13, 1),
(14, 1),
(15, 1),
(16, 1),
(17, 1),
(18, 1),
(19, 1),
(20, 1),
(21, 1),
(22, 1),
(23, 1),
(24, 1),
(25, 1),
(26, 1),
(27, 1),
(28, 1),
(29, 1),
(30, 1),
(31, 1),
(32, 1),
(33, 1),
(34, 1),
(35, 1),
(36, 1),
(37, 1),
(38, 1),
(39, 1),
(40, 1),
(41, 1),
(42, 1),
(43, 1),
(44, 1),
(45, 1),
(46, 1),
(47, 1),
(48, 1),
(49, 1),
(50, 1);

-- Заполнение таблицы phone_numbers
INSERT INTO phone_numbers (number, country_code, user_id)
VALUES
('0441234567', '+38', 1),
('0632345678', '+38', 2),
('0933456789', '+38', 3),
('0964567890', '+38', 4),
('0995678901', '+38', 5),
('0666789012', '+38', 6),
('0677890123', '+38', 7),
('0988901234', '+38', 8),
('0449012345', '+38', 9),
('0630123456', '+38', 10),
('0931234567', '+38', 11),
('0962345678', '+38', 12),
('0993456789', '+38', 13),
('0664567890', '+38', 14),
('0675678901', '+38', 15),
('0986789012', '+38', 16),
('0447890123', '+38', 17),
('0638901234', '+38', 18),
('0939012345', '+38', 19),
('0960123456', '+38', 20),
('0991234567', '+38', 21),
('0662345678', '+38', 22),
('0673456789', '+38', 23),
('0984567890', '+38', 24),
('0445678901', '+38', 25),
('0636789012', '+38', 26),
('0937890123', '+38', 27),
('0968901234', '+38', 28),
('0999012345', '+38', 29),
('0660123456', '+38', 30),
('0671234567', '+38', 31),
('0982345678', '+38', 32),
('0443456789', '+38', 33),
('0634567890', '+38', 34),
('0935678901', '+38', 35),
('0966789012', '+38', 36),
('0997890123', '+38', 37),
('0668901234', '+38', 38),
('0679012345', '+38', 39),
('0980123456', '+38', 40),
('0445678902', '+38', 41),
('0632345677', '+38', 42),
('0933456788', '+38', 43),
('0964567889', '+38', 44),
('0995678890', '+38', 45),
('0666789001', '+38', 46),
('0677890112', '+38', 47),
('0988901223', '+38', 48),
('0449012334', '+38', 49),
('0630123445', '+38', 50);

-- Заполнение таблицы booked_doctors с разным количеством посещений
INSERT INTO booked_doctors (patient_id, doctor_id, appointment_time)
VALUES
(11, 1, '2024-05-25 10:00:00'),
(12, 1, '2024-05-25 11:00:00'),
(13, 2, '2024-05-25 12:00:00'),
(14, 2, '2024-05-25 13:00:00'),
(15, 3, '2024-05-25 14:00:00'),
(16, 3, '2024-05-25 15:00:00'),
(17, 4, '2024-05-25 16:00:00'),
(18, 4, '2024-05-25 17:00:00'),
(19, 5, '2024-05-25 18:00:00'),
(20, 5, '2024-05-26 10:00:00'),
(21, 6, '2024-05-26 11:00:00'),
(22, 6, '2024-05-26 12:00:00'),
(23, 7, '2024-05-26 13:00:00'),
(24, 7, '2024-05-26 14:00:00'),
(25, 8, '2024-05-26 15:00:00'),
(26, 8, '2024-05-26 16:00:00'),
(27, 9, '2024-05-27 10:00:00'),
(28, 9, '2024-05-27 11:00:00'),
(29, 10, '2024-05-27 12:00:00'),
(30, 10, '2024-05-27 13:00:00'),
(31, 1, '2024-05-28 10:00:00'),
(32, 2, '2024-05-28 11:00:00'),
(33, 3, '2024-05-28 12:00:00'),
(34, 4, '2024-05-28 13:00:00'),
(35, 5, '2024-05-29 10:00:00'),
(36, 6, '2024-05-29 11:00:00'),
(37, 7, '2024-05-29 12:00:00'),
(38, 8, '2024-05-29 13:00:00'),
(39, 9, '2024-05-30 10:00:00'),
(40, 10, '2024-05-30 11:00:00'),
(41, 1, '2024-05-31 10:00:00'),
(42, 2, '2024-05-31 11:00:00'),
(43, 3, '2024-05-31 12:00:00'),
(44, 4, '2024-05-31 13:00:00'),
(45, 5, '2024-06-01 10:00:00'),
(46, 6, '2024-06-01 11:00:00'),
(47, 7, '2024-06-01 12:00:00'),
(48, 8, '2024-06-01 13:00:00'),
(49, 9, '2024-06-02 10:00:00'),
(50, 10, '2024-06-02 11:00:00'),
(11, 2, '2024-06-03 10:00:00'),
(12, 3, '2024-06-04 11:00:00'),
(13, 4, '2024-06-05 12:00:00'),
(14, 5, '2024-06-06 13:00:00'),
(15, 6, '2024-06-07 14:00:00'),
(16, 7, '2024-06-08 15:00:00'),
(17, 8, '2024-06-09 16:00:00'),
(18, 9, '2024-06-10 17:00:00'),
(19, 10, '2024-06-11 18:00:00'),
(20, 1, '2024-06-12 10:00:00');

-- Заполнение таблицы doctor_appointments для соответствия booked_doctors
INSERT INTO doctor_appointments (booked_doctor_id, complaint)
VALUES
(1, 'Серцева недостатність'),
(2, 'Серцева аритмія'),
(3, 'Серцева ішемія'),
(4, 'Серцева гіпертензія'),
(5, 'Серцева ангіопатія'),
(6, 'Серцева гіпертрофія'),
(7, 'Стенокардія'),
(8, 'Атеросклероз'),
(9, 'Міокардит'),
(10, 'Кардіоміопатія'),
(11, 'Серцева недостатність'),
(12, 'Серцева аритмія'),
(13, 'Серцева ішемія'),
(14, 'Серцева гіпертензія'),
(15, 'Серцева ангіопатія'),
(16, 'Серцева гіпертрофія'),
(17, 'Стенокардія'),
(18, 'Атеросклероз'),
(19, 'Міокардит'),
(20, 'Кардіоміопатія'),
(21, 'Серцева недостатність'),
(22, 'Серцева аритмія'),
(23, 'Серцева ішемія'),
(24, 'Серцева гіпертензія'),
(25, 'Серцева ангіопатія'),
(26, 'Серцева гіпертрофія'),
(27, 'Стенокардія'),
(28, 'Атеросклероз'),
(29, 'Міокардит'),
(30, 'Кардіоміопатія'),
(31, 'Серцева недостатність'),
(32, 'Серцева аритмія'),
(33, 'Серцева ішемія'),
(34, 'Серцева гіпертензія'),
(35, 'Серцева ангіопатія'),
(36, 'Серцева гіпертрофія'),
(37, 'Стенокардія'),
(38, 'Атеросклероз'),
(39, 'Міокардит'),
(40, 'Кардіоміопатія'),
(41, 'Серцева недостатність'),
(42, 'Серцева аритмія'),
(43, 'Серцева ішемія'),
(44, 'Серцева гіпертензія'),
(45, 'Серцева ангіопатія'),
(46, 'Серцева гіпертрофія'),
(47, 'Стенокардія'),
(48, 'Атеросклероз'),
(49, 'Міокардит'),
(50, 'Кардіоміопатія');

-- Заполнение таблицы allergens
INSERT INTO allergens (name)
VALUES
('Пилок'),
('Пил'),
('Горіхи'),
('Молоко'),
('Яйця'),
('Морепродукти'),
('Пшениця'),
('Соя'),
('Риба'),
('Тваринна шерсть');

-- Заполнение таблицы users_allergen
INSERT INTO users_allergen (user_id, allergen_id)
VALUES
(11, 1),
(12, 2),
(13, 3),
(14, 4),
(15, 5),
(16, 6),
(17, 7),
(18, 8),
(19, 9),
(20, 10),
(21, 1),
(22, 2),
(23, 3),
(24, 4),
(25, 5),
(26, 6),
(27, 7),
(28, 8),
(29, 9),
(30, 10),
(31, 1),
(32, 2),
(33, 3),
(34, 4),
(35, 5),
(36, 6),
(37, 7),
(38, 8),
(39, 9),
(40, 10),
(41, 1),
(42, 2),
(43, 3),
(44, 4),
(45, 5),
(46, 6),
(47, 7),
(48, 8),
(49, 9),
(50, 10);

-- Заполнение таблицы analyses
INSERT INTO analyses (name)
VALUES
('Аналіз крові'),
('Аналіз сечі'),
('Рентген'),
('МРТ'),
('КТ сканування'),
('УЗД'),
('ЕКГ'),
('ЕЕГ'),
('Алергопроба'),
('Тест на гормони щитоподібної залози');

-- Заполнение таблицы analyse_appointments
INSERT INTO analyse_appointments (analyse_id, doctor_appointment_id, is_completed)
VALUES
(1, 1, TRUE),
(2, 2, TRUE),
(3, 3, TRUE),
(4, 4, TRUE),
(5, 5, TRUE),
(6, 6, TRUE),
(7, 7, TRUE),
(8, 8, TRUE),
(9, 9, TRUE),
(10, 10, TRUE),
(1, 11, TRUE),
(2, 12, TRUE),
(3, 13, TRUE),
(4, 14, TRUE),
(5, 15, TRUE),
(6, 16, TRUE),
(7, 17, TRUE),
(8, 18, TRUE),
(9, 19, TRUE),
(10, 20, TRUE),
(1, 21, TRUE),
(2, 22, TRUE),
(3, 23, TRUE),
(4, 24, TRUE),
(5, 25, TRUE),
(6, 26, TRUE),
(7, 27, TRUE),
(8, 28, TRUE),
(9, 29, TRUE),
(10, 30, TRUE),
(1, 31, TRUE),
(2, 32, TRUE),
(3, 33, TRUE),
(4, 34, TRUE),
(5, 35, TRUE),
(6, 36, TRUE),
(7, 37, TRUE),
(8, 38, TRUE),
(9, 39, TRUE),
(10, 40, TRUE),
(1, 41, TRUE),
(2, 42, TRUE),
(3, 43, TRUE),
(4, 44, TRUE),
(5, 45, TRUE),
(6, 46, TRUE),
(7, 47, TRUE),
(8, 48, TRUE),
(9, 49, TRUE),
(10, 50, TRUE);

-- Заполнение таблицы drugs
INSERT INTO drugs (name, description)
VALUES
('Парацетамол', 'Знеболювальний і жарознижувальний засіб.'),
('Ібупрофен', 'Протизапальний і знеболювальний засіб.'),
('Амоксицилін', 'Антибіотик для лікування бактеріальних інфекцій.'),
('Метформін', 'Використовується для лікування діабету 2 типу.'),
('Аспірин', 'Знеболювальний і розріджуючий кров.'),
('Омепразол', 'Зменшує кислотність шлунка.'),
('Аторвастатин', 'Знижує рівень холестерину.'),
('Метопролол', 'Лікує високий тиск.'),
('Лізиноприл', 'Лікує високий тиск. Лікує серцеву недостатність.'),
('Альбутерол', 'Лікує астму.');

INSERT INTO drugs (name, description)
VALUES
('Бісопролол', 'Лікує серцеву недостатність.'),
('Карведилол', 'Лікує серцеву недостатність.'),
('Еналаприл', 'Лікує серцеву недостатність.'),
('Валсартан', 'Лікує серцеву недостатність.'),
('Амлодипін', 'Лікує серцеву недостатність.'),
('Небіволол', 'Лікує серцеву недостатність.'),
('Дигоксин', 'Лікує серцеву недостатність.');

-- Заполнение таблицы prescription_drugs
INSERT INTO prescription_drugs (drug_id, doctor_appointment_id)
VALUES
(1, 1),
(2, 2),
(3, 3),
(4, 4),
(5, 5),
(6, 6),
(7, 7),
(8, 8),
(9, 9),
(10, 10),
(1, 11),
(2, 12),
(3, 13),
(4, 14),
(5, 15),
(6, 16),
(7, 17),
(8, 18),
(9, 19),
(10, 20),
(1, 21),
(2, 22),
(3, 23),
(4, 24),
(5, 25),
(6, 26),
(7, 27),
(8, 28),
(9, 29),
(10, 30),
(1, 31),
(2, 32),
(3, 33),
(4, 34),
(5, 35),
(6, 36),
(7, 37),
(8, 38),
(9, 39),
(10, 40),
(1, 41),
(2, 42),
(3, 43),
(4, 44),
(5, 45),
(6, 46),
(7, 47),
(8, 48),
(9, 49),
(10, 50);

-- Добавление записей в таблицу prescription_drugs для новых препаратов
INSERT INTO prescription_drugs (drug_id, doctor_appointment_id)
VALUES
(11, 1),
(12, 2),
(13, 3),
(14, 4),
(15, 5),
(16, 6),
(17, 7),
(11, 8),
(12, 9),
(13, 10),
(14, 11),
(15, 12),
(16, 13),
(17, 14),
(11, 15),
(12, 16),
(13, 17),
(14, 18),
(15, 19),
(16, 20);

-- Заполнение таблицы patient_sick_leave
INSERT INTO patient_sick_leave (doctor_appointment_id, start_date, end_date)
VALUES
(1, '2024-05-25', '2024-06-01'),
(2, '2024-05-26', '2024-06-02'),
(3, '2024-05-27', '2024-06-03'),
(4, '2024-05-28', '2024-06-04'),
(5, '2024-05-29', '2024-06-05'),
(6, '2024-05-30', '2024-06-06'),
(7, '2024-05-31', '2024-06-07'),
(8, '2024-06-01', '2024-06-08'),
(9, '2024-06-02', '2024-06-09'),
(10, '2024-06-03', '2024-06-10'),
(11, '2024-06-04', '2024-06-11'),
(12, '2024-06-05', '2024-06-12'),
(13, '2024-06-06', '2024-06-13'),
(14, '2024-06-07', '2024-06-14'),
(15, '2024-06-08', '2024-06-15'),
(16, '2024-06-09', '2024-06-16'),
(17, '2024-06-10', '2024-06-17'),
(18, '2024-06-11', '2024-06-18'),
(19, '2024-06-12', '2024-06-19'),
(20, '2024-06-13', '2024-06-20'),
(21, '2024-06-14', '2024-06-21'),
(22, '2024-06-15', '2024-06-22'),
(23, '2024-06-16', '2024-06-23'),
(24, '2024-06-17', '2024-06-24'),
(25, '2024-06-18', '2024-06-25'),
(26, '2024-06-19', '2024-06-26'),
(27, '2024-06-20', '2024-06-27'),
(28, '2024-06-21', '2024-06-28'),
(29, '2024-06-22', '2024-06-29'),
(30, '2024-06-23', '2024-06-30'),
(31, '2024-06-24', '2024-07-01'),
(32, '2024-06-25', '2024-07-02'),
(33, '2024-06-26', '2024-07-03'),
(34, '2024-06-27', '2024-07-04'),
(35, '2024-06-28', '2024-07-05'),
(36, '2024-06-29', '2024-07-06'),
(37, '2024-06-30', '2024-07-07'),
(38, '2024-07-01', '2024-07-08'),
(39, '2024-07-02', '2024-07-09'),
(40, '2024-07-03', '2024-07-10');


SELECT * FROM users;
SELECT * FROM passwords;
SELECT * FROM phone_numbers;
SELECT * FROM roles;
SELECT * FROM users_roles;
SELECT * FROM allergens;
SELECT * FROM users_allergen;
SELECT * FROM booked_doctors;
SELECT * FROM doctor_appointments;
SELECT * FROM analyses;
SELECT * FROM analyse_appointments;
SELECT * FROM patient_sick_leave;
SELECT * FROM drugs;
SELECT * FROM prescription_drugs;
SELECT * FROM messages;

/* Description:
 * Sampled Projection: Get a list of patients seen by a specified doctor on a specified date.
 */
SELECT u.id, u.name, u.surname, u.middle_name, u.email
FROM users u
JOIN booked_doctors bd ON u.id = bd.patient_id
JOIN users_roles ur ON bd.doctor_id = ur.user_id
JOIN roles r ON ur.role_id = r.id
WHERE r.name = 'doctor'
    AND bd.doctor_id = 1  -- 1 - requeired doc ID
    AND DATE(bd.appointment_time) = '2024-05-25';


/* Description:
 * Get a list of doctors ranked by the number of patients admitted in the last month.
 */
WITH date_range AS (
    SELECT 
        date_trunc('month', CURRENT_DATE) - interval '1 month' AS last_month_start,
        date_trunc('month', CURRENT_DATE) AS last_month_end
)
SELECT 
    u.id, 
    u.name, 
    u.surname, 
    u.middle_name, 
    u.email, 
    COUNT(bd.patient_id) AS patients_count
FROM users u
JOIN booked_doctors bd ON u.id = bd.doctor_id
JOIN date_range dr ON bd.appointment_time >= dr.last_month_start
                  AND bd.appointment_time < dr.last_month_end
GROUP BY u.id
ORDER BY patients_count DESC;


/* Description:
 * Get a list of patients who have been prescribed medications from the heart disease category.
 */
SELECT DISTINCT u.id, u.name, u.surname, u.middle_name, u.email
FROM users u
JOIN booked_doctors bd ON u.id = bd.patient_id
JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
JOIN prescription_drugs pd ON da.id = pd.doctor_appointment_id
JOIN drugs d ON pd.drug_id = d.id
WHERE LOWER(d.description) LIKE '%серц%';


/* Description:
 * Count the number of patients each physician has seen per appointment during the last month.
 */
WITH date_range AS (
    SELECT 
        date_trunc('month', CURRENT_DATE) - interval '1 month' AS last_month_start,
        date_trunc('month', CURRENT_DATE) AS last_month_end
)
SELECT 
    u.id, 
    u.name, 
    u.surname, 
    u.middle_name, 
    u.email, 
    COUNT(bd.patient_id) AS patients_count
FROM users u
JOIN booked_doctors bd ON u.id = bd.doctor_id
JOIN date_range dr ON bd.appointment_time >= dr.last_month_start
                  AND bd.appointment_time < dr.last_month_end
GROUP BY u.id;


/* Description:
 * Merge: Give a total list of medications and tests that have been prescribed for the specified 
 * patient in the last six months, without repeats, with status.
 */
SELECT analyses.name AS name, 'Анализы' AS type
FROM users
         INNER JOIN booked_doctors ON booked_doctors.patient_id = users.id
         INNER JOIN doctor_appointments ON doctor_appointments.booked_doctor_id = booked_doctors.id
         INNER JOIN analyse_appointments ON analyse_appointments.doctor_appointment_id = doctor_appointments.id
         INNER JOIN analyses ON analyses.id = analyse_appointments.analyse_id
WHERE users.id = 5
  AND booked_doctors.appointment_time >= CURRENT_DATE - INTERVAL '6 months'
UNION
SELECT drugs.name AS name, 'Лекарства' AS type
FROM users
         INNER JOIN booked_doctors ON booked_doctors.patient_id = users.id
         INNER JOIN doctor_appointments ON doctor_appointments.booked_doctor_id = booked_doctors.id
         INNER JOIN prescription_drugs ON prescription_drugs.doctor_appointment_id = doctor_appointments.id
         INNER JOIN drugs ON drugs.id = prescription_drugs.drug_id
WHERE users.id = 13
  AND booked_doctors.appointment_time >= CURRENT_DATE - INTERVAL '6 months';



/* Description:
 * Get the ranking of each doctor based on: 
 *  - the number of patients seen;
 *  - the number of analyses prescribed; 
 *  - the number of drugs prescribed in the last quarter.
 */
WITH patient_count AS (
    SELECT 
        bd.doctor_id,
        COUNT(DISTINCT bd.patient_id) AS patient_count
    FROM 
        booked_doctors bd
    WHERE 
        bd.appointment_time >= date_trunc('quarter', CURRENT_DATE) - interval '3 month'
    GROUP BY 
        bd.doctor_id
),
analyse_count AS (
    SELECT 
        bd.doctor_id,
        COUNT(aa.id) AS analyse_count
    FROM 
        analyse_appointments aa
    JOIN 
        doctor_appointments da ON aa.doctor_appointment_id = da.id
    JOIN 
        booked_doctors bd ON da.booked_doctor_id = bd.id
    WHERE 
        aa.is_completed = TRUE
        AND bd.appointment_time >= date_trunc('quarter', CURRENT_DATE) - interval '3 month'
    GROUP BY 
        bd.doctor_id
),
drug_count AS (
    SELECT 
        bd.doctor_id,
        COUNT(pd.id) AS drug_count
    FROM 
        prescription_drugs pd
    JOIN 
        doctor_appointments da ON pd.doctor_appointment_id = da.id
    JOIN 
        booked_doctors bd ON da.booked_doctor_id = bd.id
    WHERE 
        bd.appointment_time >= date_trunc('quarter', CURRENT_DATE) - interval '3 month'
    GROUP BY 
        bd.doctor_id
),
combined AS (
    SELECT 
        u.id AS doctor_id,
        u.name AS doctor_name,
        u.surname AS doctor_surname,
        u.middle_name AS doctor_middle_name,
        COALESCE(pc.patient_count, 0) AS patient_count,
        COALESCE(ac.analyse_count, 0) AS analyse_count,
        COALESCE(dc.drug_count, 0) AS drug_count
    FROM 
        users u
    LEFT JOIN 
        patient_count pc ON u.id = pc.doctor_id
    LEFT JOIN 
        analyse_count ac ON u.id = ac.doctor_id
    LEFT JOIN 
        drug_count dc ON u.id = dc.doctor_id
    WHERE 
        EXISTS (SELECT 1 FROM users_roles ur WHERE ur.user_id = u.id AND ur.role_id = 2)
)
SELECT 
    doctor_id,
    doctor_name,
    doctor_surname,
    doctor_middle_name,
    patient_count,
    RANK() OVER (ORDER BY patient_count DESC) AS patient_rank,
    analyse_count,
    RANK() OVER (ORDER BY analyse_count DESC) AS analyse_rank,
    drug_count,
    RANK() OVER (ORDER BY drug_count DESC) AS drug_rank
FROM 
    combined
ORDER BY 
    doctor_id;



/* Description:
 * Sampled Projection: Get a list of patients seen by a specified doctor on a specified date.
 */
CREATE OR REPLACE FUNCTION get_patients_by_doctor_and_date(doctor_id INT, appointment_date DATE) 
RETURNS TABLE (
    id BIGINT,
    name VARCHAR(50),
    surname VARCHAR(50),
    middle_name VARCHAR(50),
    email custom_email
) AS $$
BEGIN
    RETURN QUERY
    SELECT u.id, u.name, u.surname, u.middle_name, u.email
    FROM users u
    JOIN booked_doctors bd ON u.id = bd.patient_id
    JOIN users_roles ur ON bd.doctor_id = ur.user_id
    JOIN roles r ON ur.role_id = r.id
    WHERE r.name = 'doctor'
      AND bd.doctor_id = get_patients_by_doctor_and_date.doctor_id
      AND DATE(bd.appointment_time) = get_patients_by_doctor_and_date.appointment_date;
END;
$$ LANGUAGE plpgsql;

/* Description:
 * Get a list of doctors ranked by the number of patients admitted in the last month.
 */
CREATE OR REPLACE FUNCTION get_top_doctors_last_month() 
RETURNS TABLE (
    id BIGINT,
    name VARCHAR(50),
    surname VARCHAR(50),
    middle_name VARCHAR(50),
    email custom_email,
    clients_count BIGINT
) AS $$
BEGIN
    RETURN QUERY
    SELECT *
    FROM get_patient_count_per_doctor_last_month() AS bd
    ORDER BY bd.clients_count DESC;
END;
$$ LANGUAGE plpgsql;




/* Description:
 * Get a list of patients who have been prescribed medications from the heart disease category.
 */
CREATE OR REPLACE FUNCTION get_patients_with_heart_disease_drugs() 
RETURNS TABLE (
    id BIGINT,
    name VARCHAR(50),
    surname VARCHAR(50),
    middle_name VARCHAR(50),
    email custom_email
) AS $$
BEGIN
    RETURN QUERY
    SELECT DISTINCT u.id, u.name, u.surname, u.middle_name, u.email
    FROM users u
    JOIN booked_doctors bd ON u.id = bd.patient_id
    JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
    JOIN prescription_drugs pd ON da.id = pd.doctor_appointment_id
    JOIN drugs d ON pd.drug_id = d.id
    WHERE LOWER(d.description) LIKE '%серд%';
END;
$$ LANGUAGE plpgsql;


/* Description:
 * Count the number of patients each physician has seen per appointment during the last month.
 */
CREATE OR REPLACE FUNCTION get_patient_count_per_doctor_last_month()
RETURNS TABLE (
    id BIGINT,
    name VARCHAR(50),
    surname VARCHAR(50),
    middle_name VARCHAR(50),
    email custom_email,
    clients_count BIGINT
) AS $$
DECLARE
    last_month_start DATE := date_trunc('month', CURRENT_DATE) - interval '1 month';
    last_month_end DATE := date_trunc('month', CURRENT_DATE);
BEGIN
    RETURN QUERY
    SELECT u.id, u.name, u.surname, u.middle_name, u.email, COUNT(bd.patient_id) AS patients_count
    FROM users u
    JOIN booked_doctors bd ON u.id = bd.doctor_id
    WHERE bd.appointment_time >= last_month_start
    AND bd.appointment_time < last_month_end
    GROUP BY u.id;
END;
$$ LANGUAGE plpgsql;

/* Description:
 * Merge: Give a total list of medications and tests that have been prescribed for the specified 
 * patient in the last six months, without repeats, with status.
 */
CREATE OR REPLACE FUNCTION get_medications_and_tests_for_patient(patient_id BIGINT)
    RETURNS TABLE
            (
                name varchar(128),
                type varchar(20)
            )
AS
$$
BEGIN
    RETURN QUERY
        -- Список анализов и лекарств которые были выписаны определенному пациенту за последние 6 месяцев
        (
            SELECT CAST(analyses.name AS varchar(128)) AS name, CAST('Анализы' AS varchar(20)) AS type
            FROM users
                     INNER JOIN booked_doctors ON booked_doctors.patient_id = users.id
                     INNER JOIN doctor_appointments ON doctor_appointments.booked_doctor_id = booked_doctors.id
                     INNER JOIN analyse_appointments ON analyse_appointments.doctor_appointment_id = doctor_appointments.id
                     INNER JOIN analyses ON analyses.id = analyse_appointments.analyse_id
            WHERE users.id = get_medications_and_tests_for_patient.patient_id
              AND booked_doctors.appointment_time >= CURRENT_DATE - INTERVAL '6 months'
            UNION
            SELECT CAST(drugs.name AS varchar(128)) AS name, CAST('Лекарства' AS varchar(20)) AS type
            FROM users
                     INNER JOIN booked_doctors ON booked_doctors.patient_id = users.id
                     INNER JOIN doctor_appointments ON doctor_appointments.booked_doctor_id = booked_doctors.id
                     INNER JOIN prescription_drugs ON prescription_drugs.doctor_appointment_id = doctor_appointments.id
                     INNER JOIN drugs ON drugs.id = prescription_drugs.drug_id
            WHERE users.id = get_medications_and_tests_for_patient.patient_id
              AND booked_doctors.appointment_time >= CURRENT_DATE - INTERVAL '6 months'
        );
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_user_login(user_email TEXT, user_password TEXT) RETURNS BOOLEAN AS $$
DECLARE
    stored_password TEXT;
BEGIN
    -- Извлечение пароля из таблицы паролей
    SELECT p.pass INTO stored_password
    FROM users u
    JOIN passwords p ON u.id = p.user_id
    WHERE u.email = user_email;
    
    -- Если пользователь не найден, вернуть false
    IF NOT FOUND THEN
        RETURN FALSE;
    END IF;
    
    -- Проверка пароля
    IF stored_password = user_password THEN
        RETURN TRUE;
    ELSE
        RETURN FALSE;
    END IF;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION get_last_appointments_by_surname(patient_surname VARCHAR(50))
RETURNS TABLE (
    doctor_name TEXT,
    appointment_time TIMESTAMPTZ,
    complaint VARCHAR(2048)
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        CONCAT(d.name, ' ', d.surname) AS doctor_name,
        bd.appointment_time,
        da.complaint
    FROM 
        users p
        JOIN booked_doctors bd ON p.id = bd.patient_id
        JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
        JOIN users d ON bd.doctor_id = d.id
    WHERE 
        p.surname = patient_surname
    ORDER BY 
        bd.appointment_time DESC
    LIMIT 1;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION get_clinic_statistics(start_date DATE, end_date DATE)
RETURNS TABLE (
    total_patients INT,
    infectious_diseases INT,
    hypertension INT,
    diabetes INT,
    lung_diseases INT,
    total_sick_leaves INT,
    total_sick_leave_days INT
) AS $$
DECLARE
    total_patients_count INT;
    infectious_diseases_count INT;
    hypertension_count INT;
    diabetes_count INT;
    lung_diseases_count INT;
    total_sick_leaves_count INT;
    total_sick_leave_days_count INT;
BEGIN
    -- Подсчет общего количества пациентов
    SELECT COUNT(DISTINCT bd.patient_id)
    INTO total_patients_count
    FROM booked_doctors bd
    WHERE bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    -- Подсчет количества пациентов с инфекционными заболеваниями
    SELECT COUNT(DISTINCT bd.patient_id)
    INTO infectious_diseases_count
    FROM booked_doctors bd
    JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
    WHERE da.complaint ILIKE '%infection%'
    AND bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    -- Подсчет количества пациентов с гипертонией
    SELECT COUNT(DISTINCT bd.patient_id)
    INTO hypertension_count
    FROM booked_doctors bd
    JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
    WHERE da.complaint ILIKE '%hypertension%'
    AND bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    -- Подсчет количества пациентов с диабетом
    SELECT COUNT(DISTINCT bd.patient_id)
    INTO diabetes_count
    FROM booked_doctors bd
    JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
    WHERE da.complaint ILIKE '%diabetes%'
    AND bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    -- Подсчет количества пациентов с заболеваниями легких
    SELECT COUNT(DISTINCT bd.patient_id)
    INTO lung_diseases_count
    FROM booked_doctors bd
    JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
    WHERE da.complaint ILIKE '%lung%'
    AND bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    -- Подсчет общего количества выданных больничных
    SELECT COUNT(*)
    INTO total_sick_leaves_count
    FROM patient_sick_leave psl
    JOIN doctor_appointments da ON psl.doctor_appointment_id = da.id
    JOIN booked_doctors bd ON da.booked_doctor_id = bd.id
    WHERE bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    -- Подсчет общего количества дней больничных
    SELECT SUM(psl.end_date - psl.start_date)
    INTO total_sick_leave_days_count
    FROM patient_sick_leave psl
    JOIN doctor_appointments da ON psl.doctor_appointment_id = da.id
    JOIN booked_doctors bd ON da.booked_doctor_id = bd.id
    WHERE bd.appointment_time BETWEEN get_clinic_statistics.start_date AND get_clinic_statistics.end_date;

    RETURN QUERY SELECT
        COALESCE(total_patients_count, 0),
        COALESCE(infectious_diseases_count, 0),
        COALESCE(hypertension_count, 0),
        COALESCE(diabetes_count, 0),
        COALESCE(lung_diseases_count, 0),
        COALESCE(total_sick_leaves_count, 0),
        COALESCE(total_sick_leave_days_count, 0);
END;
$$ LANGUAGE plpgsql;


-- Создание функции триггера для проверки аллергенов
CREATE OR REPLACE FUNCTION check_allergens() RETURNS TRIGGER AS $$
DECLARE
    patient_id BIGINT;
    patient_allergens TEXT[];
    allergen TEXT;
    drug_description TEXT;
BEGIN
    -- Получение ID пациента
    SELECT bd.patient_id INTO patient_id
    FROM booked_doctors bd
    JOIN doctor_appointments da ON bd.id = da.booked_doctor_id
    WHERE da.id = NEW.doctor_appointment_id;

    -- Получение описания лекарства
    SELECT description INTO drug_description
    FROM drugs
    WHERE id = NEW.drug_id;
    
    -- Получение аллергенов пациента в виде массива
    SELECT ARRAY_AGG(a.name)
    INTO patient_allergens
    FROM users_allergen ua
    JOIN allergens a ON ua.allergen_id = a.id
    WHERE ua.user_id = patient_id;

    -- Проверка на наличие аллергенов в описании лекарства
    IF patient_allergens IS NOT NULL THEN
        FOR i IN ARRAY_LOWER(patient_allergens, 1) .. ARRAY_UPPER(patient_allergens, 1) LOOP
            allergen := patient_allergens[i];
            -- Отладочное сообщение для проверки значений
            RAISE NOTICE 'Checking allergen: % in drug description: %', allergen, drug_description;
            IF position(lower(allergen) IN lower(drug_description)) > 0 THEN
                RAISE EXCEPTION 'Cannot prescribe drug with allergen %', allergen;
            END IF;
        END LOOP;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION add_new_user(
    p_name VARCHAR(50),
    p_surname VARCHAR(50),
    p_middle_name VARCHAR(50),
    p_email custom_email,
    p_password TEXT,
    p_role_id INTEGER
) RETURNS TABLE (
    is_success BOOLEAN,
    message TEXT
) AS $$
DECLARE
    new_user_id BIGINT;
    role_exists BOOLEAN;
BEGIN
    -- Проверка существования роли
    SELECT EXISTS (SELECT 1 FROM roles WHERE id = p_role_id) INTO role_exists;
    
    IF NOT role_exists THEN
        RETURN QUERY SELECT FALSE, 'Role with ID ' || p_role_id || ' does not exist';
        RETURN;
    END IF;

    -- Вставка нового пользователя в таблицу users
    BEGIN
        INSERT INTO users (name, surname, middle_name, email)
        VALUES (p_name, p_surname, p_middle_name, p_email)
        RETURNING id INTO new_user_id;
    EXCEPTION WHEN others THEN
        RETURN QUERY SELECT FALSE, 'Failed to insert user into users table: ' || SQLERRM;
        RETURN;
    END;

    -- Вставка пароля пользователя в таблицу passwords
    BEGIN
        INSERT INTO passwords (user_id, pass)
        VALUES (new_user_id, p_password);
    EXCEPTION WHEN others THEN
        RETURN QUERY SELECT FALSE, 'Failed to insert password into passwords table: ' || SQLERRM;
        RETURN;
    END;

    -- Назначение роли пользователю
    BEGIN
        INSERT INTO users_roles (user_id, role_id)
        VALUES (new_user_id, p_role_id);
    EXCEPTION WHEN others THEN
        RETURN QUERY SELECT FALSE, 'Failed to insert role into users_roles table: ' || SQLERRM;
        RETURN;
    END;

    RETURN QUERY SELECT TRUE, 'User added successfully';
END;
$$ LANGUAGE plpgsql;

SELECT * FROM get_patients_by_doctor_and_date(8, '2024-05-10');
SELECT * FROM get_top_doctors_last_month();
SELECT * FROM get_patients_with_heart_disease_drugs();
SELECT * FROM get_patient_count_per_doctor_last_month();
SELECT * FROM get_medications_and_tests_for_patient(5);
SELECT * FROM check_user_login('ivan.ivanov1@example.com', '123456');
SELECT * FROM get_last_appointments_by_surname('Павлов');
SELECT * FROM get_clinic_statistics('2024-01-01', '2024-12-31');

SELECT * FROM add_new_user(
    'Іван',
    'Іванов',
    'Іванович',
    'ivan.ivanov@example.com',
    'password123',
    1 -- ID роли
);

-- Создание триггера для таблицы prescription_drugs
CREATE TRIGGER trigger_check_allergens
BEFORE INSERT ON prescription_drugs
FOR EACH ROW
EXECUTE FUNCTION check_allergens();



-- Вставка данных в таблицу prescription_drugs для тестирования
-- Этот запрос должен пройти, так как у пациента нет аллергии на компонент лекарства
INSERT INTO prescription_drugs (drug_id, doctor_appointment_id) VALUES
(1, 1); -- Парацетамол для пациента без аллергии

-- Этот запрос должен вызвать исключение, так как у пациента аллергия на "Пилок"
INSERT INTO prescription_drugs (drug_id, doctor_appointment_id) VALUES
(1, 11); -- Парацетамол для пациента с аллергией на "Пилок"

-- Этот запрос должен вызвать исключение, так как у пациента аллергия на "Пил"
INSERT INTO prescription_drugs (drug_id, doctor_appointment_id) VALUES
(2, 12); -- Ибупрофен для пациента с аллергией на "Пил"

-- Этот запрос должен пройти, так как у пациента нет аллергии на компоненты лекарства
INSERT INTO prescription_drugs (drug_id, doctor_appointment_id) VALUES
(3, 13); -- Амоксицилін для пациента без аллергии на компоненты


/* Descripiton:
 * Create a view displaying the list of diseases for the maximum number of patients of each doctor.
 */
CREATE VIEW max_diseases_per_doctor AS
WITH doctor_patient_disease AS (
    SELECT
        bd.doctor_id,
        da.complaint AS disease,
        COUNT(DISTINCT bd.patient_id) AS patient_count
    FROM
        booked_doctors bd
    JOIN
        doctor_appointments da ON bd.id = da.booked_doctor_id
    GROUP BY
        bd.doctor_id, da.complaint
),
max_patient_disease AS (
    SELECT
        doctor_id,
        disease,
        patient_count,
        ROW_NUMBER() OVER (PARTITION BY doctor_id ORDER BY patient_count DESC, disease) AS rn
    FROM
        doctor_patient_disease
)
SELECT
    d.id AS doctor_id,
    d.name AS doctor_name,
    d.surname AS doctor_surname,
    d.middle_name AS doctor_middle_name,
    m.disease AS most_popular_disease,
    m.patient_count
FROM
    max_patient_disease m
JOIN
    users d ON m.doctor_id = d.id
WHERE
    m.rn = 1;

-- DROP VIEW IF EXISTS max_diseases_per_doctor


/* Description:
 * Create a view to display the trend in the number of doctor visits (by visit dates).
 */
CREATE VIEW doctor_visit_trend AS
WITH daily_visits AS (
    SELECT
        date_trunc('day', bd.appointment_time) AS visit_date,
        COUNT(*) AS daily_visit_count
    FROM
        booked_doctors bd
    GROUP BY
        date_trunc('day', bd.appointment_time)
),
visit_trend AS (
    SELECT
        visit_date,
        daily_visit_count,
        LAG(daily_visit_count) OVER (ORDER BY visit_date) AS previous_daily_visit_count,
        LEAD(daily_visit_count) OVER (ORDER BY visit_date) AS next_daily_visit_count
    FROM
        daily_visits
)
SELECT
    visit_date,
    daily_visit_count,
    COALESCE(daily_visit_count - previous_daily_visit_count, 0) AS difference_with_previous_day,
    COALESCE(next_daily_visit_count - daily_visit_count, 0) AS difference_with_next_day
FROM
    visit_trend
ORDER BY
    visit_date;

-- DROP VIEW IF EXISTS doctor_visit_trend

CREATE OR REPLACE VIEW user_details_view AS
SELECT
    u.id,
    u.name,
    u.surname,
    u.middle_name,
    u.email,
    p.pass AS password,
    STRING_AGG(pn.country_code || pn.number, ',') AS phone_numbers,
    r.name AS role
FROM
    users u
LEFT JOIN
    phone_numbers pn ON u.id = pn.user_id
LEFT JOIN
    users_roles ur ON u.id = ur.user_id
LEFT JOIN
    roles r ON ur.role_id = r.id
LEFT JOIN
    passwords p ON u.id = p.user_id
GROUP BY
    u.id, u.name, u.surname, u.middle_name, u.email, p.pass, r.name
ORDER BY
    u.surname, u.name;


CREATE OR REPLACE VIEW booked_doctor_view AS
SELECT
    bd.id AS booked_id,
    bd.patient_id AS user_id,
    d.name AS doctor_name,
    d.surname AS doctor_surname,
    d.middle_name AS doctor_middle_name,
    bd.book_time AS booked_time,
    bd.appointment_time IS NOT NULL AS appointment_status
FROM
    booked_doctors bd
JOIN
    users d ON bd.doctor_id = d.id
ORDER BY
    d.surname, d.name;


CREATE OR REPLACE VIEW doctor_appointment_view AS
SELECT
    bd.id AS booked_id,
    bd.patient_id AS user_id,
    da.id AS appointment_id,
    bd.appointment_time,
    d.name AS doctor_name,
    d.surname AS doctor_surname,
    d.middle_name AS doctor_middle_name,
    da.complaint
FROM
    doctor_appointments da
JOIN
    booked_doctors bd ON da.booked_doctor_id = bd.id
JOIN
    users d ON bd.doctor_id = d.id
ORDER BY
    bd.appointment_time;


CREATE OR REPLACE VIEW descriptioned_drugs_view AS
SELECT
    da.id AS appointment_id,
    bd.patient_id AS user_id,
    d.name AS doctor_name,
    d.surname AS doctor_surname,
    d.middle_name AS doctor_middle_name,
    dr.name AS drug_name
FROM
    prescription_drugs pd
JOIN
    doctor_appointments da ON pd.doctor_appointment_id = da.id
JOIN
    booked_doctors bd ON da.booked_doctor_id = bd.id
JOIN
    users d ON bd.doctor_id = d.id
JOIN
    drugs dr ON pd.drug_id = dr.id
ORDER BY
    da.id;


CREATE OR REPLACE VIEW analyse_appointments_view AS
SELECT
    a.name as analyse_name,
    da.id AS appointment_id,
    bd.patient_id AS user_id,
    d.name AS doctor_name,
    d.surname AS doctor_surname,
    d.middle_name AS doctor_middle_name,
    aa.is_completed
FROM
    analyse_appointments aa
JOIN
    doctor_appointments da ON aa.doctor_appointment_id = da.id
JOIN
    booked_doctors bd ON da.booked_doctor_id = bd.id
JOIN
    users d ON bd.doctor_id = d.id
JOIN 
    analyses a ON aa.analyse_id = a.id
ORDER BY
    da.id;


-- DROP VIEW analyse_appointments_view;



SELECT * FROM max_diseases_per_doctor;
SELECT * FROM doctor_visit_trend;
SELECT * FROM user_details_view ORDER BY id;
SELECT * FROM booked_doctor_view ORDER BY user_id;
SELECT * FROM doctor_appointment_view;
SELECT * FROM descriptioned_drugs_view;
SELECT * FROM analyse_appointments_view;

SELECT * FROM booked_doctor_view WHERE user_id = 11;
SELECT * FROM booked_doctor_view 
WHERE user_id = 11
ORDER BY booked_time DESC
LIMIT 1;

SELECT * FROM doctor_appointment_view 
WHERE user_id = 11
ORDER BY appointment_time DESC
LIMIT 1;

SELECT * FROM descriptioned_drugs_view 
WHERE user_id = 11
ORDER BY appointment_id DESC
LIMIT 1;

SELECT * FROM analyse_appointments_view 
WHERE user_id = 11
ORDER BY appointment_id DESC
LIMIT 1;