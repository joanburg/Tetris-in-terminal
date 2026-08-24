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

#define WIDTH 11
#define HEIGHT 20
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

bool checkOutOfBoundsLaterally(int piece[4][4], int x, int y) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(piece[i][j]) {
                if(x + j < 1|| x + j >= WIDTH - 1) {
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
                int nextY = *y + i + 1;
                if (nextY >= HEIGHT - 1) {
                    return true;
                }
                if (board[nextY][*x + j] == 2) {
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
                pthread_mutex_lock(&mutex);
                board[y + i][x + j] = 0;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

void renderCurrentPieceOnBoard(int board[HEIGHT][WIDTH], int piece[4][4], int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j] == 1) {
                pthread_mutex_lock(&mutex);
                board[y + i][x + j] = 1;
                pthread_mutex_unlock(&mutex);
            } else if (board[y + i][x + j] != 1 && board[y + i][x + j] != 2) {
                pthread_mutex_lock(&mutex);
                board[y + i][x + j] = 0;
                pthread_mutex_unlock(&mutex);
            } 
        }
    }
}

void rotatePieceOnBoardClockwise(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {

    int rotatedPiece[4][4] = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j] == 1) {
                rotatedPiece[j][3 - i] = 1;
            }
        }
    }

    if(checkOutOfBoundsLaterally(rotatedPiece, *x, *y)) {
        return;
    }

    clearPreviousPiece(board, piece, *x, *y);
    memcpy(piece, rotatedPiece, sizeof(rotatedPiece));

    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void randomNumberGenerator(int *number) {
    srand(time(NULL));
    *number = (int) rand() % 7;
}

char pieceIdGenerator(int pieceType) {
    char pieceIds[7] = {'I', 'Z', 'S', 'T', 'L', 'J', 'O'};
    return pieceIds[pieceType];
}

void pieceFall(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    clearPreviousPiece(board, piece, *x, *y);
    pthread_mutex_lock(&mutex);
    (*y)++;
    pthread_mutex_unlock(&mutex);
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void pieceInstantFall(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    clearPreviousPiece(board, piece, *x, *y);
    while(!checkCollision(board, piece, x, y)) {
        (*y)++;
    }
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void movePieceLeft(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    if(checkOutOfBoundsLaterally(piece, *x - 1, *y)) {
        return;
    }
    clearPreviousPiece(board, piece, *x, *y);
    (*x)--;
    renderCurrentPieceOnBoard(board, piece, *x, *y);
}

void movePieceRight(int board[HEIGHT][WIDTH], int piece[4][4], int *x, int *y) {
    if(checkOutOfBoundsLaterally(piece, *x + 1, *y)) {
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

void *fall(void *args) {
    struct arguments {
        int (*board)[WIDTH];
        int (*piece)[4];
        int *x;
        int *y;
    };
    struct arguments arg = *(struct arguments *)args;
    while(1) {
        Sleep(10000);
        if(checkCollision(arg.board, arg.piece, arg.x, arg.y)) {
            break;   
        }
        pieceFall(arg.board, arg.piece, arg.x, arg.y);
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
        char id;
    };

    bool newPiece = 1;

    pthread_t fallThread;
    bool fallThreadRunning = false;

    int currentPiece = 0;
    randomNumberGenerator(&currentPiece);
    struct piece current = {0}; 
    memcpy(current.shape, pieces[currentPiece], sizeof(current.shape));
    current.x = 3;
    current.y = 0;
    current.id = pieceIdGenerator(currentPiece);
    renderCurrentPieceOnBoard(board, current.shape, current.x, current.y);
    newPiece = 0;

    //Game loop
    while(1) {
        
        if(newPiece) {
        
        if(fallThreadRunning) {
            pthread_cancel(fallThread);
            fallThreadRunning = false;
        }
         currentPiece = 0;
         randomNumberGenerator(&currentPiece);
         memset(&current, 0, sizeof(current)); 
         memcpy(current.shape, pieces[currentPiece], sizeof(current.shape));
         current.x = 3;
         current.y = 0;
         current.id = pieceIdGenerator(currentPiece);
         renderCurrentPieceOnBoard(board, current.shape, current.x, current.y);
         newPiece = 0;

         struct arguments {
             int (*board)[WIDTH];
             int (*piece)[4];
             int *x;
             int *y;
         };

         struct arguments *args = (struct arguments *)malloc(sizeof(struct arguments));
         args->board = board;
         args->piece = current.shape;
         args->x = &current.x;
         args->y = &current.y;
         pthread_create(&fallThread, NULL, fall, args);
         fallThreadRunning = true;
        }

        if(_kbhit()) {
            char key = _getch();
            if(key == 27) { //esc
                break;
            } else if(key == 72) { //up
                rotatePieceOnBoardClockwise(board, current.shape, &current.x, &current.y);
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

        Sleep(100);
        clearScreen();
        printBoard(board);
    }

    return 0;
}