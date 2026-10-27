/*
Program name: Assignment4
Program description: 
    a C++ program that solves sudoku puzzles 
    using an object oriented, recursive approach 
    with depth-first search and backtracking
Inputs: txt file containing an unsolved 9x9 sudoku puzzle (puzzle1.txt - puzzle5.txt)
Outputs: 
    the puzzle filename, the unsolved puzzle, 
    the solved puzzle if 1+ solutions exist,
    "no solution" if no solutions exist
Collaborators: 
    "What is meant with "const" at end of function declaration?" 
    (https://stackoverflow.com/questions/3141087/what-is-meant-with-const-at-end-of-function-declaration)
Other sources: Gemini
Author: Ana Gonzalez Yuil
Creation Date: 10.14.26
Revision Date: 10.14.26
Revisions: personal comments (indicated by ***), check for enough filled cells (at least 17)
*/

/**
 * @file Assignment4.cpp
 * @brief Object-based Sudoku Solver using Recursive Backtracking (DFS) in C++.
 * 
 * This program reads partially solved Sudoku puzzles from text files where blank spaces 
 * are represented by underscores ('_'). It uses a class-based approach, storing the 
 * grid in nested vectors (lists), and solves each puzzle using depth-first search 
 * with backtracking. It outputs the filename, the unsolved puzzle, all solutions 
 * found, and the total solution count.
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

/**
 * @class SudokuPuzzle
 * @brief Represents a 9x9 Sudoku puzzle board and provides methods to load, 
 *        validate, and solve the puzzle using recursive backtracking.
 */
class SudokuPuzzle {
private:
    // 2D vector (list of lists) to store the 9x9 grid characters ('1'-'9' or '_')
    std::vector<std::vector<char>> grid;

public:
    /**
     * @brief Construct a new Sudoku Puzzle object.
     * Initializes a 9x9 grid filled with placeholder underscores ('_').
     */
    SudokuPuzzle() {
        grid.resize(9, std::vector<char>(9, '_')); // *** changes size of grid after initialization
        // *** '9' sets the size of grid to 9. 
        // *** 'std::vector<char>(9, '_')' initializes every element of grid to be a vector of size 9 with every element set to '_'
    }

    /**
     * @brief Loads a Sudoku puzzle from a text file.
     * 
     * @param filename The path/name of the file to open.
     * @return true If the file was successfully opened and read.
     * @return false If the file could not be opened.
     */
    bool loadFromFile(const std::string& filename) {// *** takes input variable by reference (does not create a copy)
        int filledCells = 0; // *** holds amount of filled cells in unsolved sudoku grid
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Read 9 rows and 9 columns from the file
        for (int i = 0; i < 9; ++i) { // ** ++i works the same as i++
            for (int j = 0; j < 9; ++j) {
                file >> grid[i][j];

                // *** implement check for at least 17 filled squares
                if (grid[i][j] != '_') {filledCells++;} // *** if the cell is not empty, add to filledCells count.
            }
        }
        file.close();

        if (filledCells < 17) {return false;} //if filledcells is less than 17, return false. (this prevents empty input)

        return true;
    }

    /**
     * @brief Prints the current state of the puzzle grid to standard output.
     */
    void print() const { // *** const keyword after function declaration makes it so that class function cannot change the input (StackOverflow)
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                std::cout << grid[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    /**
     * @brief Validates if placing a specific character in a cell follows Sudoku rules.
     * 
     * Checks if the value already exists in the given row, column, or 3x3 subgrid.
     * 
     * @param row The row index (0-8) of the cell.
     * @param col The column index (0-8) of the cell.
     * @param val The character value ('1'-'9') to check.
     * @return true If the value is valid according to Sudoku rules.
     * @return false If the value violates row, column, or subgrid constraints.
     */
    bool isValid(int row, int col, char val) const {
        // 1. Check the row and column constraints
        for (int i = 0; i < 9; ++i) {
            // *** checks every num in row to see if value already exists
            if (grid[row][i] == val) return false; // Found in row
            // *** checks every num in col to see if value already exists
            if (grid[i][col] == val) return false; // Found in column
        }

        // 2. Check the 3x3 subgrid constraint
        // *** startRow and startCol calculation: 
        // *** this snaps the start of the loop to the constraints of the corresponding subgrid
        int startRow = row - (row % 3);
        int startCol = col - (col % 3);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (grid[startRow + i][startCol + j] == val) {
                    return false; // Found in the 3x3 subgrid
                }
            }
        }

        return val; // Valid placement
    }

