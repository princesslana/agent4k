#include "engine.h"

int main(){
string line;
while (getline(cin, line)){
istringstream iss(line);
string command;
iss >> command;
if (command == "uci"){
cout << "id name Agent4k" << endl;
cout << "id author Claude" << endl;
cout << "uciok" << endl;
} else if (command == "isready"){
cout << "readyok" << endl;
} else if (command == "ucinewgame"){
init_board();
} else if (command == "position"){
parse_position(line);
} else if (command == "go"){
start_time = chrono::steady_clock::now();
string token;
while (iss >> token){
if (token == "wtime" || token == "btime"){
int time_ms;
iss >> time_ms;
if ((token == "wtime" && s == WHITE) || (token == "btime" && s == BLACK)){
time_limit_ms = time_ms / 20;
}
} else if (token == "movetime"){
iss >> time_limit_ms;
} else if (token == "depth"){
int depth;
iss >> depth;
time_limit_ms = 10000;
}
}
Move best = find_best_move(4);
string move_str = square_to_string(best.f) + square_to_string(best.t);
if (best.promotion != 0) {
    // Add promotion piece to UCI move notation
    char promo_char = 'q'; // Default to queen
    int piece_type = abs(best.promotion);
    if (piece_type == QUEEN) promo_char = 'q';
    else if (piece_type == ROOK) promo_char = 'r';
    else if (piece_type == BISHOP) promo_char = 'b';
    else if (piece_type == KNIGHT) promo_char = 'n';
    move_str += promo_char;
}
cout << "bestmove " << move_str << endl;
} else if (command == "quit"){
break;
}
}
return 0;
}