#include <iostream>
#include <string>
#include <vector>
#include<cstdlib>

using namespace std;

//==================================
//=========Main Game Class==========
//==================================

enum Game_States {
    in_progress,
    player_won,
    computer_won,
    draw
};

class Game {
    public:
        //Storing Tic-Tac-Toe board (default empty)
        char board[3][3] = {
            {'.', '.', '.'},
            {'.', '.', '.'},
            {'.', '.', '.'}
        };
        Game_States game_state = in_progress;
        int number_of_plays = 0;
};

//==================================
//=========Helper Functions=========
//==================================

//Function using nested FOR loops printing the game_state
void print_board(Game& current_game) {

    cout << "\n+" << " - - - " << "+" << endl;

    for(int i {0}; i < 3; i++) {
        cout << "| ";
        for(int j {0}; j < 3; j++) {
            cout << current_game.board[i][j] << " "; 
        }
        cout << "|" << endl;
    }
    
    cout << "+" << " - - - " << "+\n" << endl;
}

void make_move(Game& current_game, bool is_player_turn) {
    //Player Plays
    if(is_player_turn) {
        while (true) {
            cout << "Please select which tile to mark by typing 1 - 9. (Assume 1 is top left)" << endl;
            int player_choice {};
            cin >> player_choice;

            int row = (player_choice - 1) / 3;
            int column = (player_choice - 1) % 3;

            char chosen_tile = current_game.board[row][column];

            if(chosen_tile == '.') {
                current_game.board[row][column] = 'O';
                break;
            }
            else {
                cout << 
                "\n==================================================\n" <<
                "Sorry, that tile is occupied. Please, try again.\n" <<
                "=================================================="
                << endl;
                print_board(current_game);
            }
        }
        
    }
    //Computer Plays
    else {
        while(true) {
            //Exit if board full
            if (current_game.number_of_plays >= 9) return;

            //Roll the computer choice 1 - 9
            int computer_choice = (rand() % 9) + 1;  // 1..9

            //Locate tile on the board
            int row = (computer_choice - 1) / 3;
            int column = (computer_choice - 1) % 3; 
            char chosen_tile = current_game.board[row][column];

            if(chosen_tile == '.') {
                current_game.board[row][column] = 'X';
                break;
            }
        }
    }

    //Ensure both computer and player increment counter
    current_game.number_of_plays++;
}

//Helper function to check all win conditions
void check_win(Game& game_instance, char comparison_char) {
    // Check All Rows
    for (int row = 0; row < 3; row++) {
        if (game_instance.board[row][0] == comparison_char &&
            game_instance.board[row][1] == comparison_char &&
            game_instance.board[row][2] == comparison_char) {

            game_instance.game_state = (comparison_char == 'O') ? player_won : computer_won;
            return;
        }
    }

    // Check All Columns
    for (int col = 0; col < 3; col++) {
        if (game_instance.board[0][col] == comparison_char &&
            game_instance.board[1][col] == comparison_char &&
            game_instance.board[2][col] == comparison_char) {

            game_instance.game_state = (comparison_char == 'O') ? player_won : computer_won;
            return;
        }
    }

    // Check Diagonals
    if (game_instance.board[0][0] == comparison_char &&
        game_instance.board[1][1] == comparison_char &&
        game_instance.board[2][2] == comparison_char) {

        game_instance.game_state = (comparison_char == 'O') ? player_won : computer_won;
        return;
    }

    if (game_instance.board[0][2] == comparison_char &&
        game_instance.board[1][1] == comparison_char &&
        game_instance.board[2][0] == comparison_char) {

        game_instance.game_state = (comparison_char == 'O') ? player_won : computer_won;
        return;
    }
}


int main() {

    //Initialize Game instance
    Game test_game;

    // Providing a seed value
    srand((unsigned) time(NULL));

    //Keep playing until game is no longer "in progress"
    while(test_game.game_state == in_progress) {
        print_board(test_game);
        
        //Player's turn and win check
        make_move(test_game, true); 
        check_win(test_game, 'O');

        //Computer's turn and win check
        make_move(test_game, false); 
        check_win(test_game, 'X'); 

        //Check if board full
        if(test_game.number_of_plays >= 9 && test_game.game_state == in_progress) {
            test_game.game_state = draw;
        }
    }

    print_board(test_game);

    switch(test_game.game_state) {
        case draw: 
            cout <<
            "======================\n" <<
            "=====ITS A DRAW!!!====\n" <<
            "=======================" << endl;
            break;
        case player_won: 
            cout <<
            "=======================\n" <<
            "======YOU WON!!!!=====\n" <<
            "======================" << endl;
            break;
        case computer_won: 
            cout <<
            "=======================\n" <<
            "======YOU LOST!!!!=====\n" <<
            "=======================" << endl;
            break;
    }

    return 0;
}