    /**
     * @brief Iterates through the grid to find the next empty cell.
     * 
     * @param row Reference variable updated with the row index of the empty cell.
     * @param col Reference variable updated with the column index of the empty cell.
     * @return true If an empty cell ('_') is found.
     * @return false If the entire grid is filled (no empty cells remain).
     */
    bool findEmptyCell(int& row, int& col) const {
        for (row = 0; row < 9; ++row) {
            for (col = 0; col < 9; ++col) {
                if (grid[row][col] == '_') {
                    return true; // Empty cell located
                }
            }
        }
        return false; // Puzzle is completely filled
    }

    /**
     * @brief Recursive Depth-First Search function with backtracking to find all solutions.
     * 
     * Identifies an empty cell, iterates through candidate values ('1'-'9'), 
     * places valid candidates, and recurses to the next empty cell. Backtracks 
     * by resetting cells if a branch leads to a dead end.
     * 
     * @param solutionCount Reference counter tracking the number of valid solutions found.
     * @param solutions Reference container storing all valid solution grids.
     */
    void solveAll(int& solutionCount, std::vector<std::vector<std::vector<char>>>& solutions) {
        int row, col;

        // Base case: If no empty cell remains, a complete valid solution is achieved (k=81)
        if (!findEmptyCell(row, col)) {
            solutionCount++;
            solutions.push_back(grid);
            return;
        }

        // Try candidate values from '1' to '9' for the current empty cell
        for (char val = '1'; val <= '9'; ++val) {
            if (isValid(row, col, val)) {
                // Place candidate value into the cell
                grid[row][col] = val;

                // Move onto the next empty cell with a recursive function call
                solveAll(solutionCount, solutions);

                // Backtrack: Reset cell to empty if the recursive branch fails
                grid[row][col] = '_';
            }
        }
    }

    /**
     * @brief Helper function to process, solve, and print the results for a specific puzzle file.
     * 
     * @param filename The file name containing the puzzle.
     */
    static void processPuzzle(const std::string& filename) {
        std::cout << "========================================\n";
        std::cout << "Puzzle Filename: " << filename << "\n";
        std::cout << "========================================\n";

        SudokuPuzzle puzzle; 
        
        // Attempt to load the puzzle from the file
        if (!puzzle.loadFromFile(filename)) {
            std::cout << "Error: Could not open file " << filename << " or not enough squares filled.\n\n";
            return;
        }

        // Print the initial unsolved puzzle
        std::cout << "Unsolved Puzzle:\n";
        puzzle.print();
        std::cout << "\n";

        // Containers to store solutions and solution counts
        int solutionCount = 0;
        std::vector<std::vector<std::vector<char>>> solutions; //*** vector that holds each solved sudoku grid

        // Execute backtracking solver
        puzzle.solveAll(solutionCount, solutions);

        // Print number of solutions
        std::cout << "Number of Solutions: " << solutionCount << "\n\n";

        // Print puzzle solution(s) or "no solution found" message
        if (solutionCount == 0) {
            std::cout << "no solution found\n\n";
        } else {
            // *** if solutions exist, print each solution using the size of the solutions vector as limit
            // *** s indicates which solution is being printed
            for (size_t s = 0; s < solutions.size(); ++s) {
                std::cout << "Solution " << (s + 1) << ":\n";
                for (int i = 0; i < 9; ++i) {
                    for (int j = 0; j < 9; ++j) {
                        std::cout << solutions[s][i][j] << " ";
                    }
                    std::cout << "\n";
                }
                std::cout << "\n";
            }
        }
    }
};

/**
 * @brief Main execution function.
 * 
 * Iterates through the five required puzzle files and processes each one sequentially.
 */
int main() {
    // Array containing the names of the five target text files
    std::string puzzleFiles[] = {
        "puzzle1.txt", 
        "puzzle2.txt", 
        "puzzle3.txt", 
        "puzzle4.txt", 
        "puzzle5.txt"
    };

    // Process each puzzle file sequentially
    for (const std::string& filename : puzzleFiles) {
        SudokuPuzzle::processPuzzle(filename);
    }

    return 0;
}