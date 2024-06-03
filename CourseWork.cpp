#include <windows.h>
#include <string>
#include "game_board.h"
#include "Resource.h"
#include <fstream>
#include "HighScore.h"
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#define horizontal_coordinates (horizontal / 2 -  get_window_width() / 2)
#define vertical_coordinates (vertical / 2 - get_window_height() / 2)

// Оголошення функцій
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HighScoresProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CustomProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewScoreProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

int get_window_width();
int get_window_height();
void GetDesktopResolution(int& horizontal, int& vertical);
HWND** get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance);
void load_BITMAPS_and_ICON();
void check_neighbours(int g_b_X, int g_b_Y);
void neighbour_value(DWORD_PTR dwRefData, int g_b_X, int g_b_Y);
void delete_buttons(int old_rows, int old_buttons);
void clear_old_window_change_its_pos_and_dim(int old_rows, int old_buttons);
void unpressed_clear_button_normal_face();
void play_again_or_change_board(std::string again_or_board, int rows = 9, int columns = 9, int mines = 10);
void check_if_win();
void save_HighScores(int your_time, int rows, int columns, int mines);

// Глобальні змінні
HBITMAP hbit_BMPs[18]; // Масив для зберігання зображень кнопок

game_board* g_b_gameboard; // Об'єкт ігрової дошки
HighScore* high_score; // Об'єкт для зберігання результатів

HWND hwnd; // Головне вікно
HWND** hwnd_matrix; // Матриця хендлів кнопок
HINSTANCE hInstance; // Ідентифікатор екземпляру програми

HBITMAP BitMap; // Хендл для BitMap

// Основна функція WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg; // Структура для зберігання повідомлень

	MyRegisterClass(hInstance); // Реєстрація класу вікна

	// Створення вікна програми
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	// Основний цикл обробки повідомлень
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

// Функція реєстрації класу вікна
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "Minesweeper";
	wcex.hIconSm = 0;

	return RegisterClassEx(&wcex); // Реєстрація класу вікна
}

// Функція ініціалізації екземпляра програми
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	srand(time(NULL)); // Ініціалізація генератора випадкових чисел
	g_b_gameboard = new game_board(); // Створення нового об'єкта ігрової дошки
	high_score = new HighScore(); // Створення нового об'єкта для зберігання результатів

	int horizontal = 0;
	int vertical = 0;

	GetDesktopResolution(horizontal, vertical); // Отримання роздільної здатності екрану

	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(1000)); // Завантаження меню

	// Створення головного вікна
	hwnd = CreateWindowEx(WS_EX_WINDOWEDGE,
		"Minesweeper", "Minesweeper",
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
		horizontal_coordinates,
		vertical_coordinates,
		get_window_width(),
		get_window_height(),
		NULL,
		hMenu,
		hInstance,
		NULL);

	if (hwnd == NULL)
	{
		return FALSE;
	}

	// Створення кнопки усмішки
	HWND hwnd_smile = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
		get_window_width() / 2 - 14, 5, 26, 26, hwnd, (HMENU)20001, hInstance, 0);

	load_BITMAPS_and_ICON(); // Завантаження зображень та іконок
	SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); // Встановлення зображення усмішки на кнопку

	hwnd_matrix = get_hwnd_matrix(hwnd, (HINSTANCE)GetModuleHandle(NULL)); // Отримання матриці хендлів кнопок
	unpressed_clear_button_normal_face(); // Встановлення початкового стану кнопок


	ShowWindow(hwnd, nCmdShow); // Показ вікна
	UpdateWindow(hwnd); // Оновлення вікна

	return TRUE;
}

