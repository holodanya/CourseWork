#include <iostream>
#include <stdio.h>
#include "time.h"
#include <string.h>

// Структура для представлення поля гри
struct field
{
	int value;       // Значення поля: -1 для міни, 0..8 для кількості сусідніх мін
	bool flagged;    // Прапорець встановлений
	bool discovered; // Гравець відкрив значення поля
	bool last_clicked; // Останній натиснутий
};
#pragma once
class game_board
{
private:
	int rows; // Кількість рядків на полі
	int columns; // Кількість стовпців на полі
	int mines_number; // Кількість мін на полі
	field** fields; // Двовимірний масив для зберігання полів гри
	void create_fields(); // Створення матриці полів
	void rand_mines(); // Заповнення матриці випадково розташованими мінами
	void neighbours_mines(); // Зміна значень полів, суміжних з мінами, на відповідні значення
	bool victory = false; // Змінна для відображення статусу перемоги
	bool END_OF_GAME = false; // Змінна для відображення кінця гри
	int no_flagged_mines_number; // Кількість мін, що залишилися непоміченими
public:
	game_board(int rows = 9, int columns = 9, int mines_number = 10);
	~game_board();
	int get_columns() { return columns; }
	int get_rows() { return rows; }
	int get_mines_number() { return mines_number; }
	field& get_fields(int x_pos, int y_pos) { return fields[x_pos][y_pos]; }
	void change_victory() { victory = true; }
	bool get_victory() { return victory; }
	void change_END_OF_GAME() { END_OF_GAME = true; }
	bool get_END_OF_GAME() { return END_OF_GAME; }
	int get_no_flagged_mines_number() { return no_flagged_mines_number; }
	void add_one_to_no_flagged_mines_number() { no_flagged_mines_number++; }
	void substract_one_from_no_flagged_mines_number() { no_flagged_mines_number--; }
};