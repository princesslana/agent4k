#include "engine.h"

long long perft(int depth) {
    if (depth == 0) return 1;
    
    vector<Move> moves = generate_moves();
    long long nodes = 0;
    
    for (const Move& move : moves) {
        Position current_pos = get_position();
        Position new_pos = apply_move(current_pos, move);
        set_position(new_pos);
        nodes += perft(depth - 1);
        set_position(current_pos);
    }
    
    return nodes;
}

void perft_divide(int depth) {
    vector<Move> moves = generate_moves();
    long long total_nodes = 0;
    
    cout << "Split perft(" << depth << ") from starting position:" << endl;
    
    for (const Move& move : moves) {
        Position current_pos = get_position();
        Position new_pos = apply_move(current_pos, move);
        set_position(new_pos);
        
        long long nodes = (depth > 1) ? perft(depth - 1) : 1;
        set_position(current_pos);
        
        string move_str = square_to_string(move.f) + square_to_string(move.t);
        cout << move_str << " - " << nodes << endl;
        total_nodes += nodes;
    }
    
    cout << "Total: " << total_nodes << endl;
}

int main() {
    cout << "=== Current Engine Perft Test ===" << endl;
    
    init_board();
    
    // Test basic perft
    cout << "Perft(1) = " << perft(1) << " (expected: 20)" << endl;
    cout << "Perft(2) = " << perft(2) << " (expected: 400)" << endl;
    cout << "Perft(3) = " << perft(3) << " (expected: 8902)" << endl;
    
    cout << "\n=== Split Perft(3) ===" << endl;
    perft_divide(3);
    
    return 0;
}