// Головна процедура обробки повідомлень для вікна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc; 
	HDC hdcBitmap; 
	PAINTSTRUCT ps; 

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_F5)
		{
			play_again_or_change_board("again"); // Почати гру знову при натисканні F5
		}
		break;

	case WM_TIMER:
	{
		if (high_score->get_TIMER() < 999)
		{
			high_score->add_one_to_TIMER(); // Додавання однієї секунди до таймера
			InvalidateRect(hwnd, NULL, FALSE); // Оновлення вікна
		}
	}
	break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps); 
		hdcBitmap = CreateCompatibleDC(hdc); 

		BitMap = (HBITMAP)SelectObject(hdcBitmap, BitMap);
		// Малювання цифр таймера
		BitBlt(hdc, get_window_width() - 66, 6, 15, 23, hdcBitmap, 17 * ((high_score->get_TIMER() / 100) % 10) + 1, 0, SRCCOPY);
		BitBlt(hdc, get_window_width() - 51, 6, 15, 23, hdcBitmap, 17 * ((high_score->get_TIMER() / 10) % 10) + 1, 0, SRCCOPY);
		BitBlt(hdc, get_window_width() - 36, 6, 15, 23, hdcBitmap, 17 * (high_score->get_TIMER() % 10) + 1, 0, SRCCOPY);

		// Малювання кількості невідкритих мін
		if (g_b_gameboard->get_no_flagged_mines_number() >= 0)
		{
			BitBlt(hdc, 4, 6, 15, 23, hdcBitmap, 1, 0, SRCCOPY);
		}
		else
		{
			BitBlt(hdc, 4, 6, 15, 23, hdcBitmap, 17 * 10 + 1, 0, SRCCOPY);
		}
		BitBlt(hdc, 19, 6, 15, 23, hdcBitmap, abs(17 * ((g_b_gameboard->get_no_flagged_mines_number() / 100) % 10)) + 1, 0, SRCCOPY);
		BitBlt(hdc, 34, 6, 15, 23, hdcBitmap, abs(17 * ((g_b_gameboard->get_no_flagged_mines_number() / 10) % 10)) + 1, 0, SRCCOPY);
		BitBlt(hdc, 49, 6, 15, 23, hdcBitmap, abs(17 * (g_b_gameboard->get_no_flagged_mines_number() % 10)) + 1, 0, SRCCOPY);

		BitMap = (HBITMAP)SelectObject(hdcBitmap, BitMap); // Повернення старого BitMap
		DeleteDC(hdcBitmap); // Видалення сумісного контексту пристрою
		EndPaint(hwnd, &ps); // Закінчення малювання
	}
	break;

	case WM_CLOSE:
		DestroyWindow(hwnd); // Знищення вікна при закритті
		break;

	case WM_DESTROY:
		delete g_b_gameboard; // Видалення об'єкта ігрової дошки
		delete high_score; // Видалення об'єкта для зберігання результатів
		KillTimer(hwnd, ID_TIMER); // Видалення таймера
		PostQuitMessage(0); // Вихід з програми
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 1001: 
			play_again_or_change_board("again"); // Почати нову гру
			break;
		case 1002: 
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_CUSTOM),
				hwnd, reinterpret_cast<DLGPROC>(CustomProc)); // Показ діалогового вікна для налаштування поля
			break;
		case 1003: 
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_HIGHSCORES),
				hwnd, reinterpret_cast<DLGPROC>(HighScoresProc)); // Показ діалогового вікна з рекордами
			break;
		case 1004:
			DestroyWindow(hwnd); // Вихід з програми
			break;
		//case 1005: 
		//	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_ABOUT),
		//		hwnd, reinterpret_cast<DLGPROC>(AboutProc)); // Показ діалогового вікна "Про програму"
		//	break;
		}
		if (HIWORD(wParam) == BN_CLICKED)
		{
			int button_id = LOWORD(wParam);
			if (button_id == 20001) 
			{
				play_again_or_change_board("again"); // Почати нову гру при натисканні кнопки усмішки
				SetFocus(hwnd); // Встановити фокус на головне вікно
			}
		}
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam); 
	}

	return 0;
}

