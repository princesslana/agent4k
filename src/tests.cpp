#include "engine.h"

void test_queen_diagonal_moves() {
    cout << "=== Testing Queen Diagonal Moves ===\n";
    
    // Set up position: c2c3 d7d6
    init_board();
    parse_position("position startpos moves c2c3 d7d6");
    
    vector<Move> moves = generate_moves();
    
    bool found_d1a4 = false;
    bool found_d1b3 = false;
    bool found_d1c2 = false;
    
    cout << "Generated Queen moves from d1:\n";
    for (const Move& move : moves) {
        if (move.f == square_from_string("d1")) {
            string move_str = square_to_string(move.f) + square_to_string(move.t);
            cout << "  " << move_str << "\n";
            
            if (move.t == square_from_string("a4")) found_d1a4 = true;
            if (move.t == square_from_string("b3")) found_d1b3 = true;
            if (move.t == square_from_string("c2")) found_d1c2 = true;
        }
    }
    
    cout << "Results:\n";
    cout << "  d1a4: " << (found_d1a4 ? "FOUND" : "MISSING") << "\n";
    cout << "  d1b3: " << (found_d1b3 ? "FOUND" : "MISSING") << "\n";
    cout << "  d1c2: " << (found_d1c2 ? "FOUND" : "MISSING") << "\n";
    
    if (found_d1a4 && found_d1b3 && found_d1c2) {
        cout << "✓ All Queen diagonal moves correctly generated\n";
    } else {
        cout << "✗ Missing Queen diagonal moves\n";
    }
}

void test_basic_move_generation() {
    cout << "=== Testing Basic Move Generation ===\n";
    
    init_board();
    vector<Move> moves = generate_moves();
    
    cout << "Starting position generates " << moves.size() << " moves (expected: 20)\n";
    
    if (moves.size() == 20) {
        cout << "✓ Correct number of starting moves\n";
    } else {
        cout << "✗ Wrong number of starting moves\n";
    }
}


void test_pawn_promotion_moves() {
    cout << "=== Testing Pawn Promotion Moves ===\n";
    
    // Position where black pawn can promote via capture
    string promotion_position = "position startpos moves a2a4 c7c5 b1c3 b7b5 g1f3 a7a6 b2b4 a6a5 c1a3 c5c4 d1b1 d7d6 b1d1 e7e6 d1c1 e6e5 c1b2 d6d5 a1d1 e5e4 d1a1 b5a4 a1d1 a5b4 d1a1 b4c3 a1c1 d5d4 c1d1 e4f3 d1a1 d4d3 a1d1 f3e2 d1b1 f7f6 b1d1 d3c2 d1a1 f6f5 a1d1";
    
    cout << "Promotion position: " << promotion_position << "\n";
    parse_position(promotion_position);
    
    cout << "Current side to move: " << (s == WHITE ? "WHITE" : "BLACK") << "\n";
    
    // Check board state around promotion moves
    int e2 = square_from_string("e2");
    int f1 = square_from_string("f1");
    int c2 = square_from_string("c2");
    int c1 = square_from_string("c1");
    int d1 = square_from_string("d1");
    
    cout << "e2 (" << e2 << ") contains: " << b[e2] << " (black pawn)\n";
    cout << "f1 (" << f1 << ") contains: " << b[f1] << " (white bishop)\n";
    cout << "c2 (" << c2 << ") contains: " << b[c2] << " (black pawn)\n";
    cout << "c1 (" << c1 << ") contains: " << b[c1] << "\n";
    cout << "d1 (" << d1 << ") contains: " << b[d1] << " (white rook)\n";
    
    vector<Move> moves = generate_moves();
    bool found_e2f1_promotion = false;
    bool found_c2c1_promotion = false;
    bool found_c2d1_promotion = false;
    
    cout << "\nChecking for promotion moves:\n";
    for (const Move& move : moves) {
        string move_str = square_to_string(move.f) + square_to_string(move.t);
        if (move_str == "e2f1" && move.promotion != 0) {
            found_e2f1_promotion = true;
            cout << "✓ Found e2f1 promotion move (piece: " << move.p << ", capture: " << move.capture << ", promotion: " << move.promotion << ")\n";
        }
        if (move_str == "c2c1" && move.promotion != 0) {
            found_c2c1_promotion = true;
            cout << "✓ Found c2c1 promotion move (piece: " << move.p << ", promotion: " << move.promotion << ")\n";
        }
        if (move_str == "c2d1" && move.promotion != 0) {
            found_c2d1_promotion = true;
            cout << "✓ Found c2d1 promotion move (piece: " << move.p << ", capture: " << move.capture << ", promotion: " << move.promotion << ")\n";
        }
    }
    
    cout << "\nPromotion move test results:\n";
    cout << "e2f1 promotion: " << (found_e2f1_promotion ? "FOUND" : "MISSING") << "\n";
    cout << "c2c1 promotion: " << (found_c2c1_promotion ? "FOUND" : "MISSING") << "\n";
    cout << "c2d1 promotion: " << (found_c2d1_promotion ? "FOUND" : "MISSING") << "\n";
    
    if (found_e2f1_promotion && found_c2c1_promotion && found_c2d1_promotion) {
        cout << "✓ All expected promotion moves correctly generated\n";
    } else {
        cout << "✗ Some promotion moves missing\n";
    }
}

void test_uci_promotion_notation() {
    cout << "=== Testing UCI Promotion Notation ===\n";
    
    // Test UCI move string formatting for promotion moves
    Move e2f1_promotion(square_from_string("e2"), square_from_string("f1"), -1, 3);
    e2f1_promotion.promotion = -5; // Black queen
    
    Move c2c1_promotion(square_from_string("c2"), square_from_string("c1"), -1, 0);
    c2c1_promotion.promotion = -5; // Black queen
    
    cout << "Testing UCI notation formatting:\n";
    
    // Test e2f1q (capture promotion)
    string move_str = square_to_string(e2f1_promotion.f) + square_to_string(e2f1_promotion.t);
    if (e2f1_promotion.promotion != 0) {
        char promo_char = 'q'; // Default to queen
        int piece_type = abs(e2f1_promotion.promotion);
        if (piece_type == QUEEN) promo_char = 'q';
        else if (piece_type == ROOK) promo_char = 'r';
        else if (piece_type == BISHOP) promo_char = 'b';
        else if (piece_type == KNIGHT) promo_char = 'n';
        move_str += promo_char;
    }
    cout << "e2f1 promotion UCI notation: " << move_str << "\n";
    
    // Test c2c1q (non-capture promotion)
    move_str = square_to_string(c2c1_promotion.f) + square_to_string(c2c1_promotion.t);
    if (c2c1_promotion.promotion != 0) {
        char promo_char = 'q'; // Default to queen
        int piece_type = abs(c2c1_promotion.promotion);
        if (piece_type == QUEEN) promo_char = 'q';
        else if (piece_type == ROOK) promo_char = 'r';
        else if (piece_type == BISHOP) promo_char = 'b';
        else if (piece_type == KNIGHT) promo_char = 'n';
        move_str += promo_char;
    }
    cout << "c2c1 promotion UCI notation: " << move_str << "\n";
    
    cout << "✓ UCI promotion notation test completed\n";
}

int main() {
    test_basic_move_generation();
    cout << "\n";
    test_queen_diagonal_moves();
    cout << "\n";
    test_pawn_promotion_moves();
    cout << "\n";
    test_uci_promotion_notation();
    return 0;
}