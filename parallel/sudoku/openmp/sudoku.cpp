/*                    
 *  This file is part of Christian's OpenMP parallel Sudoku Solver
 *  
 *  Copyright (C) 2013 by Christian Terboven <christian@terboven.com>
 *                                                                       
 *  This program is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "SudokuBoard.h"

int found_sudokus = 0;
const int MAX_LEVEL = 2;  // Adjust this value based on experimentation

// Helper function to find an empty location on the board
bool findEmptyLocation(CSudokuBoard *board, int &row, int &col) {
    int field_size = board->getFieldSize();
    for (row = 0; row < field_size; row++) {
        for (col = 0; col < field_size; col++) {
            if (board->get(row, col) == 0)
                return true;
        }
    }
    return false;
}

// Helper function to check if a number can be placed in a given cell
bool isSafe(CSudokuBoard *board, int row, int col, int num) {
    int field_size = board->getFieldSize();
    int block_size = board->getBlockSize();

    // Check row and column
    for (int k = 0; k < field_size; k++) {
        if (board->get(row, k) == num || board->get(k, col) == num)
            return false;
    }

    // Check block
    int startRow = row - row % block_size;
    int startCol = col - col % block_size;
    for (int r = 0; r < block_size; r++) {
        for (int c = 0; c < block_size; c++) {
            if (board->get(startRow + r, startCol + c) == num)
                return false;
        }
    }

    return true;
}

// Recursive function to solve the Sudoku puzzle
void solveSudoku(CSudokuBoard *board, int level) {
    int row, col;
    if (!findEmptyLocation(board, row, col)) {
        // No empty location left; solution found
        #pragma omp critical
        {
            found_sudokus++;
            // std::cout << "Solution #" << found_sudokus << std::endl;
            // board->printBoard();
            // std::cout << std::endl;
        }
        return;
    }

    int field_size = board->getFieldSize();
    for (int num = 1; num <= field_size; num++) {
        if (isSafe(board, row, col, num)) {
            // Set the number on the board
            board->set(row, col, num);
            if (level < MAX_LEVEL) {
                // Create a new board as a copy
                CSudokuBoard *newBoard = new CSudokuBoard(*board);
                // Create a new task
                #pragma omp task firstprivate(newBoard, level)
                {
                    solveSudoku(newBoard, level + 1);
                    delete newBoard;
                }
                
            } else {
                // Continue recursion without creating a new task
                solveSudoku(board, level + 1);
                // Backtrack
                
            }
			// Reset the cell for the next iteration - backtracking
			board->set(row, col, 0);
        }
    }
    // Wait for all child tasks to complete at this level
    if (level < MAX_LEVEL) {
        #pragma omp taskwait
    }
}

int main(int argc, char* argv[]) {
    // Measure the time
    double t3, t4;

    // Expect three command line arguments: field size, block size, and input file
    if (argc != 4) {
        std::cout << "Usage: sudoku.exe <field size> <block size> <input filename>" << std::endl;
        std::cout << std::endl;
        return -1;
    }
    else {
        CSudokuBoard *sudoku1 = new CSudokuBoard(atoi(argv[1]), atoi(argv[2]));
        if (!sudoku1->loadFromFile(argv[3])) {
            std::cout << "There was an error reading a Sudoku template from " << argv[3] << std::endl;
            std::cout << std::endl;
            return -1;
        }

        // Print the Sudoku board template
        // std::cout << "Given Sudoku template" << std::endl;
        // sudoku1->printBoard();

        // Solve the Sudoku by finding (and printing) all solutions
        t3 = omp_get_wtime();

        #pragma omp parallel
        {
            #pragma omp single
            {
                solveSudoku(sudoku1, 0);
            }
        }

        t4 = omp_get_wtime();

        delete sudoku1;
    }

    // Print the time
    std::cout << "Parallel computation took " << t4 - t3 << " seconds ("
              << omp_get_max_threads() << " threads)." << std::endl << std::endl;

    return 0;
}






/*
		#pragma omp parallel num_threads(4)
		{
			#pragma omp master
			printf("Threads:      %d\n", omp_get_num_threads());
			#pragma omp single
			{
				printf("I am thread %d and I am creating tasks\n", omp_get_thread_num());
				p=head;
				while (p)
				{
					#pragma omp task firstprivate(p)
					{
						processwork(p);
						printf("I am thread %d\n", omp_get_thread_num());
					}
					p = p->next;
				}
			}
		}
*/