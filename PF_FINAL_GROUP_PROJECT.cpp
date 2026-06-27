#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <Windows.h>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;

int bsize; // will be set to 8 or 10
char board[10][10]; // maximum size 10x10
int score = 0;

// ======== Board Initialization ========
void generateBoard() {
	char candies[7] = { '@', '#', '&', '$', '%', '!', '*' };
	for (int r = 0; r<bsize; r++) {
		for (int c = 0; c<bsize; c++) {
			while (true) {
				board[r][c] = candies[rand() % 7];
				bool bad = false;
				if (c >= 2 && board[r][c] == board[r][c - 1] && board[r][c] == board[r][c - 2]) bad = true;
				if (r >= 2 && board[r][c] == board[r - 1][c] && board[r][c] == board[r - 2][c]) bad = true;
				if (!bad) break;
			}
		}
	}
}

// ======== Display Board + Timer + Score ========
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void displayBoard(int remaining) {
	system("cls");

	// Timer
	cout << "Time Remaining: ";
	if (remaining / 60 < 10) cout << "0";
	cout << remaining / 60 << ":";
	if (remaining % 60 < 10) cout << "0";
	cout << remaining % 60 << endl;

	// Score
	cout << "Score: " << score << endl << endl;

	// Column headers
	cout << "   ";
	for (int c = 0; c < bsize; c++) cout << c << " ";
	cout << endl;

	// Board with colors
	for (int r = 0; r < bsize; r++) {
		cout << r << "  ";
		for (int c = 0; c < bsize; c++) {
			char ch = board[r][c];
			switch (ch) {
			case '@': setColor(4); break;
			case '#': setColor(2); break;
			case '&': setColor(3); break;
			case '$': setColor(6); break;
			case '%': setColor(5); break;
			case '!': setColor(1); break;
			case '*': setColor(14); break;
			}
			cout << ch << " ";
			setColor(7); // reset
		}
		cout << endl;
	}
}

// ======== Swap and Adjacent Functions ========
bool isAdjacent(int r1, int c1, int r2, int c2) {
	return (abs(r1 - r2) + abs(c1 - c2) == 1);
}

void swapCandies(int r1, int c1, int r2, int c2) {
	char temp = board[r1][c1];
	board[r1][c1] = board[r2][c2];
	board[r2][c2] = temp;
}

// ======== Helper ========
bool isValid(int r, int c) {
	return r >= 0 && r<bsize && c >= 0 && c<bsize;
}

// ======== Match Detection ========
bool findMatches(int &score) {
	bool matched = false;

	// Horizontal matches
	for (int r = 0; r<bsize; r++) {
		for (int c = 0; c<bsize - 2; c++) {
			if (board[r][c] == board[r][c + 1] && board[r][c] == board[r][c + 2]) {
				matched = true;
				int len = 3;
				while (c + len<bsize && board[r][c] == board[r][c + len]) len++;
				score += (len == 3 ? 10 : (len == 4 ? 15 : 20));
				for (int k = 0; k<len; k++) board[r][c + k] = ' ';
				c += len - 1;
			}
		}
	}

	// Vertical matches
	for (int c = 0; c<bsize; c++) {
		for (int r = 0; r<bsize - 2; r++) {
			if (board[r][c] == board[r + 1][c] && board[r][c] == board[r + 2][c]) {
				matched = true;
				int len = 3;
				while (r + len<bsize && board[r][c] == board[r + len][c]) len++;
				score += (len == 3 ? 10 : (len == 4 ? 15 : 20));
				for (int k = 0; k<len; k++) board[r + k][c] = ' ';
				r += len - 1;
			}
		}
	}

	// L-shape matches
	for (int r = 0; r<bsize - 2; r++) {
		for (int c = 0; c<bsize - 2; c++) {
			char ch = board[r][c];
			if (ch == ' ') continue;
			// vertical 3 + horizontal 2 right
			if (board[r + 1][c] == ch && board[r + 2][c] == ch && board[r + 2][c + 1] == ch && board[r + 2][c + 2] == ch) {
				matched = true;
				score += 25;
				board[r][c] = board[r + 1][c] = board[r + 2][c] = board[r + 2][c + 1] = board[r + 2][c + 2] = ' ';
			}
			// horizontal 3 + vertical 2 down
			if (board[r][c + 1] == ch && board[r][c + 2] == ch && board[r + 1][c + 2] == ch && board[r + 2][c + 2] == ch) {
				matched = true;
				score += 25;
				board[r][c + 1] = board[r][c + 2] = board[r + 1][c + 2] = board[r + 2][c + 2] = board[r][c] = ' ';
			}
		}
	}

	return matched;
}

// ======== Gravity + Refill ========
void applyGravity() {
	for (int c = 0; c<bsize; c++) {
		for (int r = bsize - 1; r >= 0; r--) {
			if (board[r][c] == ' ') {
				int k = r - 1;
				while (k >= 0 && board[k][c] == ' ') k--;
				if (k >= 0) {
					board[r][c] = board[k][c];
					board[k][c] = ' ';
				}
			}
		}
	}
}