// Процедура обробки повідомлень для кнопок
LRESULT CALLBACK ButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (message)
	{
	case WM_RBUTTONDOWN: // Обробка натискання правої кнопки миші
	{
		int button_id = GetDlgCtrlID(hWnd); // Отримання ID кнопки
		int g_b_X = button_id / g_b_gameboard->get_columns(); // Визначення рядка кнопки
		int g_b_Y = button_id % g_b_gameboard->get_columns(); // Визначення стовпця кнопки

		if (g_b_gameboard->get_END_OF_GAME() == false) // Перевірка чи гра не закінчена
		{
			if (g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == false && g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered == false) // Якщо клітинка не позначена прапорцем і не відкрита
			{
				SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[17]); // Встановлення зображення прапорця
				g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged = true; // Позначення клітинки прапорцем

				g_b_gameboard->substract_one_from_no_flagged_mines_number(); // Зменшення кількості невідкритих мін
				InvalidateRect(hWnd, NULL, FALSE); // Оновлення вікна
			}
			else if (g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == true && g_b_gameboard->get_END_OF_GAME() == false) // Якщо клітинка позначена прапорцем
			{
				SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]); // Зняття прапорця
				g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged = false; // Відміна позначки прапорцем

				g_b_gameboard->add_one_to_no_flagged_mines_number(); // Збільшення кількості невідкритих мін
				InvalidateRect(hWnd, NULL, FALSE); // Оновлення вікна
			}
		}
	}
	break;

	case WM_LBUTTONDOWN: // Обробка натискання лівої кнопки миші
		if (g_b_gameboard->get_END_OF_GAME() == false) // Якщо гра не закінчена
		{
			SendMessage(GetDlgItem(GetParent(hWnd), 20001), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]); // Зміна зображення усмішки
		}
		break;

	case WM_LBUTTONUP: // Обробка відпускання лівої кнопки миші
		if (g_b_gameboard->get_END_OF_GAME() == false) // Якщо гра не закінчена
		{
			int button_id = GetDlgCtrlID(hWnd); // Отримання ID кнопки
			int g_b_X = button_id / g_b_gameboard->get_columns(); // Визначення рядка кнопки
			int g_b_Y = button_id % g_b_gameboard->get_columns(); // Визначення стовпця кнопки

			SendMessage(GetDlgItem(GetParent(hWnd), 20001), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); // Зміна зображення усмішки

			if (!high_score->get_Started_TIMER()) { // Якщо таймер не запущено
				SetTimer(GetParent(hWnd), ID_TIMER, 1000, NULL); // Запуск таймера
				high_score->change_Started_TIMER_true(); // Встановлення флагу запущеного таймера
			}

		if (g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == false) // Якщо клітинка не позначена прапорцем
				SendMessage(hWnd, BM_SETSTATE, TRUE, NULL); // Зміна стану кнопки

			if (g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered == false && g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == false) // Якщо клітинка не відкрита і не позначена прапорцем
			{
				g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered = true; // Відкриття клітинки
				switch (g_b_gameboard->get_fields(g_b_X, g_b_Y).value)
				{
				case -1: // Якщо клітинка містить міну
					if (g_b_gameboard->get_END_OF_GAME() == false)
					{
						KillTimer(GetParent(hWnd), ID_TIMER); // Зупинка таймера
						SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[11]); // Встановлення зображення міни
						g_b_gameboard->get_fields(g_b_X, g_b_Y).last_clicked = true; // Позначення клітинки як останньої натиснутої
						SendMessage(GetDlgItem(GetParent(hWnd), 20001), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[13]); // Зміна зображення усмішки

						// Відкриття всіх мін на полі
						for (int i = 0; i < g_b_gameboard->get_rows(); i++)
						{
							for (int j = 0; j < g_b_gameboard->get_columns(); j++)
							{
								if (g_b_gameboard->get_fields(i, j).value == -1 && g_b_gameboard->get_fields(i, j).discovered == false)
								{
									if (g_b_gameboard->get_fields(i, j).flagged == false)
									{
										SendMessage(GetDlgItem(GetParent(hWnd), g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[9]); // Встановлення зображення міни
										g_b_gameboard->get_fields(i, j).discovered = true; // Відкриття клітинки
									}
								}
								else if (g_b_gameboard->get_fields(i, j).value != -1 && g_b_gameboard->get_fields(i, j).flagged == true)
								{
									SendMessage(GetDlgItem(GetParent(hWnd), g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[10]); // Встановлення зображення неправильно позначеної міни
									g_b_gameboard->get_fields(i, j).discovered = true; // Відкриття клітинки
								}

							}
						}
						g_b_gameboard->change_END_OF_GAME(); // Встановлення флагу закінчення гри
					}
					break;
				case 0: // Якщо клітинка не містить мін поруч
					g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered = true; // Відкриття клітинки
					SendMessage(hWnd, BM_SETSTATE, TRUE, NULL); // Зміна стану кнопки
					SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[0]); // Встановлення зображення порожньої клітинки
					check_neighbours(g_b_X, g_b_Y); // Перевірка сусідніх клітинок
					break;
				default:
					SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[g_b_gameboard->get_fields(g_b_X, g_b_Y).value]); // Встановлення відповідного зображення
					break;
				}

				check_if_win(); // Перевірка на перемогу
			}
		}
		break;

	default:
		return DefSubclassProc(hWnd, message, wParam, lParam); // Виклик стандартної обробки повідомлень
	}

	return 0;
}

