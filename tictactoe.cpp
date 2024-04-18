/**
 * @file tictactoe.cpp
 * @author Miles Rasmussen
 * 
 * @brief A game of tic tac toe that can be played through the terminal
 * At the end of the game, it will prompt user to play again or quit
 * User specifies where to place on their turn by giving a column (1,2,3)
 * and a row (A, B, C). So, the topleft is (A1), middle is (B2) and so on
 * 
 * @date 4/15/24 started project, created display, input checking, piece 
 * and position nums to work with the board array easier, and all win 
 * conditions and game logic.
 * 
 * @date 4/16/24 Added highlighting to winning sequences, and added a 
 * pause screen between games that simply waits for input. Using chatGPT, 
 * some functions, such as checking for win conditions, were refactored for 
 * more simplicity. Finally, (again using chatGPT) all system() functions were
 * removed in favor of ASCII escape sequences for more efficiency. The print 
 * board function had to be refactored as well, adding a helper function, to do
 * all highlighting only through ASCII escape sequences. Also expaned piece enum
 * to include OW and XW to flag pieces to highlight in a win sequence
 */

#include <iostream>
#include <string>

/* ENUMS FOR GLOBAL SCOPE */
// enum for pos 0   1   2   3   4   5   6   7   8
enum position {a1, a2, a3, b1, b2, b3, c1, c2, c3};

// enum within array to track piece. 0 is blank
// OW and XW are winning sequences to highlight colors later
enum piece {B, O, X, OW, XW};

/* FUNCTION PROTOTYPES */

/// @brief print out the state of the board.
/// clears everything else in the terminal before it
/// @param board Array of pieces used to display what
/// tile a piece is in.on the board
void print_board(const piece board[]);

/// @brief  Takes a piece and returns a string representation of it
/// if it is part of a winning sequence, it is colored green with ASCII escape
/// @param p the piece being checked
/// @return a colored string of the piece being displayed
std::string getColorChar(piece p);

/// @brief prompt the user  what tile to place a piece on
/// Also allows them to quit the program or restart the round
/// @param board Array of pieces to populate board
/// @param x_turn Bool to track which player's turn it is
/// @return 0 if round played, 1 if game reset, 2 if game quit
/// or 3 if someone wins
int prompt_input(piece board[], const bool x_turn);

/// @brief check the board to see if there are any piece
/// that are three in a row. 
/// @param board Array of pieces to check the state of. 
/// will change X and O to XW and OW if they are 3-in-a-row
/// @param x_turn Bool to track if x's turn (true) or O's turn (false)
/// @return Bool. True if winner found, else false
bool check_winner(piece board[], const bool x_turn);

/// @brief display the end game state, and if a player won
/// the display stays up until the player prompts to continue or quit
/// @param board Array of pieces
/// @param winner If someone won or if tie game
/// @param x_turn What player's turn it was last
/// @return true if playing another game, false if game over
bool end_game_prompt(const piece board[], const bool winner, const bool x_turn);

/* MAIN FUNCTION */
int main() {
    bool game_active = true;

    while (game_active) {
        // setup vars for each game
        // board values
        piece board[9] = {B, B, B, B, B, B, B, B, B};
        bool x_turn = false;        // tracks if it is Xs turn
        bool round_active = true;  // flag to restart game
        bool winner = false;       // flag if winner found

        for (int i = 0; i < 9 && round_active; ++i)
        {
           // get user input
            switch (prompt_input(board, x_turn)) {
                case 0:  // if 0, then round normal change turns
                    break;
                case 1:  // if 1, reset this current round
                    round_active = false;
                    break;
                case 2:  // if 2, end program
                    game_active = false;
                    round_active = false;
                    break;
                case 3: // winner
                    round_active = false;
                    winner = true;
                    break;
                default: break;  // compiler be mad
            }

            // display end game prompt if game over, else switch turns
            if (winner || i == 8) {
                game_active = end_game_prompt(board, winner, x_turn);
            } else {
                x_turn = !x_turn;  // flip the turn
            }
        }
    }

    // clear terminal for last time, with ASCII escape sequence
    std::cout << "\033[2J\033[1;1H";

    return 0;
}

/* FUNCTION DEFINITIONS */
void print_board(const piece board[]) {

    // Display the Tic-Tac-Toe board
    std::cout << "\n    1   2   3\n";
    std::cout << "  +-----------\n";
    std::cout << "A | " << getColorChar(board[a1]) << " | " << getColorChar(board[a2]) << " | " << getColorChar(board[a3]) << "\n";
    std::cout << "  | ---+---+---\n";
    std::cout << "B | " << getColorChar(board[b1]) << " | " << getColorChar(board[b2]) << " | " << getColorChar(board[b3]) << "\n";
    std::cout << "  | ---+---+---\n";
    std::cout << "C | " << getColorChar(board[c1]) << " | " << getColorChar(board[c2]) << " | " << getColorChar(board[c3]) << "\n";
    std::cout << "  +-----------\n\n";
}

// Helper function to get colorized character representation for piece
std::string getColorChar(piece p) {
    switch (p) {
        case O:
            return "O";
        case X:
            return "X";
        case B:
            return " ";  // Empty space
        case OW:
            return "\033[32mO\033[0m";  // Green color for winning O
        case XW:
            return "\033[32mX\033[0m";  // Green color for winning X
        default:
            return " ";  // Handle unknown piece (should not occur)
    }
}

