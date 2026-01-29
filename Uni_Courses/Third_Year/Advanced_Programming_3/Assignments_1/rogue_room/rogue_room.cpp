#include <iostream>
#include <string>
#include <vector>

using namespace std;

//================================
//=======Class setup section======
//================================

class Game {
public:
    int number_of_rows = 10;
    int number_of_columns = 15;
    int player_row = 0;
    int player_column = 0;
    bool playing = true;
};


//Function used for room drawing
void draw_room(Game& game) {
    string horizontal_boundaries(game.number_of_columns, '-');

    cout << "+" << horizontal_boundaries << "+" << endl;

    //Nested FOR loops checking player row and column position
    for(int i {0}; i < game.number_of_rows; i++) {
        cout << "|";
        for(int j {0}; j < game.number_of_columns; j++) {
            if(game.player_row == i && game.player_column == j) {
                cout << "@";
            }
            else {
                cout << ".";
            }
        }
        cout << "|" << endl;
    }
    
    cout << "+" << horizontal_boundaries << "+" << endl;

}

//This prompts the user for an input direction in the WASD style
bool player_move(Game& game) {

    //Terminal interface
    cout << "Press WASD to move. Press Q to quit ";
    char input{};
    cin >> input;

    //User input & Player coordinate check
    switch (input) {
    case 'W':
    case 'w':
        if (game.player_row > 0)
            game.player_row--;
        else
            cout << "\n" 
            << "========================================\n"
            << "Sorry, you have reached the North Wall.\n" 
            << "========================================\n"
            << endl;
        break;

    case 'S':
    case 's':
        if (game.player_row < game.number_of_rows - 1)
            game.player_row++;
        else
            cout << "\n" 
            << "========================================\n"
            << "Sorry, you have reached the South Wall.\n" 
            << "========================================\n"
            << endl;
        break;

    case 'A':
    case 'a':
        if (game.player_column > 0)
            game.player_column--;
        else
            cout << "\n" 
            << "========================================\n"
            << "Sorry, you have reached the West Wall.\n" 
            << "========================================\n"
            << endl;
        break;

    case 'D':
    case 'd':
        if (game.player_column < game.number_of_columns - 1)
            game.player_column++;
        else
            cout << "\n" 
            << "========================================\n"
            << "Sorry, you have reached the East Wall.\n" 
            << "========================================\n"
            << endl;
        break;
    
    //Game Termination 
    case 'q': case 'Q': return false;

    default:
        //Restart again if the user types incorrectly
        cout << "Invalid input.\n";
    }

    return true;

}

int main() {

    Game current_game;

    while(current_game.playing) {
        draw_room(current_game);
        if(!player_move(current_game)) {
            current_game.playing = false;
        };
    }
    
}

