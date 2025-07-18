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
            // Just pick the first legal move
            vector<Move> moves = generate_all_legal_moves(board);
            
            if (!moves.empty()) {
                cout << "info score cp 0" << endl;  // Output info line to avoid fastchess warnings
                cout << "bestmove " << move_to_uci(moves[0]) << endl;
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