void refillBoard() {
	char candies[7] = { '@', '#', '&', '$', '%', '!', '*' };
	for (int r = 0; r<bsize; r++) {
		for (int c = 0; c<bsize; c++) {
			if (board[r][c] == ' ') board[r][c] = candies[rand() % 7];
		}
	}
}

// ======== High Score System ========
void saveScore(string name, int score) {
	ofstream file("highestScore.txt", ios::app);
	if (file.is_open()) {
		file << name << " " << score << endl;
		file.close();
	}
}

void showHighScores() {
	ifstream file("highestScore.txt");
	string names[100];
	int scores[100];
	int count = 0;
	if (file.is_open()) {
		while (file >> names[count] >> scores[count]) count++;
		file.close();
	}

	// Bubble sort descending
	for (int i = 0; i<count - 1; i++) {
		for (int j = 0; j<count - i - 1; j++) {
			if (scores[j]<scores[j + 1]) {
				swap(scores[j], scores[j + 1]);
				swap(names[j], names[j + 1]);
			}
		}
	}

	cout << "\n--- TOP 10 HIGH SCORES ---\n";
	for (int i = 0; i<count && i<10; i++) {
		cout << i + 1 << ". " << names[i] << " : " << scores[i] << endl;
	}
}

// ======== Save / Load Game ========
void saveGame(int remaining) {
	ofstream file("savedGame.txt");
	if (file.is_open()) {
		file << score << " " << remaining << " " << bsize << endl;
		for (int r = 0; r<bsize; r++) {
			for (int c = 0; c<bsize; c++) file << board[r][c] << " ";
			file << endl;
		}
		file.close();
		cout << "Game saved!\n";
	}
}

bool loadGame(int &remaining) {
	ifstream file("savedGame.txt");
	if (!file.is_open()) {
		cout << "No saved game found.\n";
		return false;
	}
	file >> score >> remaining >> bsize;
	for (int r = 0; r<bsize; r++) {
		for (int c = 0; c<bsize; c++) file >> board[r][c];
	}
	file.close();
	cout << "Game loaded!\n";
	return true;
}

// ======== Player Move (with Pause) ========
bool playerMove(int &remaining) {
	int r, c;
	cout << "Select candy row: ";
	cin >> r;
	cout << "Select candy column: ";
	cin >> c;

	cout << "Press W/A/S/D to swap, P to pause: ";
	char move = _getch();

	if (move == 'p' || move == 'P') {
		saveGame(remaining);
		return false;
	}

	int r2 = r, c2 = c;
	if (move == 'w') r2--;
	else if (move == 's') r2++;
	else if (move == 'a') c2--;
	else if (move == 'd') c2++;
	else return false;

	if (!isValid(r2, c2) || !isAdjacent(r, c, r2, c2)) return false;

	swapCandies(r, c, r2, c2);

	if (!findMatches(score)) {
		swapCandies(r, c, r2, c2);
		return false;
	}
	return true;
}

// ======== Game Loop ========
void gameLoop(int remainingTime) {
	auto start = chrono::steady_clock::now();
	int timeLimit = remainingTime;

	while (true) {
		auto now = chrono::steady_clock::now();
		int elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
		int remaining = timeLimit - elapsed;

		if (remaining <= 0) {
			cout << "\nTime's up!\nFinal Score: " << score << endl;
			break;
		}

		displayBoard(remaining);

		if (!playerMove(remaining)) {
			Sleep(500); // pause or invalid move
		}

		while (findMatches(score)) {
			applyGravity();
			refillBoard();
			displayBoard(remaining);
			Sleep(300);
		}
	}

	// save score & show high scores
	string playerName;
	cout << "\nEnter your name: ";
	cin >> playerName;
	saveScore(playerName, score);
	showHighScores();
}

// ======== Main Menu ========
int main() {
	srand(time(0));
	int choice;
	while (true) {
		cout << "\n CANDY CRUSH CONSOLE GAME\n";
		cout << "1. New Game\n2. Load Game\n3. View High Scores\n4. Exit\n";
		cout << "Enter choice: ";
		cin >> choice;

		if (choice == 1) {
			score = 0;
			// Select mode
			int mode;
			cout << "Select Mode:\n1. Easy (8x8)\n2. Hard (10x10)\nChoice: ";
			cin >> mode;
			if (mode == 1) bsize = 8;
			else if (mode == 2) bsize = 10;
			else { cout << "Invalid mode\n"; continue; }

			generateBoard();
			gameLoop(40);
		}
		else if (choice == 2) {
			int remainingTime = 40;
			if (loadGame(remainingTime)) gameLoop(remainingTime);
		}
		else if (choice == 3) {
			showHighScores();
		}
		else if (choice == 4) {
			cout << "Exiting...\n";
			break;
		}
		else {
			cout << "Invalid choice, try again.\n";
		}
	}
	system("pause");
	return 0;
}
