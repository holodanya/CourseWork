#include "HighScore.h"
#include <fstream>

// Конструктор класу HighScore
HighScore::HighScore()
{
    // Ініціалізація класу (порожній конструктор)
}

// Деструктор класу HighScore
HighScore::~HighScore()
{
    // Деструктор класу (порожній деструктор)
}

// Функція для скидання рекордів
void HighScore::reset_HighScores()
{
    // Відкриваємо файл у режимі "truncate" для очищення його вмісту
    std::fstream file;
    file.open("HighScores.txt", std::ios::out | std::ios::trunc); // Відкриваємо файл для запису, очищаючи його
    file.close(); // Закриваємо файл

    // Скидаємо рекорди, які зберігаються в пам'яті
    for (int i = 0; i < 9; ++i)
    {
        HighScores[i].name = ""; // Очищуємо ім'я гравця
        HighScores[i].time = 0; // Скидаємо час
        HighScores[i].rows = 0; // Скидаємо кількість рядків
        HighScores[i].cols = 0; // Скидаємо кількість стовпців
        HighScores[i].mines = 0; // Скидаємо кількість мін
    }

    // Скидаємо лічильник рекордів
    recordCount = 0;
}

// Функція для скидання рекордів у випадку переповнення
void HighScore::reset_HighScores_overflow()
{
    // Відкриваємо файл у режимі "truncate" для очищення його вмісту
    std::fstream file;
    file.open("HighScores.txt", std::ios::out | std::ios::trunc); // Відкриваємо файл для запису, очищаючи його
    file.close(); // Закриваємо файл

    // Зберігаємо останнього переможця
    score lastWinner = HighScores[7]; // Зберігаємо останній запис до змінної
    std::string player_name = HighScores[7].name; // Ім'я гравця
    int your_time = HighScores[7].time; // Час гри
    int rows = HighScores[7].rows; // Кількість рядків
    int cols = HighScores[7].cols; // Кількість стовпців
    int mines = HighScores[7].mines; // Кількість мін

    // Скидаємо рекорди, які зберігаються в пам'яті
    for (int i = 0; i < 9; i++)
    {
        HighScores[i].name = ""; // Очищуємо ім'я гравця
        HighScores[i].time = 0; // Скидаємо час
        HighScores[i].rows = 0; // Скидаємо кількість рядків
        HighScores[i].cols = 0; // Скидаємо кількість стовпців
        HighScores[i].mines = 0; // Скидаємо кількість мін
    }
    HighScores[0] = lastWinner; // Зберігаємо останнього переможця як перший запис
}

// Функція для завантаження рекордів з файлу
void HighScore::load_HighScores()
{
    std::fstream file;
    file.open("HighScores.txt", std::ios::in); // Відкриваємо файл у режимі читання
    if (!file.is_open()) // Якщо файл не відкрився
    {
        reset_HighScores(); // Скидаємо рекорди
        return;
    }

    std::string line;
    int lineNum = 0;
    recordCount = 0; // Ініціалізуємо лічильник рекордів

    while (std::getline(file, line)) // Читаємо файл по рядках
    {
        int index = lineNum / 5; // Кожен запис складається з 5 рядків
        switch (lineNum % 5)
        {
        case 0:
            HighScores[index].name = line; // Зберігаємо ім'я гравця
            break;
        case 1:
            HighScores[index].time = std::stoi(line); // Зберігаємо час гри
            break;
        case 2:
            HighScores[index].rows = std::stoi(line); // Зберігаємо кількість рядків
            break;
        case 3:
            HighScores[index].cols = std::stoi(line); // Зберігаємо кількість стовпців
            break;
        case 4:
            HighScores[index].mines = std::stoi(line); // Зберігаємо кількість мін
            recordCount++; // Збільшуємо лічильник рекордів після зчитування повного запису
            break;
        }
        lineNum++;
    }
    file.close(); // Закриваємо файл
}

// Функція для збереження рекордів у файл
void HighScore::save_HighScores(LPCSTR player_name, int your_time, int rows, int cols, int mines)
{
    std::ofstream outFile("highscores.txt", std::ios::app); // Відкриваємо файл у режимі додавання
    if (outFile.is_open()) // Якщо файл відкрився успішно
    {
        outFile << player_name << std::endl; // Записуємо ім'я гравця
        outFile << your_time << std::endl; // Записуємо час гри
        outFile << rows << std::endl; // Записуємо кількість рядків
        outFile << cols << std::endl; // Записуємо кількість стовпців
        outFile << mines << std::endl; // Записуємо кількість мін
        outFile.close(); // Закриваємо файл
    }
}