int prompt_input(piece board[], const bool x_turn) {
    int exit_status{0};  // 0 if round normal, 1 if reset, 2 if quit, 3 if win
    bool good_response = false;  // actual loop condition flag
    bool bad_response  = false;  // flag for error message general
    bool already_taken = false;  // flag for error message invalid pos

    piece players_piece = (x_turn)?X:O;  // what piece to insert

    position pos;  // where to insert piece

    while (!good_response) {

        // display any error message
            std::cout << "\033[2J\033[1;1H";  // Clear screen and move cursor to top-left corner     // clear terminal
        if (bad_response) {
            std::cout << "ERROR! Bad response given." << std::endl;
            bad_response = false;  // reset flag
        } else if (already_taken) {
            std::cout << "ERROR! That position is already filled." << std::endl;
            already_taken = false;  // reset flag
        }

        // display board
        print_board(board);  // display board

        // Display prompt
        std::cout <<  ((x_turn)?'X':'O') << "'s turn!\n";
        std::cout << "Enter Row,Col you wish to place a piece (ex, A1 or C3)\n";
        std::cout << "Or enter 'q' to quit or 'r' to restart this round\n";
        std::cout << "note: be careful of extra whitespace flagging invalid answer\n";
        std::cout << ">: " << std::flush;

        // read in response
        std::string response;
        std::getline(std::cin, response);

        // determine what to do for each response
        // topleft
        if (response == "A1" || response == "a1") {
            if (board[a1] == B) {
                pos = a1;
                good_response = true;
            } else { already_taken = true; }
        }
        // top middle
        else if (response == "A2" || response == "a2") {
            if (board[a2] == B) {
                pos = a2;
                good_response = true;
            } else {already_taken = true;}
        }
        // top right
        else if (response == "A3" || response == "a3") {
            if (board[a3] == B) {
                pos = a3;
                good_response = true;
            } else {already_taken = true;}
        }
        // middle left
        else if (response == "B1" || response == "b1") {
            if (board[b1] == B) {
                pos = b1;
                good_response = true;
            } else {already_taken = true;}
        }
       // middle middle
        else if (response == "B2" || response == "b2") {
            if (board[b2] == B) {
                pos = b2;
                good_response = true;
            } else {already_taken = true;}
        }
        // middle right
        else if (response == "B3" || response == "b3") {
            if (board[b3] == B) {
                pos = b3;
                good_response = true;
            } else {already_taken = true;}
        }
        // bottom left
        else if (response == "C1" || response == "c1") {
            if (board[c1] == B) {
                pos = c1;
                good_response = true;
            } else {already_taken = true;}
        }
        // bottom middle
        else if (response == "C2" || response == "c2") {
            if (board[c2] == B) {
                pos = c2;
                good_response = true;
            } else {already_taken = true;}
        }
        // bottom right
        else if (response == "C3" || response == "c3") {
            if (board[c3] == B) {
                pos = c3;
                good_response = true;
            } else {already_taken = true;}
        }
        // quit game
        else if (response == "q" || response == "Q") {
            exit_status = 2;
            good_response = true;
        }
        // restart game
        else if (response == "r" || response == "R") {
            exit_status = 1;
            good_response = true;
        }
        // default "case"
        else {
            bad_response = true;
        }
    }
    
    // if placing a piece
    if (exit_status == 0) {

        // add it to the board
        board[static_cast<int>(pos)] = players_piece;

        // check if someone one this round
        if (check_winner(board, x_turn)) {
            exit_status = 3;  // set exit status for win
        }
    }

    return exit_status;
}

bool check_winner(piece board[], const bool x_turn) {
    piece cp = ((x_turn)?X:O);    // track CurrentPiece
    piece wp = ((x_turn)?XW:OW);  // piece to replace with for green highlight
    bool win = false;             // flag if win found

    // Define winning combinations as arrays of positions
    const position win_combinations[8][3] = {
        {a1, a2, a3},  // top row
        {b1, b2, b3},  // middle row
        {c1, c2, c3},  // bottom row
        {a1, b1, c1},  // left column
        {a2, b2, c2},  // middle column
        {a3, b3, c3},  // right column
        {a1, b2, c3},  // diagonal (top-left to bottom-right)
        {a3, b2, c1}   // diagonal (top-right to bottom-left)
    };

    // Check each winning combination
    for (size_t i = 0; i < 8 && !win; ++i) {
        // pointer to sub-array in win_conditions
        const position* combo = win_combinations[i];
        if (board[combo[0]] == cp &&
            board[combo[1]] == cp &&
            board[combo[2]] == cp) {
            // Mark winning positions for highlighting
            board[combo[0]] = wp;
            board[combo[1]] = wp;
            board[combo[2]] = wp;
            win = true;  // Found a winner
        }
    }

    return win;  // true if winner, false if no winner
}

bool end_game_prompt(const piece board[], const bool winner, const bool x_turn) {
    bool still_playing = true;  // flag to track if game still active

        std::cout << "\033[2J\033[1;1H";  // Clear screen and move cursor to top-left corner

    // display heading
    if (winner) {
        std::cout << "    " << (x_turn?'X':'O') 
        << " WINS!!!" << std::endl;
    } else {
        std::cout << "    " << " TIE GAME!" << std::endl;
    }
                    
    print_board(board);

    // display and read in message
    std::string response;
    std::cout << "Enter 'q' to quit, or anything else to continue: ";
    std::getline(std::cin, response);

    if (response == "q" || response == "Q") {still_playing = false;}

    return still_playing;
}

/* EOF */