// Процедура обробки повідомлень для діалогового вікна з рекордами
LRESULT CALLBACK HighScoresProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	int ID_CTRL_NAME;
	int ID_CTRL_TIME;
	int ID_CTRL_ROWS;
	int ID_CTRL_COLUMNS;
	int ID_CTRL_MINES;
	char buffer_for_time[10];
	char buffer_for_rows[10];
	char buffer_for_cols[10];
	char buffer_for_mines[10];
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		ID_CTRL_NAME = 1016;
		ID_CTRL_TIME = 1017;
		ID_CTRL_ROWS = 1018;
		ID_CTRL_COLUMNS = 1019;
		ID_CTRL_MINES = 1020;
		high_score->load_HighScores(); // Завантаження рекордів
		int recordCount = high_score->get_record_count(); // Отримання кількості рекордів
		for (int i = 0; i < recordCount; i++)
		{
			SetDlgItemText(hWndDlg, ID_CTRL_NAME, high_score->get_i_HighScores_name(i)); // Встановлення імені гравця
			ID_CTRL_NAME += 5;

			_itoa_s(high_score->get_i_HighScores_time(i), buffer_for_time, 10);
			SetDlgItemText(hWndDlg, ID_CTRL_TIME, buffer_for_time); // Встановлення часу гри
			ID_CTRL_TIME += 5;

			_itoa_s(high_score->get_i_HighScores_rows(i), buffer_for_rows, 10);
			SetDlgItemText(hWndDlg, ID_CTRL_ROWS, buffer_for_rows); // Встановлення кількості рядків
			ID_CTRL_ROWS += 5;

			_itoa_s(high_score->get_i_HighScores_cols(i), buffer_for_cols, 10);
			SetDlgItemText(hWndDlg, ID_CTRL_COLUMNS, buffer_for_cols); // Встановлення кількості стовпців
			ID_CTRL_COLUMNS += 5;

			_itoa_s(high_score->get_i_HighScores_mines(i), buffer_for_mines, 10);
			SetDlgItemText(hWndDlg, ID_CTRL_MINES, buffer_for_mines); // Встановлення кількості мін
			ID_CTRL_MINES += 5;
		}
		return TRUE;
	}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_OK:
		{
			EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
			break;
		}
		case ID_RESET:
		{
			int record_count = high_score->get_record_count(); // Отримання кількості рекордів
			high_score->reset_HighScores(); // Скидання рекордів

			ID_CTRL_NAME = 1016;
			ID_CTRL_TIME = 1017;
			ID_CTRL_ROWS = 1018;
			ID_CTRL_COLUMNS = 1019;
			ID_CTRL_MINES = 1020;
			for (int i = 0; i < record_count; i++) // Очищення полів рекордів
			{
				SetDlgItemText(hWndDlg, ID_CTRL_NAME, "");
				SetDlgItemText(hWndDlg, ID_CTRL_TIME, "");
				SetDlgItemText(hWndDlg, ID_CTRL_ROWS, "");
				SetDlgItemText(hWndDlg, ID_CTRL_COLUMNS, "");
				SetDlgItemText(hWndDlg, ID_CTRL_MINES, "");

				ID_CTRL_NAME += 5;
				ID_CTRL_TIME += 5;
				ID_CTRL_ROWS += 5;
				ID_CTRL_COLUMNS += 5;
				ID_CTRL_MINES += 5;
			}

			break;
		}
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
		break;
	}
	break;

	default: return FALSE; // Виклик стандартної обробки повідомлень для діалогового вікна
	}

	return FALSE;
}


// Процедура обробки повідомлень для діалогового вікна "Про програму"
LRESULT CALLBACK AboutProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		// Ініціалізація діалогового вікна
	case WM_INITDIALOG:
		return TRUE; // Повернення TRUE для вказівки на успішну ініціалізацію

		// Обробка команд
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) // Перевірка ID команди
		{
		case ID_OK: // Якщо натиснута кнопка OK
		{
			EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
			break;
		}
		}
		break;
	}

	// Якщо діалогове вікно закривається
	case WM_CLOSE:
	{
		EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
		break;
	}
	break;

	default: return FALSE; // Виклик стандартної обробки повідомлень для діалогового вікна
	}

	return FALSE;
}



