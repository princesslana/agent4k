#include "chess.h"
#include <iostream>
using namespace std;

int main() {
    Board board;
    string line;
    
    while (getline(cin, line)) {
        if (line == "uci") {
            cout << "id name Agent4k" << endl;
            cout << "id author Claude" << endl;
            cout << "uciok" << endl;
        }
        else if (line == "isready") {
            cout << "readyok" << endl;
        }
        else if (line.substr(0, 8) == "position") {
            board = parse_uci_position(line);
        }
        else if (line.substr(0, 2) == "go") {
            // Search for best move
            Move best_move = search_best_move(board, 3); // 3-ply search
            
            if (best_move.from != best_move.to) {
                // Get evaluation for info line
                int score = evaluate_position(board);
                cout << "info score cp " << score << endl;
                cout << "bestmove " << move_to_uci(best_move) << endl;
            } else {
                cout << "bestmove a1a1" << endl;
            }
        }
        else if (line == "quit") {
            break;
        }
        // Ignore other commands (graceful handling as required)
    }
    
    return 0;
}