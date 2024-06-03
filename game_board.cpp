#include "game_board.h"

// Конструктор класу game_board
game_board::game_board(int a_rows, int a_columns, int a_mines_number)
{
	/* Гравець повинен ввести розмір поля від 9x9 до 56x50
	   та кількість мін від 10 до 2695.
	   Максимальна кількість мін залежить від розміру поля,
	   якщо розмір поля AxB, максимальна кількість мін (A-1)x(B-1).
	*/

	fields = NULL; // Ініціалізація вказівника на масив полів

	// Встановлення розміру стовпців в межах від 9 до 56
	if (a_columns < 9) a_columns = 9;
	else if (a_columns > 56) a_columns = 56;

	// Встановлення розміру рядків в межах від 9 до 50
	if (a_rows < 9) a_rows = 9;
	else if (a_rows > 50) a_rows = 50;

	columns = a_columns; // Присвоєння значення стовпців
	rows = a_rows; // Присвоєння значення рядків
	this->create_fields(); // Створення полів (масиву)

	int max_mines_number = 0;
	max_mines_number = (columns - 1) * (rows - 1); // Обчислення максимальної кількості мін

	// Встановлення кількості мін в межах від 10 до max_mines_number
	if (a_mines_number < 10) a_mines_number = 10;
	else if (a_mines_number > max_mines_number) a_mines_number = max_mines_number;

	mines_number = a_mines_number; // Присвоєння значення кількості мін
	no_flagged_mines_number = mines_number; // Ініціалізація кількості не позначених мін
	this->rand_mines(); // Розташування мін на полі випадковим чином
	this->neighbours_mines(); // Підрахунок мін у сусідніх клітинках
}

// Деструктор класу game_board
game_board::~game_board()
{
	// Видалення динамічно виділеної пам'яті для полів
	for (int i = 0; i < rows; i++)
	{
		delete[] fields[i];
	}
	delete[] fields;
}

// Функція для створення масиву полів
void game_board::create_fields()
{
	fields = new field * [rows]; // Створення масиву вказівників на рядки

	// Ініціалізація кожного поля масиву
	for (int i = 0; i < rows; i++)
	{
		fields[i] = new field[columns]; // Створення масиву полів для кожного рядка

		for (int j = 0; j < columns; j++)
		{
			fields[i][j].value = 0; // Значення поля (0 означає, що немає мін поруч)
			fields[i][j].flagged = false; // Поле не позначено прапорцем
			fields[i][j].discovered = false; // Поле не відкрито
			fields[i][j].last_clicked = false; // Поле не було останнім натиснутим
		}
	}
}

// Функція для випадкового розташування мін на полі
void game_board::rand_mines()
{
	int X = 0;
	int Y = 0;

	// Розташування мін в випадкових місцях поля
	for (int i = 0; i < mines_number; i++)
	{
		do
		{
			X = rand() % rows; // Випадковий рядок
			Y = rand() % columns; // Випадковий стовпець
		} while (fields[X][Y].value == -1); // Повторення, якщо в цьому місці вже є міна

		fields[X][Y].value = -1; // Встановлення міни в поле
	}
}

// Функція для підрахунку кількості мін у сусідніх клітинках
void game_board::neighbours_mines()
{


	// Прохід по всіх полях
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			// Якщо поле містить міну, збільшуємо значення сусідніх полів
			if (fields[i][j].value == -1)
			{
				// Вгору
				if ((i - 1 >= 0) && (fields[i - 1][j].value != -1))
				{
					fields[i - 1][j].value++;
				}
				// Вправо
				if ((j + 1 < columns) && (fields[i][j + 1].value != -1))
				{
					fields[i][j + 1].value++;
				}
				// Вниз
				if ((i + 1 < rows) && (fields[i + 1][j].value != -1))
				{
					fields[i + 1][j].value++;
				}
				// Вліво
				if ((j - 1 >= 0) && (fields[i][j - 1].value != -1))
				{
					fields[i][j - 1].value++;
				}
				// Вгору вліво по діагоналі
				if ((i - 1 >= 0) && (j - 1 >= 0) && (fields[i - 1][j - 1].value != -1))
				{
					fields[i - 1][j - 1].value++;
				}
				// Вгору вправо по діагоналі
				if ((i - 1 >= 0) && (j + 1 < columns) && (fields[i - 1][j + 1].value != -1))
				{
					fields[i - 1][j + 1].value++;
				}
				// Вниз вліво по діагоналі
				if ((i + 1 < rows) && (j - 1 >= 0) && (fields[i + 1][j - 1].value != -1))
				{
					fields[i + 1][j - 1].value++;
				}
				// Вниз вправо по діагоналі
				if ((i + 1 < rows) && (j + 1 < columns) && (fields[i + 1][j + 1].value != -1))
				{
					fields[i + 1][j + 1].value++;
				}
			}
		}
	}
}