// Процедура обробки повідомлень для діалогового вікна налаштувань гри
LRESULT CALLBACK CustomProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG: // Ініціалізація діалогового вікна
	{
		char help_buffer[5]; // Буфер для зберігання чисел у вигляді рядка
		_itoa_s(g_b_gameboard->get_rows(), help_buffer, 5, 10); // Перетворення кількості рядків у рядок
		SetWindowText(GetDlgItem(hWndDlg, IDC_ROWS_T), help_buffer); // Встановлення тексту для поля вводу рядків
		_itoa_s(g_b_gameboard->get_columns(), help_buffer, 5, 10); // Перетворення кількості стовпців у рядок
		SetWindowText(GetDlgItem(hWndDlg, IDC_COLUMNS_T), help_buffer); // Встановлення тексту для поля вводу стовпців
		_itoa_s(g_b_gameboard->get_mines_number(), help_buffer, 5, 10); // Перетворення кількості мін у рядок
		SetWindowText(GetDlgItem(hWndDlg, IDC_MINES_T), help_buffer); // Встановлення тексту для поля вводу мін
		return TRUE; // Повернення TRUE для успішної ініціалізації
	}

	case WM_COMMAND: // Обробка команд
	{
		switch (LOWORD(wParam)) // Перевірка ID команди
		{
		case ID_OK: // Якщо натиснута кнопка OK
		{
			TCHAR rows_buffer[7]; // Буфер для зберігання кількості рядків
			TCHAR columns_buffer[7]; // Буфер для зберігання кількості стовпців
			TCHAR mines_buffer[7]; // Буфер для зберігання кількості мін
			GetWindowText(GetDlgItem(hWndDlg, IDC_ROWS_T), rows_buffer, 7); // Отримання тексту з поля вводу рядків
			GetWindowText(GetDlgItem(hWndDlg, IDC_COLUMNS_T), columns_buffer, 7); // Отримання тексту з поля вводу стовпців
			GetWindowText(GetDlgItem(hWndDlg, IDC_MINES_T), mines_buffer, 7); // Отримання тексту з поля вводу мін
			play_again_or_change_board("board", atoi(rows_buffer), atoi(columns_buffer), atoi(mines_buffer)); // Зміна налаштувань гри
			EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
			break;
		}
		case ID_CANCEL: // Якщо натиснута кнопка Cancel
		{
			EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
			break;
		}
		}
		break;
	}
	case WM_CLOSE: // Якщо діалогове вікно закривається
	{
		EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
		break;
	}
	break;

	default: return FALSE; // Виклик стандартної обробки повідомлень для діалогового вікна
	}

	return FALSE;
}

// Процедура обробки повідомлень для діалогового вікна введення нового рекорду
LRESULT CALLBACK NewScoreProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	std::string congratulations; // Рядок для зберігання повідомлення привітання
	switch (Msg)
	{
	case WM_INITDIALOG: // Ініціалізація діалогового вікна
	{
		congratulations = "Congratulations!\nYou have won the game of Minesweeper\nPlease enter your name for the Leadeboard."; // Формування повідомлення привітання
		SetDlgItemText(hWndDlg, ID_CONGRATULATIONS, congratulations.c_str()); // Встановлення тексту привітання у відповідному полі
		SetWindowText(GetDlgItem(hWndDlg, IDC_NAME), "Player"); // Встановлення тексту за замовчуванням для поля введення імені
		return TRUE; // Повернення TRUE для успішної ініціалізації
	}

	case WM_COMMAND: // Обробка команд
	{
		switch (LOWORD(wParam)) // Перевірка ID команди
		{
		case ID_OK: // Якщо натиснута кнопка OK
		{
			std::string player_name = high_score->get_PlayerName(); // Отримання імені гравця з об'єкта high_score
			GetWindowTextA(GetDlgItem(hWndDlg, IDC_NAME), high_score->get_PlayerName(), 17); // Отримання тексту з поля введення імені
			EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
			break;
		}
		}
		break;
	}
	case WM_CLOSE: // Якщо діалогове вікно закривається
	{
		EndDialog(hWndDlg, NULL); // Закриття діалогового вікна
		break;
	}
	break;

	default: return FALSE; // Виклик стандартної обробки повідомлень для діалогового вікна
	}

	return FALSE;
}

// Функція для отримання ширини вікна
int get_window_width()
{
	int width = 0;
	width = 18 + 16 * g_b_gameboard->get_columns(); // Обчислення ширини вікна: 18 пікселів + 16 пікселів за кожен стовпець
	return width;
}

// Функція для отримання висоти вікна
int get_window_height()
{
	int height = 0;
	height = 96 + 16 * g_b_gameboard->get_rows(); // Обчислення висоти вікна: 96 пікселів + 16 пікселів за кожен рядок
	return height;
}

// Функція для отримання роздільної здатності екрану
// Параметри horizontal і vertical міститимуть ширину та висоту екрану відповідно
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;

	// Отримання хендла робочого столу
	const HWND hDesktop = GetDesktopWindow();
	// Отримання розмірів екрану в змінну desktop
	GetWindowRect(hDesktop, &desktop);
	// Верхній лівий кут буде мати координати (0,0), а нижній правий кут буде мати координати (horizontal, vertical)
	horizontal = desktop.right; // Ширина екрану
	vertical = desktop.bottom;  // Висота екрану
}

