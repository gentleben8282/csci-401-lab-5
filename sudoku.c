/* CSCI 401: Lab #5: Programming using Threads
 * Programmer (Student ID): Ben Corriette (@02956064)
 * Last modified date: 11/22/2021
 * 
 * Summary: A program that uses parallel threads to solve sudoku puzzles.
 * 
 * References: Lab #3: Part 2: Processes and Shared Memory
 *             Quick Sort and Function Pointers Handout (Lab #4)
 *             Linux Programmer's Manual: pthread_create(), pthread_join(), perror()
 *             https://stackoverflow.com/questions/13956285/pthread-t-pointer-as-argument-of-pthread-create
 */ 

#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 9
#define BOX_SIZE 3

// Sudoku board. Modify this to test your checker!
// 0 == empty cell; 1-9 is the filled in digit.
int board[SIZE][SIZE] = {
    {1,2,3,4,8,9,5,3,6},
    {6,4,5,4,5,3,5,1,2},
    {7,8,9,4,1,5,8,6,7},
    {8,7,9,2,5,6,3,1,4},
    {7,5,3,6,8,9,5,3,1},
    {9,3,5,4,6,8,5,7,3},
    {4,6,7,5,2,3,7,5,1},
    {3,1,4,8,3,2,1,2,6},
    {5,9,2,3,4,7,4,4,8},
};

bool row_check[SIZE];
bool col_check[SIZE];
bool box_check[SIZE];

void print_board(int board[SIZE][SIZE]) {
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            printf("%5d", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Checks the given row for duplicate numbers, and updates the row_check
// value for that row appropriately. If no number is repeated in that row,
// row_check[row] will be set to true; otherwise, it will be false.
void* check_row(void* args) {
  int row_temp_check[SIZE];
  for(int i = 0; i < SIZE; i++) {
    row_check[i] = true;
    for(int j = 0; j < SIZE; j++) {
      row_temp_check[j] = board[i][j];
      for(int k = 0; k < j; k++) {
        if (row_temp_check[k] == board[i][j]) {
          row_check[i] = false;
          break;
        }
      }
      if (!row_check[i]) { break; }
    }
  }
}

// Checks the given col for duplicate numbers, and updates the col_check
// value for that col appropriately. If no number is repeated in that col,
// col_check[col] will be set to true; otherwise, it will be false.
void* check_col(void* args) {
  int col_temp_check[SIZE];
  for(int i = 0; i < SIZE; i++) {
    col_check[i] = true;
    for(int j = 0; j < SIZE; j++) {
      col_temp_check[j] = board[j][i];
      for(int k = 0; k < j; k++) {
        if (col_temp_check[k] == board[j][i]) {
          col_check[i] = false;
          break;
        }
      }
      if (!col_check[i]) { break; }
    }
  }
}

// Checks the given 3x3 box for duplicate numbers, and updates the box_check
// value for that box appropriately. If no number is repeated in that box,
// box_check[box] will be set to true; otherwise, it will be false.
void* check_box(void* args) {
  int box_temp_check[SIZE];
  int box_temp_check_index = 0;
  int row_start_index = 0;
  int column_start_index = 0;
  int row_end_index = 2;
  int column_end_index = 2;
  
  for(int h = 0; h < SIZE; h++) {
    box_temp_check_index = 0;
    box_check[h] = true;
    for(int i = row_start_index; i <= row_end_index; i++) {
      for(int j = column_start_index; j <= column_end_index; j++, box_temp_check_index++) {
        box_temp_check[box_temp_check_index] = board[i][j];
        for(int k = 0; k < box_temp_check_index; k++) {
          if (box_temp_check[k] == board[i][j]) {
            box_check[h] = false;
            break;
          }
        }
        if (!box_check[h]) { break; }
      }
      if (!box_check[h]) { break; }
    }
    if (column_end_index < SIZE - 1) {
      column_start_index = column_start_index + BOX_SIZE;
      column_end_index = column_end_index + BOX_SIZE;
    } else if (row_end_index < SIZE - 1) {
      row_start_index = row_start_index + BOX_SIZE;
      row_end_index = row_end_index + BOX_SIZE;
      column_start_index = 0;
      column_end_index = 2;
    }
    for(int i = 0; i < SIZE; i++) {
      box_temp_check[i] = 0;
    }
  }
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() { 
    // 1. Print the board.
    printf("Board:\n");
    print_board(board);
    
    // 2. Create pthread_t objects for our threads.
	  pthread_t parallel_thread_row;
		pthread_t parallel_thread_column;
		pthread_t parallel_thread_box;
    void* parallel_thread_row_exit_status_ptr;
    void* parallel_thread_column_exit_status_ptr;
    void* parallel_thread_box_exit_status_ptr;
    
    // 3. Create a thread for each cell of each matrix operation.
    if (pthread_create(&parallel_thread_row, NULL, &check_row, NULL) != 0) {
      perror("main(): A parallel thread creation error has occurred");
      if (pthread_cancel(parallel_thread_row) != 0) {
        perror("main(): A parallel thread cancellation error has occurred");
      }
    }
  
    if (pthread_create(&parallel_thread_column, NULL, &check_col, NULL) != 0) {
      perror("main(): A parallel thread creation error has occurred");
      if (pthread_cancel(parallel_thread_column) != 0) {
        perror("main(): A parallel thread cancellation error has occurred");
      }
    }
  
    if (pthread_create(&parallel_thread_box, NULL, &check_box, NULL) != 0) {
      perror("main(): A parallel thread creation error has occurred");
      if (pthread_cancel(parallel_thread_box) != 0) {
        perror("main(): A parallel thread cancellation error has occurred");
      }
    }
    
    // 4. Wait for all threads to finish.
    if (pthread_join(parallel_thread_row, &parallel_thread_row_exit_status_ptr) != 0) {
      perror("main(): A parallel thread termination error has occurred");
    }
  
    if (pthread_join(parallel_thread_column, &parallel_thread_column_exit_status_ptr) != 0) {
      perror("main(): A parallel thread termination error has occurred");
    }
  
    if (pthread_join(parallel_thread_box, &parallel_thread_box_exit_status_ptr) != 0) {
      perror("main(): A parallel thread termination error has occurred");
    }
    
    // 5. Print the results.
    printf("Results:\n");
    bool all_rows_passed = true;
    printf("Rows:\n");
    for (int i = 0; i < SIZE; i++) {
        if (!row_check[i]) {
            printf("Row %i did not pass\n", i);
            all_rows_passed = false;
        }
    }
    if (all_rows_passed) {
        printf("All rows passed!\n");
    }
    
    bool all_cols_passed = true;
    printf("Cols:\n");
    for (int i = 0; i < SIZE; i++) {
        if (!col_check[i]) {
            printf("Col %i did not pass\n", i);
            all_cols_passed = false;
        }
    }
    if (all_cols_passed) {
        printf("All cols passed!\n");
    }
    
    bool all_boxes_passed = true;
    printf("Boxes:\n");
    for (int i = 0; i < SIZE; i++) {
        if (!box_check[i]) {
            printf("Box %i did not pass\n", i);
            all_boxes_passed = false;
        }
    }
    if (all_boxes_passed) {
        printf("All boxes passed!\n");
    }
    return 0;
}

