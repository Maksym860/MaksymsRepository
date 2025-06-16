#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <chrono>
#include <limits> 

using namespace std;
using namespace std::chrono;

const int MAX_SIZE = 20;

char board[MAX_SIZE][MAX_SIZE];        // Справжнє ігрове поле
char display[MAX_SIZE][MAX_SIZE];      // Відображення для гравця
bool flagged[MAX_SIZE][MAX_SIZE];      // Прапорці
int rows, cols, mines; 
bool gameOver = false;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void initializeBoard() {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            board[i][j] = '.';
            display[i][j] = '#';
            flagged[i][j] = false;
        }
}

void placeMines() {
    int placed = 0;
    while (placed < mines) {
        int x = rand() % rows;
        int y = rand() % cols;
        if (board[x][y] != 'M') {
            board[x][y] = 'M';
            placed++;
        }
    }
}

int countAdjacentMines(int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && board[nx][ny] == 'M')
                count++;
        }
    return count;
}

void calculateNumbers() {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (board[i][j] != 'M')
                board[i][j] = '0' + countAdjacentMines(i, j);
}

void printBoard() {
    cout << "\n    ";
    for (int j = 0; j < cols; ++j)
        cout << setw(3) << j;
    cout << endl;

    // Верхня рамка
    cout << "   ┌";
    for (int j = 0; j < cols - 1; ++j)
        cout << "───┬";
    cout << "───┐\n";

    for (int i = 0; i < rows; ++i) {
        cout << setw(2) << i << " │";
        for (int j = 0; j < cols; ++j) {
            if (flagged[i][j]) {
                setColor(14); // Жовтий
                cout << " F ";
            } else if (display[i][j] != '#' || (gameOver && board[i][j] == 'M')) {
                if (board[i][j] == 'M') {
                    setColor(12); // Червоний
                    cout << " M ";
                } else {
                    setColor(10); // Зелений
                    cout << " " << display[i][j] << " ";
                }
            } else {
                setColor(7);
                cout << " # ";
            }
            setColor(7);
            cout << "│";
        }
        cout << endl;
        if (i < rows - 1) {
            cout << "   ├";
            for (int j = 0; j < cols - 1; ++j)
                cout << "───┼";
            cout << "───┤\n";
        }
    }

    // Нижня рамка
    cout << "   └";
    for (int j = 0; j < cols - 1; ++j)
        cout << "───┴";
    cout << "───┘\n";

    cout << "\n Правила гри:\n";
    cout << "   - Відкривайте клітинки, уникаючи мін.\n";
    cout << "   - Цифри вказують кількість мін навколо.\n";
    cout << "   - Ставте прапорці на потенційні міни (F).\n";
    cout << "   - Щоб виграти, відкрий усі безпечні клітинки.\n";
}

void revealCell(int x, int y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols || display[x][y] != '#' || flagged[x][y]) return;

    display[x][y] = board[x][y];
    if (board[x][y] == '0') {
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                revealCell(x + dx, y + dy);
    }
}

bool checkWin() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board[i][j] != 'M' && display[i][j] == '#') {
                return false;
            }
            if (board[i][j] == 'M' && !flagged[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void revealAllMines() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board[i][j] == 'M') {
                display[i][j] = 'M';
            }
        }
    }
}

int main() {
    srand(time(0));
    cout << "Введіть розмір поля (рядки і колонки): ";
    cin >> rows >> cols;
    cout << "Введіть кількість мін: ";
    cin >> mines;

    if (rows > MAX_SIZE || cols > MAX_SIZE || mines >= rows * cols) {
        cout << "Некоректні параметри!\n";
        // Пауза перед виходом
        cout << "Натисніть Enter для виходу...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return 1;
    }

    initializeBoard();
    placeMines();
    calculateNumbers();

    auto start = steady_clock::now();

    while (true) {
        printBoard();
        
        if (checkWin()) {
            printBoard();
            cout << "Вітаємо! Ви виграли гру!\n";
            
            auto end = steady_clock::now();
            auto duration = duration_cast<seconds>(end - start);
            cout << "Тривалість гри: " << duration.count() << " секунд.\n";
            
            //Пауза перед виходом, після перемоги
            cout << "Натисніть Enter для виходу...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            break;
        }
        
        cout << "\nВиберіть дію:\n1 - відкрити клітинку\n2 - поставити/зняти прапорець\nВаш вибір: ";
        int choice, x, y;
        cin >> choice;

        cout << "Введіть координати (рядок колонка): ";
        cin >> x >> y;

        if (x < 0 || x >= rows || y < 0 || y >= cols) {
            cout << "Некоректні координати!\n";
            continue;
        }

        if (choice == 1) {
            if (flagged[x][y]) {
                cout << "Спочатку зніміть прапорець.\n";
                continue;
            }
            if (board[x][y] == 'M') {
                gameOver = true;
                revealAllMines();
                printBoard();
                cout << "Ви натрапили на міну! Гра закінчена.\n";

                auto end = steady_clock::now();
                auto duration = duration_cast<seconds>(end - start);
                cout << "Тривалість гри: " << duration.count() << " секунд.\n";
                
                //Пауза перед виходом, після поразки
                cout << "Натисніть Enter для виходу...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
            }
            revealCell(x, y);
        } else if (choice == 2) {
            flagged[x][y] = !flagged[x][y];
        } else {
            cout << "Невірний вибір.\n";
        }
    }

    return 0;
}