// Функція для створення матриці хендлів кнопок
// Повертає двовимірний масив хендлів кнопок
HWND** get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance)
{
	int button_number = 0;

	// Виділення пам'яті для рядків матриці
	HWND** hwnd_matrix = new HWND * [g_b_gameboard->get_rows()];

	for (int i = 0; i < g_b_gameboard->get_rows(); i++)
	{
		// Виділення пам'яті для стовпців матриці
		hwnd_matrix[i] = new HWND[g_b_gameboard->get_columns()];

		for (int j = 0; j < g_b_gameboard->get_columns(); j++)
		{
			// Створення кнопки і додавання її в матрицю
			hwnd_matrix[i][j] = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_NOTIFY | BS_PUSHBUTTON,
				1 + 16 * j, 35 + 16 * i, 16, 16, hwnd, (HMENU)button_number, hInstance, 0);
			button_number++;

			// Призначення підкласу для кожної кнопки
			SetWindowSubclass(hwnd_matrix[i][j], ButtonProc, 0, button_number - 1);
		}
	}
	return hwnd_matrix;
}

// Функція для завантаження зображень та іконок
void load_BITMAPS_and_ICON()
{
	std::string name = "";
	char help_bufor[3] = "";

	for (int i = 0; i < 18; i++)
	{
		_itoa_s(i, help_bufor, 10); // Перетворення числа в рядок
		name = "BMPs\\" + std::string(help_bufor) + ".bmp"; // Формування шляху до зображення
		hbit_BMPs[i] = (HBITMAP)LoadImage(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE); // Завантаження зображення
	}

	// Завантаження іконки
	name = "BMPs\\icon.ico";
	HANDLE icon = LoadImage(hInstance, name.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	if (!icon)
	{
		MessageBox(hwnd, "Could not load icon.ico!", "Error", MB_OK | MB_ICONERROR); // Виведення повідомлення про помилку, якщо іконку не вдалося завантажити
	}
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon); // Встановлення великої іконки для вікна
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon); // Встановлення маленької іконки для вікна

	// Завантаження зображення для цифр таймера
	name = "BMPs\\Digits.bmp";
	BitMap = (HBITMAP)LoadImage(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!BitMap)
	{
		MessageBox(hwnd, "There's problem with opening \"Digits.bmp\"", "No File", MB_ICONERROR); // Виведення повідомлення про помилку, якщо зображення не вдалося завантажити
	}
}


// Функція для перевірки сусідніх клітинок навколо даної координати (g_b_X, g_b_Y)
void check_neighbours(int g_b_X, int g_b_Y)
{
	// Перевірка клітинки зверху
	if ((g_b_X - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y).discovered == false) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y).flagged == false)) // вгору
	{
		g_b_gameboard->get_fields(g_b_X - 1, g_b_Y).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1) * g_b_gameboard->get_columns() + g_b_Y), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X - 1) * g_b_gameboard->get_columns() + g_b_Y, g_b_X - 1, g_b_Y); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки справа
	if ((g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->get_fields(g_b_X, g_b_Y + 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X, g_b_Y + 1).flagged == false)) // вправо
	{
		g_b_gameboard->get_fields(g_b_X, g_b_Y + 1).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X, g_b_Y + 1); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки знизу
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y).discovered == false) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y).flagged == false)) // вниз
	{
		g_b_gameboard->get_fields(g_b_X + 1, g_b_Y).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1) * g_b_gameboard->get_columns() + g_b_Y), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X + 1) * g_b_gameboard->get_columns() + g_b_Y, g_b_X + 1, g_b_Y); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки зліва
	if ((g_b_Y - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X, g_b_Y - 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X, g_b_Y - 1).flagged == false)) // вліво
	{
		g_b_gameboard->get_fields(g_b_X, g_b_Y - 1).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X, g_b_Y - 1); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки вгору вліво по діагоналі
	if ((g_b_X - 1 >= 0) && (g_b_Y - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y - 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y - 1).flagged == false)) // вгору вліво по діагоналі
	{
		g_b_gameboard->get_fields(g_b_X - 1, g_b_Y - 1).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1) * g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X - 1) * g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X - 1, g_b_Y - 1); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки вгору вправо по діагоналі
	if ((g_b_X - 1 >= 0) && (g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y + 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y + 1).flagged == false)) // вгору вправо по діагоналі
	{
		g_b_gameboard->get_fields(g_b_X - 1, g_b_Y + 1).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1) * g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X - 1) * g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X - 1, g_b_Y + 1); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки вниз вліво по діагоналі
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_Y - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y - 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y - 1).flagged == false)) // вниз вліво по діагоналі
	{
		g_b_gameboard->get_fields(g_b_X + 1, g_b_Y - 1).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1) * g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X + 1) * g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X + 1, g_b_Y - 1); // Встановлюємо значення сусідньої клітинки
	}

	// Перевірка клітинки вниз вправо по діагоналі
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y + 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y + 1).flagged == false)) // вниз вправо по діагоналі
	{
		g_b_gameboard->get_fields(g_b_X + 1, g_b_Y + 1).discovered = true; // Відкриваємо клітинку
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1) * g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL); // Змінюємо стан кнопки
		neighbour_value((g_b_X + 1) * g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X + 1, g_b_Y + 1); // Встановлюємо значення сусідньої клітинки
	}
}

