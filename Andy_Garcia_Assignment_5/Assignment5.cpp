    /*
    Program name: Assignment5 (revision of Assignment4)
    Program description: 
        a C++ program that solves sudoku puzzles 
        using an object oriented, recursive approach 
        with depth-first search and backtracking
    Inputs: txt file containing an unsolved 9x9 sudoku puzzle (puzzle1.txt - puzzle5.txt)
    Outputs: 
        the puzzle filename, the unsolved puzzle, 
        the solved puzzle if 1+ solutions exist,
        "no solution" if no solutions exist
        solution1.txt - solution5.txt (all solutions then "Solutions found: n", or the line "No solution found.")
    Collaborators: 
        "What is meant with "const" at end of function declaration?" 
        (https://stackoverflow.com/questions/3141087/what-is-meant-with-const-at-end-of-function-declaration)
    Other sources: 
        Gemini (original Assignment 4 code)
        ChatGPT (assignmetnt 5 changes)
        Claude (assignment 5 improvements after changes)

    Author: Ana Gonzalez Yuil
    Creation Date: 10.14.26
    Revision Date: 10.14.26
    Revisions: personal comments (indicated by ***), check for enough filled cells (at least 17)

    Revised by: Andy Garcia 
    Revision Date: 10/28
    New revisions marked by REVISIONS: 

    */

    /**
     * @file Assignment5.cpp
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

    // Removed the <<cctype> include that ChatGPT added because it did not use it inside any change. (Andy Garcia)
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
                    // REVISION: Checks if the cell at the index can be read
                    if (!(file >> grid[i][j])) {
                        file.close(); // REVISION: Closes the file if the character at the index can't be read
                        return false; // REVISION: Rejects the file if it has less than 81 cell values
                    }

                    // REVISION: Checks and rejects anything that is not an _ or 1-9
                    if (grid[i][j] != '_' &&
                        (grid[i][j] < '1' || grid[i][j] > '9')) {
                        file.close(); // REVISION: Closes the file before it returns
                        return false; // REVISION: Returns false to prevent bad characters from being in the solver function
                    }

                    // *** implement check for at least 17 filled squares
                    if (grid[i][j] != '_') {
                        filledCells++; // *** if the cell is not empty, add to filledCells count.
                    }
                }
            }

            // file.close();  CODE CHANGE: Removed this line to allow for the next checks to happen (Andy Garcia)

            // REVISION: Reject extra values that exceed the 9x9 grid
            char extraCharacter;
            if (file >> extraCharacter) {
                return false;
            }

            // REVISION: Keep the original filled cell check
            if (filledCells < 17) {
                return false; // Prevents empty input
            }

            // REVISION: Check to see if the values violate the Sudoku rules
            // modified the Revision to improve maintainability moving it to a new function
            if (!givensAreConsistent()) {
                return false; // two given numbers will break Sudoku rules
            }
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

            return true; // REVISION: Returning true instead of val to make it permanent and improve readability 
        }


        /**
         * @brief Checks that the given numbers do not already break a Sudoku rule.
         *        This is ChatGPT's conflict check, moved out of loadFromFile into its own function.
         * @return true if no given number conflicts with another, false otherwise.
         */
        // Added givensAreConsistent() function. Code implemented by Andy Garcia suggested by Claude.
        bool givensAreConsistent() {
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (grid[i][j] == '_') continue;  // only filled cells need checking
                    char originalValue = grid[i][j];  // save the given before temporarily clearing it
                    grid[i][j] = '_';                 // exclude the given itself from the checks
                    bool valid = isValid(i, j, originalValue); // check it against its row, column, and box
                    grid[i][j] = originalValue;       // restore the given
                    if (!valid) return false;         // two givens conflict
                }
            }
            return true;
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

            // REVISION: Derive the output filename by replacing the "puzzle" prefix with "solution".
            // For example, "puzzle3.txt" becomes "solution3.txt".
            std::string outputFilename = "solution" + filename.substr(6);

            // REVISION: Open the output file so this puzzle's results are written separately.
            std::ofstream outputFile(outputFilename);

            // REVISION: Report an output-file error rather than silently losing the results.
            if (!outputFile.is_open()) {
                std::cout << "Error: Could not create output file " << outputFilename << ".\n\n";
                return;
            }

            SudokuPuzzle puzzle;

            // Attempt to load the puzzle from the file
            if (!puzzle.loadFromFile(filename)) {
                std::cout << "Error: Could not open file " << filename
                        << " or puzzle input is invalid.\n\n";

                // REVISION: Write the no solution message when the puzzle does not open
                outputFile << "No solution found.\n";
                outputFile.close(); // REVISION: Close file after the error occurs
                return;
            }

            // Print the initial unsolved puzzle
            std::cout << "Unsolved Puzzle:\n";
            puzzle.print();
            std::cout << "\n";

            // Containers to store solutions and solution counts
            int solutionCount = 0;
            std::vector<std::vector<std::vector<char>>> solutions; // *** vector that holds each solved sudoku grid

            // Execute backtracking solver
            puzzle.solveAll(solutionCount, solutions);

            // Print number of solutions
            std::cout << "Number of Solutions: " << solutionCount << "\n\n";

            // Print puzzle solution(s) or "no solution found" message
            if (solutionCount == 0) {
                std::cout << "no solution found\n\n";

                // REVISION: Only put the no solution found message on the output file and nothing else
                outputFile << "No solution found.\n";
            } else {
                // *** if solutions exist, print each solution using the size of the solutions vector as limit
                // *** s indicates which solution is being printed
                for (size_t s = 0; s < solutions.size(); ++s) {
                    std::cout << "Solution " << (s + 1) << ":\n";

                    // REVISION: Write the solution the same way as the input was given
                    for (int i = 0; i < 9; ++i) {
                        for (int j = 0; j < 9; ++j) {
                            std::cout << solutions[s][i][j] << " ";
                            outputFile << solutions[s][i][j] << " "; // REVISION: Save each cell to the file piece by piece
                        }
                        std::cout << "\n";
                        outputFile << "\n"; // REVISION: Keep one row per line on the output file
                    }

                    std::cout << "\n";
                    outputFile << "\n"; // REVISION: Seperate the sections with this blank line
                }

                // REVISION: Add the total solution count as the final line of the file
                outputFile << "Solutions found: " << solutionCount << "\n";
            }

            // REVISION: Close the output file after all results have been put on it
            outputFile.close();
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