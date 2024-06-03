#pragma once
#include <string>
#include "windows.h"

struct score
{
	std::string name;
	int time;
	int rows;
	int cols;
	int mines;
};

class HighScore
{
private:
	bool Started_TIMER = false;
	unsigned int TIMER = 0;
	score HighScores[9];
	TCHAR PlayerName[17];
	int recordCount = 0;
public:
	HighScore();
	~HighScore();

	void reset_HighScores();
	void reset_HighScores_overflow();
	void load_HighScores();
	void save_HighScores(LPCSTR player_name, int your_time, int rows, int cols, int mines);
	bool get_Started_TIMER() { return Started_TIMER; }
	void change_Started_TIMER_true() { Started_TIMER = true; }
	void change_Started_TIMER_false() { Started_TIMER = false; }
	unsigned int get_TIMER() { return TIMER; }
	void add_one_to_TIMER() { TIMER++; }
	void reset_Timer() { TIMER = 0; }
	LPCTSTR get_i_HighScores_name(int i) { return HighScores[i].name.c_str(); }
	int get_i_HighScores_rows(int i) { return HighScores[i].rows; }
	int get_i_HighScores_cols(int i) { return HighScores[i].cols; }
	int get_i_HighScores_mines(int i) { return HighScores[i].mines; }
	int get_i_HighScores_time(int i) { return HighScores[i].time; }
	TCHAR* get_PlayerName() { return PlayerName; }
	int get_record_count() { return recordCount; }
};