// Функція для встановлення значення сусідньої клітинки
void neighbour_value(DWORD_PTR dwRefData, int g_b_X, int g_b_Y)
{
	switch (g_b_gameboard->get_fields(g_b_X, g_b_Y).value)
	{
	case 0: // Якщо значення клітинки 0 (немає сусідніх мін)
		SendMessage(GetDlgItem(hwnd, dwRefData), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[0]); // Встановлення зображення 0
		check_neighbours(g_b_X, g_b_Y); // Рекурсивна перевірка сусідніх клітинок
		break;
	case 1: // Якщо значення клітинки 1 (одна сусідня міна)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[1]); // Встановлення зображення 1
		break;
	case 2: // Якщо значення клітинки 2 (дві сусідні міни)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[2]); // Встановлення зображення 2
		break;
	case 3: // Якщо значення клітинки 3 (три сусідні міни)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[3]); // Встановлення зображення 3
		break;
	case 4: // Якщо значення клітинки 4 (чотири сусідні міни)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[4]); // Встановлення зображення 4
		break;
	case 5: // Якщо значення клітинки 5 (п'ять сусідніх мін)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[5]); // Встановлення зображення 5
		break;
	case 6: // Якщо значення клітинки 6 (шість сусідніх мін)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[6]); // Встановлення зображення 6
		break;
	case 7: // Якщо значення клітинки 7 (сім сусідніх мін)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[7]); // Встановлення зображення 7
		break;
	case 8: // Якщо значення клітинки 8 (вісім сусідніх мін)
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[8]); // Встановлення зображення 8
		break;
	default:
		break;
	}
}
// Функція для видалення кнопок зі старої ігрової дошки
void delete_buttons(int old_rows, int old_columns)
{
	for (int i = 0; i < old_rows; i++)
	{
		for (int j = 0; j < old_columns; j++)
		{
			DestroyWindow(hwnd_matrix[i][j]); // Знищення кнопки
		}
		delete[] hwnd_matrix[i]; // Видалення масиву кнопок для кожного рядка
	}
	delete[] hwnd_matrix; // Видалення масиву кнопок для всієї дошки
}

// Функція для очищення старого вікна та зміни його розмірів і позиції
void clear_old_window_change_its_pos_and_dim(int old_rows, int old_columns)
{
	int horizontal = 0;
	int vertical = 0;

	GetDesktopResolution(horizontal, vertical); // Отримання роздільної здатності екрану

	delete_buttons(old_rows, old_columns); // Видалення старих кнопок

	hwnd_matrix = get_hwnd_matrix(hwnd, (HINSTANCE)GetModuleHandle(NULL)); // Створення нової матриці хендлів кнопок

	MoveWindow(GetDlgItem(hwnd, -1), get_window_width() / 2 - 22, 5, 26, 26, TRUE); // Переміщення та зміна розміру кнопки

	// Встановлення нової позиції та розмірів вікна
	SetWindowPos(hwnd,
		HWND_TOP,
		horizontal_coordinates,
		vertical_coordinates,
		get_window_width(), get_window_height(),
		SWP_SHOWWINDOW);
}

// Функція для очищення кнопок і встановлення нормального обличчя (усмішки)
void unpressed_clear_button_normal_face()
{
	for (int i = 0; i < g_b_gameboard->get_rows() * g_b_gameboard->get_columns(); i++)
	{
		SendMessage(GetDlgItem(hwnd, i), BM_SETSTATE, FALSE, NULL); // Відміна натискання кнопки
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]); // Встановлення зображення стандартної кнопки
	}
	SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); // Встановлення зображення усмішки
}

