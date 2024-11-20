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
const int MAX_DEPTH_FOR_TASKS = 2; // Adjust based on performance testing

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
bool check_valid(const CSudokuBoard *sudoku, int x, int y, int value) {
    int field_size = sudoku->getFieldSize();
    int block_size = sudoku->getBlockSize();
    int block_x = x / block_size;
    int block_y = y / block_size;

    // Check row and column
    for (int i = 0; i < field_size; i++) {
        if (sudoku->get(x, i) == value || sudoku->get(i, y) == value) {
            return false;
        }
    }

    // Check block
    for (int i = 0; i < block_size; i++) {
        for (int j = 0; j < block_size; j++) {
            if (sudoku->get(block_x * block_size + i, block_y * block_size + j) == value) {
                return false;
            }
        }
    }

    return true;
}

// Recursive function to solve the Sudoku puzzle
void solve_sudoku(CSudokuBoard *sudoku, int depth) {
    // int row = -1, col = -1;
    // bool is_done = true;
    int field_size = sudoku->getFieldSize();

    // // Find an empty cell
    // for (int i = 0; i < field_size; i++) {
    //     for (int j = 0; j < field_size; j++) {
    //         if (sudoku->get(i, j) == 0) {
    //             row = i;
    //             col = j;
    //             is_done = false;
    //             break;
    //         }
    //     }
    //     if (!is_done) {
    //         break;
    //     }
    // }

    int row, col;
    if (!findEmptyLocation(sudoku, row, col)) {
        // No empty location left; solution found
        #pragma omp critical
        {
            found_sudokus++;
			sudoku->incrementSolutionCounter();
            // std::cout << "Solution #" << found_sudokus << std::endl;
            // sudoku->printBoard();
            // std::cout << std::endl;
        }
        return;
    }

    // If no empty cell is found, we have a solution
    // if (is_done) {
    //     #pragma omp critical
    //     {
    //         found_sudokus++;
    //         sudoku->incrementSolutionCounter();
    //         // std::cout << "Solution " << found_sudokus << std::endl;
    //         // sudoku->printBoard();
    //     }
    //     return;
    // }

    // Try all possible values for the empty cell
    for (int value = 1; value <= field_size; value++) {
        if (check_valid(sudoku, row, col, value)) {
			#pragma omp task firstprivate(row, col, value, sudoku) final (depth > 2)
			{
				CSudokuBoard *new_sudoku = new CSudokuBoard(*sudoku);
            	new_sudoku->set(row, col, value);
				solve_sudoku(new_sudoku, depth + 1);
				delete new_sudoku;
			}

        }
    }

    // Wait for all tasks created at this level to complete
    #pragma omp taskwait
}

int main(int argc, char *argv[]) {
    double t3, t4;

    // Expect three command line arguments: field size, block size, and input file
    if (argc != 4) {
        std::cout << "Usage: sudoku.exe <field size> <block size> <input filename>" << std::endl;
        return -1;
    }

    // Create a Sudoku board and load the puzzle from the file
    int field_size = atoi(argv[1]);
    int block_size = atoi(argv[2]);
    CSudokuBoard *sudoku = new CSudokuBoard(field_size, block_size);

    if (!sudoku->loadFromFile(argv[3])) {
        std::cout << "Error reading Sudoku template from " << argv[3] << std::endl;
        return -1;
    }

    // Print the given Sudoku board template
    // std::cout << "Given Sudoku template" << std::endl;
    // sudoku->printBoard();

    // Solve the Sudoku by finding all solutions
    t3 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
            solve_sudoku(sudoku, 0);
        }
    }
    t4 = omp_get_wtime();

	delete sudoku;

    // Print the time taken
    std::cout << "Parallel computation took " << t4 - t3 << " seconds ("
              << omp_get_max_threads() << " threads)." << std::endl;

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