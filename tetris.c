#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#define WIDTH 10
#define HEIGHT 20

void printBoard(int board[HEIGHT][WIDTH]) {

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if(j == 0 || j == WIDTH - 1) {
                printf("| ");
            } else if (i == HEIGHT - 1) {
                printf("- ");
            } else if (board[i][j] == 2 || board[i][j] == 1) {
                printf("# ");
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

void clearScreen() {
    system("cls");
}

bool checkOutOfBoundsLaterally(int piece[4][4], int *x, int *y) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(piece[i][j]) {
                if(*x + j < 0 || *x + j >= WIDTH) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool checkCollision(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j]) {
                if (board[*y + i][*x + j] == 2 || *y + i >= HEIGHT) {
                    return true;
                }
            }
        }
    }
    return false;
}

void clearPreviousPiece(int board[HEIGHT][WIDTH], int piece[4][4], int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j] == 1) {
                board[y + i][x + j] = 0;
            }
        }
    }
}

void renderCurrentPieceOnBoard(int board[HEIGHT][WIDTH], int piece[4][4], int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j] == 1) {
                board[y + i][x + j] = 1;
            } else if (board[y + i][x + j] != 1 && board[y + i][x + j] != 2) {
                board[y + i][x + j] = 0;
            } 
        }
    }
}

void rotatePieceOnBoard(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    clearPreviousPiece(board, piece, *x, *y);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j] == 1) {
                int temp = piece[i][j];
                piece[i][j] = piece[j][3 - i];
                piece[j][3 - i] = temp;
            }
        }
    }

    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void randomNumberGenerator(int *number) {
    srand(time(NULL));
    *number = (int) rand() % 7;
}

void pieceFall(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    clearPreviousPiece(board, piece, *x, *y);
    (*y)++;
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void pieceInstantFall(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    clearPreviousPiece(board, piece, *x, *y);
    while(!checkCollision(board, piece, x, y)) {
        (*y)++;
    }
    (*y)--;
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void movePieceLeft(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    if(checkOutOfBoundsLaterally(piece, x, y)) {
        return;
    }
    clearPreviousPiece(board, piece, *x, *y);
    (*x)--;
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void movePieceRight(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    if(checkOutOfBoundsLaterally(piece,  x, y)) {
        return;
    }
    clearPreviousPiece(board, piece, *x, *y);
    (*x)++;
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void placePieceOnBoard(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j]) {
                board[*y + i][*x + j] = 2;
            }
        }
    }
}

int main() {
    clearScreen();
    int board[HEIGHT][WIDTH] = {0};
    printBoard(board);

    int pieces[7][4][4] = {
        {
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }, {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }, {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }, {
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }, {
        {1, 1, 1, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }, {
        {1, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }, {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }
    };

    struct piece {
        int shape[4][4];
        int x;
        int y;
    };

    bool newPiece = 1;


    int currentPiece = 0;
    randomNumberGenerator(&currentPiece);
    struct piece current = {0}; 
    memcpy(current.shape, pieces[currentPiece], sizeof(current.shape));
    current.x = 3;
    current.y = 0;
    renderCurrentPieceOnBoard(board, current.shape, current.x, current.y);
    newPiece = 0;

    //Game loop
    while(1) {
        
        if(newPiece) {
         currentPiece = 0;
         randomNumberGenerator(&currentPiece);
         memset(&current, 0, sizeof(current)); 
         memcpy(current.shape, pieces[currentPiece], sizeof(current.shape));
         current.x = 3;
         current.y = 0;
         renderCurrentPieceOnBoard(board, current.shape, current.x, current.y);
         newPiece = 0;
        }

        if(_kbhit()) {
            char key = _getch();
            if(key == 27) { //esc
                break;
            } else if(key == 72) { //up
                rotatePieceOnBoard(board, current.shape, &current.x, &current.y);
            } else if(key == 80) { //down
                pieceFall(board, current.shape, &current.x, &current.y);
            } else if(key == 77) { //right  
                movePieceRight(board, current.shape, &current.x, &current.y);
            } else if(key == 75) { //left
                movePieceLeft(board, current.shape, &current.x, &current.y);
            } else if(key == 32) { //space
                pieceInstantFall(board, current.shape, &current.x, &current.y);
            }
        }

        if(checkCollision(board, current.shape, &current.x, &current.y)) {
            placePieceOnBoard(board, current.shape, &current.x, &current.y);
            newPiece = 1;
        }

        pieceFall(board, current.shape, &current.x, &current.y);
        Sleep(500);
        clearScreen();

        printBoard(board);
    }

    return 0;
}