// Функція для початку нової гри або зміни ігрового поля
void play_again_or_change_board(std::string again_or_board, int rows, int columns, int mines)
{
	high_score->change_Started_TIMER_false(); // Зупинка таймера
	high_score->reset_Timer(); // Скидання таймера
	KillTimer(hwnd, ID_TIMER); // Видалення таймера

	if (again_or_board.compare("again") == 0) // Якщо почати нову гру
	{
		InvalidateRect(hwnd, NULL, FALSE); // Оновлення вікна
		unpressed_clear_button_normal_face(); // Очищення кнопок і встановлення нормального обличчя
		g_b_gameboard = new game_board(g_b_gameboard->get_rows(), g_b_gameboard->get_columns(), g_b_gameboard->get_mines_number()); // Створення нової ігрової дошки
	}
	else if (again_or_board.compare("board") == 0) // Якщо змінити ігрове поле
	{
		RECT* help_RECT_struct = new RECT;

		help_RECT_struct->left = get_window_width() - 73;
		help_RECT_struct->right = get_window_width() - 17;
		help_RECT_struct->top = 8;
		help_RECT_struct->bottom = 28;

		int old_rows = 0;
		int old_columns = 0;
		old_rows = g_b_gameboard->get_rows(); // Збереження кількості старих рядків
		old_columns = g_b_gameboard->get_columns(); // Збереження кількості старих стовпців

		g_b_gameboard = new game_board(rows, columns, mines); // Створення нової ігрової дошки з новими параметрами

		InvalidateRect(hwnd, help_RECT_struct, TRUE); // Оновлення прямокутника
		InvalidateRect(hwnd, NULL, FALSE); // Оновлення вікна

		clear_old_window_change_its_pos_and_dim(old_rows, old_columns); // Очищення старого вікна та зміна його розмірів і позиції

		for (int i = 0; i < g_b_gameboard->get_rows() * g_b_gameboard->get_columns(); i++)
		{
			SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]); // Встановлення зображення стандартної кнопки
		}

		SendMessage(GetDlgItem(hwnd, 20001), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); // Встановлення зображення нормальної усмішки
	}
}

// Функція для перевірки на перемогу
void check_if_win()
{
	int undiscovered_cells = 0;

	for (int i = 0; i < g_b_gameboard->get_rows(); i++)
	{
		for (int j = 0; j < g_b_gameboard->get_columns(); j++)
		{
			// Якщо клітинка не є міною та ще не відкрита
			if (g_b_gameboard->get_fields(i, j).value != -1 && !g_b_gameboard->get_fields(i, j).discovered)
			{
				undiscovered_cells++; // Підрахунок невідкритих клітинок
			}
		}
	}

	// Якщо всі клітинки, які не є мінами, були відкриті, то гра виграна
	if (undiscovered_cells == 0)
	{
		KillTimer(hwnd, ID_TIMER); // Зупинка таймера
		g_b_gameboard->change_END_OF_GAME(); // Встановлення флагу закінчення гри

		// Оновлення смайлика на лице переможця
		SendMessage(GetDlgItem(hwnd, 20001), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[15]);

		// Помітити всі невідкриті міни прапорцями
		for (int i = 0; i < g_b_gameboard->get_rows(); i++)
		{
			for (int j = 0; j < g_b_gameboard->get_columns(); j++)
			{
				if (g_b_gameboard->get_fields(i, j).value == -1 && !g_b_gameboard->get_fields(i, j).discovered && !g_b_gameboard->get_fields(i, j).flagged)
				{
					SendMessage(GetDlgItem(hwnd, g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[17]); // Встановлення зображення прапорця
					g_b_gameboard->get_fields(i, j).flagged = true; // Встановлення флагу прапорця
				}
			}
		}

		g_b_gameboard->change_victory(); // Встановлення флагу перемоги
		save_HighScores(high_score->get_TIMER(), g_b_gameboard->get_rows(), g_b_gameboard->get_columns(), g_b_gameboard->get_mines_number()); // Збереження результатів гри
	}
}

// Функція для збереження рекордів
void save_HighScores(int your_time, int rows, int columns, int mines)
{
	// Відкриття діалогового вікна для введення імені гравця
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_NEW_SCORE),
		hwnd, reinterpret_cast<DLGPROC>(NewScoreProc));
	// Отримання імені гравця з діалогового вікна
	std::string player_name = high_score->get_PlayerName();
	// Запис результату у файл

	int records = high_score->get_record_count(); // Отримання кількості записів

	if (records >= 8) {
		high_score->reset_HighScores_overflow(); // Скидання рекордів, якщо їх кількість досягла максимуму
	}
	
	
	high_score->save_HighScores(player_name.c_str(), your_time, rows, columns, mines);
	// Показ діалогового вікна з рекордами
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_HIGHSCORES),
		hwnd, reinterpret_cast<DLGPROC>(HighScoresProc));
}
