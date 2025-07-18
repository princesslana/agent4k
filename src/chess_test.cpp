#include "chess.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>

void test_square_utilities() {
    cout << "Testing square utilities..." << endl;
    
    // Test valid squares
    assert(is_valid_square(0));   // a1
    assert(is_valid_square(7));   // h1
    assert(is_valid_square(56));  // a8
    assert(is_valid_square(63));  // h8
    
    // Test invalid squares
    assert(!is_valid_square(-1));
    assert(!is_valid_square(64));
    
    // Test square to string conversion
    assert(square_to_string(0) == "a1");
    assert(square_to_string(7) == "h1");
    assert(square_to_string(56) == "a8");
    assert(square_to_string(63) == "h8");
    
    // Test string to square conversion
    assert(string_to_square("a1") == 0);
    assert(string_to_square("h1") == 7);
    assert(string_to_square("a8") == 56);
    assert(string_to_square("h8") == 63);
    
    cout << "✓ Square utilities tests passed" << endl;
}

void test_board_initialization() {
    cout << "Testing board initialization..." << endl;
    
    Board board = create_starting_position();
    
    // Test white pieces
    assert(get_piece(board, 0) == 4);   // white rook on a1
    assert(get_piece(board, 1) == 2);   // white knight on b1
    assert(get_piece(board, 2) == 3);   // white bishop on c1
    assert(get_piece(board, 3) == 5);   // white queen on d1
    assert(get_piece(board, 4) == 6);   // white king on e1
    assert(get_piece(board, 5) == 3);   // white bishop on f1
    assert(get_piece(board, 6) == 2);   // white knight on g1
    assert(get_piece(board, 7) == 4);   // white rook on h1
    
    // Test white pawns
    for (int i = 8; i < 16; i++) {
        assert(get_piece(board, i) == 1);  // white pawn
    }
    
    // Test empty squares
    for (int i = 16; i < 48; i++) {
        assert(get_piece(board, i) == 0);  // empty
    }
    
    // Test black pawns
    for (int i = 48; i < 56; i++) {
        assert(get_piece(board, i) == -1);  // black pawn
    }
    
    // Test black pieces
    assert(get_piece(board, 56) == -4);  // black rook on a8
    assert(get_piece(board, 57) == -2);  // black knight on b8
    assert(get_piece(board, 58) == -3);  // black bishop on c8
    assert(get_piece(board, 59) == -5);  // black queen on d8
    assert(get_piece(board, 60) == -6);  // black king on e8
    assert(get_piece(board, 61) == -3);  // black bishop on f8
    assert(get_piece(board, 62) == -2);  // black knight on g8
    assert(get_piece(board, 63) == -4);  // black rook on h8
    
    // Test turn
    assert(is_white_to_move(board));
    
    cout << "✓ Board initialization tests passed" << endl;
}

void test_board_manipulation() {
    cout << "Testing board manipulation..." << endl;
    
    Board board = create_starting_position();
    
    // Test setting pieces
    set_piece(board, 20, 1);  // place white pawn on e3
    assert(get_piece(board, 20) == 1);
    
    set_piece(board, 20, 0);  // clear square
    assert(get_piece(board, 20) == 0);
    
    set_piece(board, 20, -5);  // place black queen
    assert(get_piece(board, 20) == -5);
    
    cout << "✓ Board manipulation tests passed" << endl;
}

void test_pawn_moves() {
    cout << "Testing pawn move generation..." << endl;
    
    // Test 1: Starting position white pawn moves
    Board board = create_starting_position();
    
    // Test pawn on e2 - should have 2 moves: e3 and e4
    vector<Move> e2_moves = generate_pawn_moves(board, string_to_square("e2"));
    assert(e2_moves.size() == 2);
    
    bool found_e3 = false, found_e4 = false;
    for (const Move& move : e2_moves) {
        if (move.to == string_to_square("e3")) found_e3 = true;
        if (move.to == string_to_square("e4")) found_e4 = true;
    }
    assert(found_e3 && found_e4);
    
    // Test 2: Blocked pawn - place piece on e3, pawn on e2 should have no moves
    set_piece(board, string_to_square("e3"), 2); // white knight blocks
    vector<Move> e2_blocked = generate_pawn_moves(board, string_to_square("e2"));
    assert(e2_blocked.size() == 0);
    
    // Test 3: Pawn captures
    Board capture_board = create_starting_position();
    set_piece(capture_board, string_to_square("e4"), 1); // white pawn on e4
    set_piece(capture_board, string_to_square("d5"), -1); // black pawn on d5
    set_piece(capture_board, string_to_square("f5"), -2); // black knight on f5
    
    vector<Move> e4_moves = generate_pawn_moves(capture_board, string_to_square("e4"));
    assert(e4_moves.size() == 3); // e5, capture d5, capture f5
    
    bool found_e5 = false, found_d5 = false, found_f5 = false;
    for (const Move& move : e4_moves) {
        if (move.to == string_to_square("e5")) found_e5 = true;
        if (move.to == string_to_square("d5")) found_d5 = true;
        if (move.to == string_to_square("f5")) found_f5 = true;
    }
    assert(found_e5 && found_d5 && found_f5);
    
    // Test 4: Black pawn moves
    vector<Move> e7_moves = generate_pawn_moves(board, string_to_square("e7"));
    assert(e7_moves.size() == 2); // e6 and e5
    
    bool found_e6 = false, found_e5_black = false;
    for (const Move& move : e7_moves) {
        if (move.to == string_to_square("e6")) found_e6 = true;
        if (move.to == string_to_square("e5")) found_e5_black = true;
    }
    assert(found_e6 && found_e5_black);
    
    cout << "✓ Pawn move generation tests passed" << endl;
}

void test_knight_moves() {
    cout << "Testing knight move generation..." << endl;
    
    // Test 1: Knight in center (can't move to d2/f2 due to pawns)
    Board board = create_starting_position();
    set_piece(board, string_to_square("e4"), 2); // white knight on e4
    
    vector<Move> knight_moves = generate_knight_moves(board, string_to_square("e4"));
    assert(knight_moves.size() == 6); // c3, g3, c5, g5, d6, f6 (d2, f2 blocked by pawns)
    
    // Expected knight moves from e4: c3, g3, c5, g5, d6, f6
    vector<string> expected = {"c3", "g3", "c5", "g5", "d6", "f6"};
    for (const string& expected_square : expected) {
        bool found = false;
        for (const Move& move : knight_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 1b: Knight in center on empty board (maximum mobility)
    Board empty_board;
    set_piece(empty_board, string_to_square("e4"), 2); // white knight on e4
    
    vector<Move> max_moves = generate_knight_moves(empty_board, string_to_square("e4"));
    assert(max_moves.size() == 8); // All 8 knight moves available
    
    // Expected knight moves from e4: d2, f2, c3, g3, c5, g5, d6, f6
    vector<string> all_expected = {"d2", "f2", "c3", "g3", "c5", "g5", "d6", "f6"};
    for (const string& expected_square : all_expected) {
        bool found = false;
        for (const Move& move : max_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 2: Knight in corner (limited mobility)
    Board corner_board = create_starting_position();
    vector<Move> corner_moves = generate_knight_moves(corner_board, string_to_square("b1"));
    assert(corner_moves.size() == 2); // a3, c3 (d2 blocked by pawn)
    
    bool found_a3 = false, found_c3 = false;
    for (const Move& move : corner_moves) {
        if (move.to == string_to_square("a3")) found_a3 = true;
        if (move.to == string_to_square("c3")) found_c3 = true;
    }
    assert(found_a3 && found_c3);
    
    // Test 3: Knight captures
    Board capture_board = create_starting_position();
    set_piece(capture_board, string_to_square("e4"), 2); // white knight
    set_piece(capture_board, string_to_square("d6"), -1); // black pawn
    set_piece(capture_board, string_to_square("f6"), -3); // black bishop
    
    vector<Move> capture_moves = generate_knight_moves(capture_board, string_to_square("e4"));
    assert(capture_moves.size() == 6); // Still 6 moves (including captures, d2/f2 blocked)
    
    // Verify captures are included
    bool found_d6_capture = false, found_f6_capture = false;
    for (const Move& move : capture_moves) {
        if (move.to == string_to_square("d6")) found_d6_capture = true;
        if (move.to == string_to_square("f6")) found_f6_capture = true;
    }
    assert(found_d6_capture && found_f6_capture);
    
    cout << "✓ Knight move generation tests passed" << endl;
}

void test_bishop_moves() {
    cout << "Testing bishop move generation..." << endl;
    
    // Test 1: Bishop in center on empty board (maximum mobility)
    Board empty_board;
    set_piece(empty_board, string_to_square("d4"), 3); // white bishop on d4
    
    vector<Move> center_moves = generate_bishop_moves(empty_board, string_to_square("d4"));
    
    assert(center_moves.size() == 13); // 3+3+3+4 moves in 4 diagonal directions
    
    // Test all 4 diagonal directions from d4
    vector<string> expected_moves = {
        // Up-right diagonal: e5, f6, g7, h8
        "e5", "f6", "g7", "h8",
        // Up-left diagonal: c5, b6, a7
        "c5", "b6", "a7",
        // Down-right diagonal: e3, f2, g1
        "e3", "f2", "g1",
        // Down-left diagonal: c3, b2, a1
        "c3", "b2", "a1"
    };
    
    for (const string& expected_square : expected_moves) {
        bool found = false;
        for (const Move& move : center_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 2: Bishop blocked by own pieces
    Board blocked_board = create_starting_position();
    set_piece(blocked_board, string_to_square("d4"), 3); // white bishop on d4
    
    vector<Move> blocked_moves = generate_bishop_moves(blocked_board, string_to_square("d4"));
    
    assert(blocked_moves.size() == 8); // e5, f6, g7, c5, b6, a7, e3, c3 (blocked by own pawns on rank 2)
    
    // Should not be able to move to f2, g1, b2, a1 due to own pawns on rank 2
    for (const Move& move : blocked_moves) {
        int target_rank = move.to / 8;
        assert(target_rank >= 2); // Should not reach rank 1 due to own pawns (rank 2 is ok)
    }
    
    // Test 3: Bishop captures
    Board capture_board = create_starting_position();
    set_piece(capture_board, string_to_square("d4"), 3); // white bishop on d4
    set_piece(capture_board, string_to_square("f6"), -1); // black pawn on f6
    set_piece(capture_board, string_to_square("b6"), -2); // black knight on b6
    
    vector<Move> capture_moves = generate_bishop_moves(capture_board, string_to_square("d4"));
    
    // Should be able to capture on f6 and b6, but not move beyond them
    bool found_f6_capture = false, found_b6_capture = false;
    bool found_g7 = false, found_a7 = false;
    
    for (const Move& move : capture_moves) {
        if (move.to == string_to_square("f6")) found_f6_capture = true;
        if (move.to == string_to_square("b6")) found_b6_capture = true;
        if (move.to == string_to_square("g7")) found_g7 = true;
        if (move.to == string_to_square("a7")) found_a7 = true;
    }
    
    assert(found_f6_capture && found_b6_capture);
    assert(!found_g7 && !found_a7); // Should not be able to move beyond captures
    
    // Test 4: Bishop in corner (limited mobility)
    Board corner_board;
    set_piece(corner_board, string_to_square("a1"), 3); // white bishop on a1
    
    vector<Move> corner_moves = generate_bishop_moves(corner_board, string_to_square("a1"));
    assert(corner_moves.size() == 7); // Only up-right diagonal: b2, c3, d4, e5, f6, g7, h8
    
    cout << "✓ Bishop move generation tests passed" << endl;
}

void test_rook_moves() {
    cout << "Testing rook move generation..." << endl;
    
    // Test 1: Rook in center on empty board (maximum mobility)
    Board empty_board;
    set_piece(empty_board, string_to_square("d4"), 4); // white rook on d4
    
    vector<Move> center_moves = generate_rook_moves(empty_board, string_to_square("d4"));
    assert(center_moves.size() == 14); // 7 vertical + 7 horizontal moves
    
    // Test all 4 directions from d4
    vector<string> expected_moves = {
        // Up: d5, d6, d7, d8
        "d5", "d6", "d7", "d8",
        // Down: d3, d2, d1
        "d3", "d2", "d1",
        // Right: e4, f4, g4, h4
        "e4", "f4", "g4", "h4",
        // Left: c4, b4, a4
        "c4", "b4", "a4"
    };
    
    for (const string& expected_square : expected_moves) {
        bool found = false;
        for (const Move& move : center_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 2: Rook blocked by own pieces
    Board blocked_board = create_starting_position();
    set_piece(blocked_board, string_to_square("d4"), 4); // white rook on d4
    
    vector<Move> blocked_moves = generate_rook_moves(blocked_board, string_to_square("d4"));
    
    assert(blocked_moves.size() == 11); // d5, d6, d7, d3, c4, b4, a4, e4, f4, g4, h4 (blocked by pawns on d2 and d7)
    
    // Should not be able to move to d2, d1, d8 due to being blocked
    for (const Move& move : blocked_moves) {
        assert(move.to != string_to_square("d2"));
        assert(move.to != string_to_square("d1"));
        assert(move.to != string_to_square("d8"));
    }
    
    // Test 3: Rook captures
    Board capture_board = create_starting_position();
    set_piece(capture_board, string_to_square("d4"), 4); // white rook on d4
    set_piece(capture_board, string_to_square("d6"), -1); // black pawn on d6
    set_piece(capture_board, string_to_square("f4"), -2); // black knight on f4
    
    vector<Move> capture_moves = generate_rook_moves(capture_board, string_to_square("d4"));
    
    // Should be able to capture on d6 and f4, but not move beyond them
    bool found_d6_capture = false, found_f4_capture = false;
    bool found_d7 = false, found_g4 = false;
    
    for (const Move& move : capture_moves) {
        if (move.to == string_to_square("d6")) found_d6_capture = true;
        if (move.to == string_to_square("f4")) found_f4_capture = true;
        if (move.to == string_to_square("d7")) found_d7 = true;
        if (move.to == string_to_square("g4")) found_g4 = true;
    }
    
    assert(found_d6_capture && found_f4_capture);
    assert(!found_d7 && !found_g4); // Should not be able to move beyond captures
    
    // Test 4: Rook in corner (limited mobility)
    Board corner_board;
    set_piece(corner_board, string_to_square("a1"), 4); // white rook on a1
    
    vector<Move> corner_moves = generate_rook_moves(corner_board, string_to_square("a1"));
    assert(corner_moves.size() == 14); // 7 up + 7 right moves
    
    // Test 5: Rook on edge (partial mobility)
    Board edge_board;
    set_piece(edge_board, string_to_square("a4"), 4); // white rook on a4
    
    vector<Move> edge_moves = generate_rook_moves(edge_board, string_to_square("a4"));
    assert(edge_moves.size() == 14); // 4 up + 3 down + 7 right moves
    
    cout << "✓ Rook move generation tests passed" << endl;
}

void test_queen_moves() {
    cout << "Testing queen move generation..." << endl;
    
    // Test 1: Queen in center on empty board (maximum mobility)
    Board empty_board;
    set_piece(empty_board, string_to_square("d4"), 5); // white queen on d4
    
    vector<Move> center_moves = generate_queen_moves(empty_board, string_to_square("d4"));
    assert(center_moves.size() == 27); // 14 rook moves + 13 bishop moves
    
    // Test all 8 directions from d4 (4 rook + 4 bishop)
    vector<string> expected_moves = {
        // Rook moves: up, down, left, right
        "d5", "d6", "d7", "d8",         // up
        "d3", "d2", "d1",               // down
        "c4", "b4", "a4",               // left
        "e4", "f4", "g4", "h4",         // right
        // Bishop moves: diagonals
        "e5", "f6", "g7", "h8",         // up-right
        "c5", "b6", "a7",               // up-left
        "e3", "f2", "g1",               // down-right
        "c3", "b2", "a1"                // down-left
    };
    
    for (const string& expected_square : expected_moves) {
        bool found = false;
        for (const Move& move : center_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 2: Queen blocked by own pieces
    Board blocked_board = create_starting_position();
    set_piece(blocked_board, string_to_square("d4"), 5); // white queen on d4
    
    vector<Move> blocked_moves = generate_queen_moves(blocked_board, string_to_square("d4"));
    assert(blocked_moves.size() == 19); // 11 rook moves + 8 bishop moves from previous tests
    
    // Test 3: Queen captures
    Board capture_board = create_starting_position();
    set_piece(capture_board, string_to_square("d4"), 5); // white queen on d4
    set_piece(capture_board, string_to_square("d6"), -1); // black pawn on d6
    set_piece(capture_board, string_to_square("f6"), -2); // black knight on f6
    set_piece(capture_board, string_to_square("b4"), -3); // black bishop on b4
    
    vector<Move> capture_moves = generate_queen_moves(capture_board, string_to_square("d4"));
    
    // Should be able to capture on d6, f6, and b4
    bool found_d6_capture = false, found_f6_capture = false, found_b4_capture = false;
    bool found_d7 = false, found_g7 = false, found_a4 = false;
    
    for (const Move& move : capture_moves) {
        if (move.to == string_to_square("d6")) found_d6_capture = true;
        if (move.to == string_to_square("f6")) found_f6_capture = true;
        if (move.to == string_to_square("b4")) found_b4_capture = true;
        if (move.to == string_to_square("d7")) found_d7 = true;
        if (move.to == string_to_square("g7")) found_g7 = true;
        if (move.to == string_to_square("a4")) found_a4 = true;
    }
    
    assert(found_d6_capture && found_f6_capture && found_b4_capture);
    assert(!found_d7 && !found_g7 && !found_a4); // Should not be able to move beyond captures
    
    // Test 4: Queen in corner (still powerful)
    Board corner_board;
    set_piece(corner_board, string_to_square("a1"), 5); // white queen on a1
    
    vector<Move> corner_moves = generate_queen_moves(corner_board, string_to_square("a1"));
    assert(corner_moves.size() == 21); // 14 rook moves + 7 bishop moves
    
    cout << "✓ Queen move generation tests passed" << endl;
}

void test_king_moves() {
    cout << "Testing king move generation..." << endl;
    
    // Test 1: King in center on empty board (maximum mobility)
    Board empty_board;
    set_piece(empty_board, string_to_square("d4"), 6); // white king on d4
    
    vector<Move> center_moves = generate_king_moves(empty_board, string_to_square("d4"));
    assert(center_moves.size() == 8); // All 8 surrounding squares
    
    // Test all 8 directions from d4
    vector<string> expected_moves = {
        "c3", "c4", "c5",  // left column
        "d3", "d5",        // same file (up/down)
        "e3", "e4", "e5"   // right column
    };
    
    for (const string& expected_square : expected_moves) {
        bool found = false;
        for (const Move& move : center_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 2: King blocked by own pieces
    Board blocked_board = create_starting_position();
    set_piece(blocked_board, string_to_square("e2"), 6); // white king on e2 (near pawns)
    
    vector<Move> blocked_moves = generate_king_moves(blocked_board, string_to_square("e2"));
    
    assert(blocked_moves.size() == 3); // d3, e3, f3 (blocked by own pieces on d1, d2, e1, f1, f2)
    
    // Should not be able to move to squares with own pieces
    for (const Move& move : blocked_moves) {
        assert(move.to != string_to_square("d1")); // white queen
        assert(move.to != string_to_square("d2")); // white pawn
        assert(move.to != string_to_square("e1")); // where king was originally
        assert(move.to != string_to_square("f1")); // white bishop
        assert(move.to != string_to_square("f2")); // white pawn
    }
    
    // Test 3: King captures
    Board capture_board = create_starting_position();
    set_piece(capture_board, string_to_square("e4"), 6); // white king on e4
    set_piece(capture_board, string_to_square("d5"), -1); // black pawn on d5
    set_piece(capture_board, string_to_square("f5"), -2); // black knight on f5
    
    vector<Move> capture_moves = generate_king_moves(capture_board, string_to_square("e4"));
    assert(capture_moves.size() == 8); // All 8 squares available
    
    // Should be able to capture on d5 and f5
    bool found_d5_capture = false, found_f5_capture = false;
    for (const Move& move : capture_moves) {
        if (move.to == string_to_square("d5")) found_d5_capture = true;
        if (move.to == string_to_square("f5")) found_f5_capture = true;
    }
    assert(found_d5_capture && found_f5_capture);
    
    // Test 4: King in corner (limited mobility)
    Board corner_board;
    set_piece(corner_board, string_to_square("a1"), 6); // white king on a1
    
    vector<Move> corner_moves = generate_king_moves(corner_board, string_to_square("a1"));
    assert(corner_moves.size() == 3); // Only a2, b1, b2
    
    vector<string> corner_expected = {"a2", "b1", "b2"};
    for (const string& expected_square : corner_expected) {
        bool found = false;
        for (const Move& move : corner_moves) {
            if (move.to == string_to_square(expected_square)) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test 5: King on edge (partial mobility)
    Board edge_board;
    set_piece(edge_board, string_to_square("d1"), 6); // white king on d1 (bottom edge)
    
    vector<Move> edge_moves = generate_king_moves(edge_board, string_to_square("d1"));
    assert(edge_moves.size() == 5); // c1, c2, d2, e1, e2
    
    cout << "✓ King move generation tests passed" << endl;
}

void test_en_passant_moves() {
    cout << "Testing en passant move generation..." << endl;
    
    // Test 1: White pawn captures black pawn en passant
    Board board;
    set_piece(board, string_to_square("e5"), 1);  // white pawn on e5
    set_piece(board, string_to_square("d5"), -1); // black pawn on d5 (just moved 2 squares)
    
    // Black pawn moved from d7 to d5, so en passant target square is d6
    board.en_passant_square = string_to_square("d6");
    
    vector<Move> en_passant_moves = generate_pawn_moves(board, string_to_square("e5"));
    
    // Should have regular move e6 plus en passant capture to d6
    assert(en_passant_moves.size() == 2);
    
    bool found_e6 = false, found_d6_en_passant = false;
    for (const Move& move : en_passant_moves) {
        if (move.to == string_to_square("e6")) found_e6 = true;
        if (move.to == string_to_square("d6")) found_d6_en_passant = true;
    }
    assert(found_e6 && found_d6_en_passant);
    
    // Test 2: Black pawn captures white pawn en passant
    Board board2;
    set_piece(board2, string_to_square("d4"), -1); // black pawn on d4
    set_piece(board2, string_to_square("e4"), 1);  // white pawn on e4 (just moved 2 squares)
    
    // White pawn moved from e2 to e4, so en passant target square is e3
    board2.en_passant_square = string_to_square("e3");
    board2.white_to_move = false;
    
    vector<Move> black_en_passant = generate_pawn_moves(board2, string_to_square("d4"));
    
    // Should have regular move d3 plus en passant capture to e3
    assert(black_en_passant.size() == 2);
    
    bool found_d3 = false, found_e3_en_passant = false;
    for (const Move& move : black_en_passant) {
        if (move.to == string_to_square("d3")) found_d3 = true;
        if (move.to == string_to_square("e3")) found_e3_en_passant = true;
    }
    assert(found_d3 && found_e3_en_passant);
    
    // Test 3: No en passant if no en passant square is set
    Board board3;
    set_piece(board3, string_to_square("e5"), 1);  // white pawn on e5
    set_piece(board3, string_to_square("d5"), -1); // black pawn on d5
    
    // No en passant square set (remains -1)
    
    vector<Move> no_en_passant = generate_pawn_moves(board3, string_to_square("e5"));
    
    // Should only have regular move e6 (no en passant)
    assert(no_en_passant.size() == 1);
    assert(no_en_passant[0].to == string_to_square("e6"));
    
    // Test 4: No en passant if pawn isn't on correct rank
    Board board4;
    set_piece(board4, string_to_square("e4"), 1);  // white pawn on e4 (wrong rank for en passant)
    set_piece(board4, string_to_square("d4"), -1); // black pawn on d4
    
    board4.en_passant_square = string_to_square("d3"); // En passant square set but wrong rank
    
    vector<Move> wrong_rank = generate_pawn_moves(board4, string_to_square("e4"));
    
    // Should only have regular move e5 (no en passant from 4th rank)
    assert(wrong_rank.size() == 1);
    assert(wrong_rank[0].to == string_to_square("e5"));
    
    cout << "✓ En passant move generation tests passed" << endl;
}

void test_castling_moves() {
    cout << "Testing castling move generation..." << endl;
    
    // Test 1: White kingside castling
    Board board;
    set_piece(board, string_to_square("e1"), 6);  // white king on e1
    set_piece(board, string_to_square("h1"), 4);  // white rook on h1
    
    // Enable castling rights
    board.white_can_castle_kingside = true;
    board.white_can_castle_queenside = false;
    board.black_can_castle_kingside = false;
    board.black_can_castle_queenside = false;
    
    vector<Move> king_moves = generate_king_moves(board, string_to_square("e1"));
    
    // Should have regular king moves plus castling move to g1
    bool found_castling = false;
    for (const Move& move : king_moves) {
        if (move.to == string_to_square("g1")) {
            found_castling = true;
            break;
        }
    }
    assert(found_castling);
    
    // Test 2: White queenside castling
    Board board2;
    set_piece(board2, string_to_square("e1"), 6);  // white king on e1
    set_piece(board2, string_to_square("a1"), 4);  // white rook on a1
    
    board2.white_can_castle_kingside = false;
    board2.white_can_castle_queenside = true;
    board2.black_can_castle_kingside = false;
    board2.black_can_castle_queenside = false;
    
    vector<Move> queen_moves = generate_king_moves(board2, string_to_square("e1"));
    
    // Should have regular king moves plus castling move to c1
    bool found_queenside = false;
    for (const Move& move : queen_moves) {
        if (move.to == string_to_square("c1")) {
            found_queenside = true;
            break;
        }
    }
    assert(found_queenside);
    
    // Test 3: Black kingside castling
    Board board3;
    set_piece(board3, string_to_square("e8"), -6); // black king on e8
    set_piece(board3, string_to_square("h8"), -4); // black rook on h8
    
    board3.white_can_castle_kingside = false;
    board3.white_can_castle_queenside = false;
    board3.black_can_castle_kingside = true;
    board3.black_can_castle_queenside = false;
    board3.white_to_move = false;
    
    vector<Move> black_king_moves = generate_king_moves(board3, string_to_square("e8"));
    
    // Should have regular king moves plus castling move to g8
    bool found_black_castling = false;
    for (const Move& move : black_king_moves) {
        if (move.to == string_to_square("g8")) {
            found_black_castling = true;
            break;
        }
    }
    assert(found_black_castling);
    
    // Test 4: No castling if pieces block the path
    Board board4;
    set_piece(board4, string_to_square("e1"), 6);  // white king on e1
    set_piece(board4, string_to_square("h1"), 4);  // white rook on h1
    set_piece(board4, string_to_square("f1"), 3);  // white bishop blocks path
    
    board4.white_can_castle_kingside = true;
    board4.white_can_castle_queenside = false;
    board4.black_can_castle_kingside = false;
    board4.black_can_castle_queenside = false;
    
    vector<Move> blocked_moves = generate_king_moves(board4, string_to_square("e1"));
    
    // Should NOT have castling move to g1 (blocked by bishop)
    bool found_blocked_castling = false;
    for (const Move& move : blocked_moves) {
        if (move.to == string_to_square("g1")) {
            found_blocked_castling = true;
            break;
        }
    }
    assert(!found_blocked_castling);
    
    // Test 5: No castling if castling rights are disabled
    Board board5;
    set_piece(board5, string_to_square("e1"), 6);  // white king on e1
    set_piece(board5, string_to_square("h1"), 4);  // white rook on h1
    
    // No castling rights
    board5.white_can_castle_kingside = false;
    board5.white_can_castle_queenside = false;
    board5.black_can_castle_kingside = false;
    board5.black_can_castle_queenside = false;
    
    vector<Move> no_rights_moves = generate_king_moves(board5, string_to_square("e1"));
    
    // Should NOT have castling move to g1 (no rights)
    bool found_no_rights_castling = false;
    for (const Move& move : no_rights_moves) {
        if (move.to == string_to_square("g1")) {
            found_no_rights_castling = true;
            break;
        }
    }
    assert(!found_no_rights_castling);
    
    cout << "✓ Castling move generation tests passed" << endl;
}

void test_pawn_promotion() {
    cout << "Testing pawn promotion move generation..." << endl;
    
    // Test 1: White pawn promotes on 8th rank
    Board board;
    set_piece(board, string_to_square("e7"), 1);  // white pawn on e7
    
    vector<Move> promotion_moves = generate_pawn_moves(board, string_to_square("e7"));
    
    // Should have 4 promotion moves: queen, rook, bishop, knight
    assert(promotion_moves.size() == 4);
    
    // Check that all promotions are to e8 with different promotion values
    bool found_queen = false, found_rook = false, found_bishop = false, found_knight = false;
    for (const Move& move : promotion_moves) {
        assert(move.to == string_to_square("e8"));
        if (move.promotion == 5) found_queen = true;
        else if (move.promotion == 4) found_rook = true;
        else if (move.promotion == 3) found_bishop = true;
        else if (move.promotion == 2) found_knight = true;
    }
    assert(found_queen && found_rook && found_bishop && found_knight);
    
    // Test 2: Black pawn promotes on 1st rank
    Board board2;
    set_piece(board2, string_to_square("d2"), -1); // black pawn on d2
    board2.white_to_move = false;
    
    vector<Move> black_promotion = generate_pawn_moves(board2, string_to_square("d2"));
    
    // Should have 4 promotion moves: queen, rook, bishop, knight
    assert(black_promotion.size() == 4);
    
    // Check that all promotions are to d1 with different promotion values
    bool found_black_queen = false, found_black_rook = false, found_black_bishop = false, found_black_knight = false;
    for (const Move& move : black_promotion) {
        assert(move.to == string_to_square("d1"));
        if (move.promotion == 5) found_black_queen = true;
        else if (move.promotion == 4) found_black_rook = true;
        else if (move.promotion == 3) found_black_bishop = true;
        else if (move.promotion == 2) found_black_knight = true;
    }
    assert(found_black_queen && found_black_rook && found_black_bishop && found_black_knight);
    
    // Test 3: White pawn captures and promotes
    Board board3;
    set_piece(board3, string_to_square("e7"), 1);  // white pawn on e7
    set_piece(board3, string_to_square("f8"), -4); // black rook on f8
    
    vector<Move> capture_promotion = generate_pawn_moves(board3, string_to_square("e7"));
    
    // Should have 8 moves: 4 forward promotions + 4 capture promotions
    assert(capture_promotion.size() == 8);
    
    // Check that we have moves to both e8 and f8
    int moves_to_e8 = 0, moves_to_f8 = 0;
    for (const Move& move : capture_promotion) {
        if (move.to == string_to_square("e8")) moves_to_e8++;
        else if (move.to == string_to_square("f8")) moves_to_f8++;
    }
    assert(moves_to_e8 == 4 && moves_to_f8 == 4);
    
    // Test 4: No promotion if not on 7th/2nd rank
    Board board4;
    set_piece(board4, string_to_square("e6"), 1);  // white pawn on e6 (not promoting)
    
    vector<Move> no_promotion = generate_pawn_moves(board4, string_to_square("e6"));
    
    // Should have 1 regular move with no promotion
    assert(no_promotion.size() == 1);
    assert(no_promotion[0].to == string_to_square("e7"));
    assert(no_promotion[0].promotion == 0); // No promotion
    
    cout << "✓ Pawn promotion move generation tests passed" << endl;
}

void test_check_detection() {
    cout << "Testing check detection..." << endl;
    
    // Test 1: King in check from rook
    Board board;
    set_piece(board, string_to_square("e1"), 6);  // white king on e1
    set_piece(board, string_to_square("e8"), -4); // black rook on e8
    
    assert(is_in_check(board, true));  // White king is in check
    assert(!is_in_check(board, false)); // Black king is not in check (no black king on board)
    
    // Test 2: King in check from bishop
    Board board2;
    set_piece(board2, string_to_square("e1"), 6);  // white king on e1
    set_piece(board2, string_to_square("a5"), -3); // black bishop on a5
    
    assert(is_in_check(board2, true));  // White king is in check from diagonal
    
    // Test 3: King in check from knight
    Board board3;
    set_piece(board3, string_to_square("e4"), 6);  // white king on e4
    set_piece(board3, string_to_square("d6"), -2); // black knight on d6
    
    assert(is_in_check(board3, true));  // White king is in check from knight
    
    // Test 4: King in check from pawn
    Board board4;
    set_piece(board4, string_to_square("e4"), 6);  // white king on e4
    set_piece(board4, string_to_square("d5"), -1); // black pawn on d5
    
    assert(is_in_check(board4, true));  // White king is in check from pawn
    
    // Test 5: King in check from queen
    Board board5;
    set_piece(board5, string_to_square("e1"), 6);  // white king on e1
    set_piece(board5, string_to_square("e8"), -5); // black queen on e8
    
    assert(is_in_check(board5, true));  // White king is in check from queen
    
    // Test 6: King NOT in check - pieces blocked
    Board board6;
    set_piece(board6, string_to_square("e1"), 6);  // white king on e1
    set_piece(board6, string_to_square("e8"), -4); // black rook on e8
    set_piece(board6, string_to_square("e4"), 1);  // white pawn blocks
    
    assert(!is_in_check(board6, true)); // White king is NOT in check (blocked)
    
    // Test 7: King NOT in check - no attacking pieces
    Board board7;
    set_piece(board7, string_to_square("e1"), 6);  // white king on e1
    set_piece(board7, string_to_square("a8"), -4); // black rook on a8 (no line of attack)
    
    assert(!is_in_check(board7, true)); // White king is NOT in check
    
    // Test 8: Black king in check
    Board board8;
    set_piece(board8, string_to_square("e8"), -6); // black king on e8
    set_piece(board8, string_to_square("e1"), 4);  // white rook on e1
    
    assert(is_in_check(board8, false)); // Black king is in check
    assert(!is_in_check(board8, true));  // White king is not in check (no white king)
    
    cout << "✓ Check detection tests passed" << endl;
}

void test_legal_move_validation() {
    cout << "Testing legal move validation..." << endl;
    
    // Test 1: Move that leaves king in check should be illegal
    Board board;
    set_piece(board, string_to_square("e1"), 6);  // white king on e1
    set_piece(board, string_to_square("e8"), -4); // black rook on e8
    set_piece(board, string_to_square("e2"), 4);  // white rook on e2 (blocking check)
    
    // Moving the rook OFF the e-file would expose the king to check
    Move illegal_move(string_to_square("e2"), string_to_square("d2"));
    assert(!is_legal_move(board, illegal_move));
    
    // Test 2: Move that doesn't expose king should be legal
    Board board2;
    set_piece(board2, string_to_square("e1"), 6);  // white king on e1
    set_piece(board2, string_to_square("d2"), 1);  // white pawn on d2
    
    // Moving this pawn doesn't expose the king
    Move legal_move(string_to_square("d2"), string_to_square("d3"));
    assert(is_legal_move(board2, legal_move));
    
    // Test 3: King move into check should be illegal
    Board board3;
    set_piece(board3, string_to_square("e1"), 6);  // white king on e1
    set_piece(board3, string_to_square("e8"), -4); // black rook on e8
    
    // King moving to e2 would be moving into check
    Move king_into_check(string_to_square("e1"), string_to_square("e2"));
    assert(!is_legal_move(board3, king_into_check));
    
    // Test 4: King move out of check should be legal
    Board board4;
    set_piece(board4, string_to_square("e1"), 6);  // white king on e1
    set_piece(board4, string_to_square("e8"), -4); // black rook on e8 (king in check)
    
    // King moving to d1 gets out of check
    Move king_escape(string_to_square("e1"), string_to_square("d1"));
    assert(is_legal_move(board4, king_escape));
    
    // Test 5: Capturing the attacking piece should be legal
    Board board5;
    set_piece(board5, string_to_square("e1"), 6);  // white king on e1
    set_piece(board5, string_to_square("e2"), -4); // black rook on e2 (giving check)
    
    // King capturing the rook should be legal
    Move capture_attacker(string_to_square("e1"), string_to_square("e2"));
    assert(is_legal_move(board5, capture_attacker));
    
    // Test 6: Blocking check should be legal
    Board board6;
    set_piece(board6, string_to_square("e1"), 6);  // white king on e1
    set_piece(board6, string_to_square("e8"), -4); // black rook on e8 (giving check)
    set_piece(board6, string_to_square("d3"), 1);  // white pawn on d3
    
    // Pawn moving to e4 blocks the check
    Move block_check(string_to_square("d3"), string_to_square("e4"));
    assert(is_legal_move(board6, block_check));
    
    // Test 7: En passant that exposes king should be illegal
    Board board7;
    set_piece(board7, string_to_square("e5"), 6);  // white king on e5
    set_piece(board7, string_to_square("d5"), 1);  // white pawn on d5
    set_piece(board7, string_to_square("c5"), -1); // black pawn on c5
    set_piece(board7, string_to_square("a5"), -4); // black rook on a5
    board7.en_passant_square = string_to_square("c6"); // En passant available
    
    // En passant capture would remove the pawn blocking the rook's attack
    Move en_passant_expose(string_to_square("d5"), string_to_square("c6"));
    assert(!is_legal_move(board7, en_passant_expose));
    
    // Test 8: Castling through check should be illegal
    Board board8;
    set_piece(board8, string_to_square("e1"), 6);  // white king on e1
    set_piece(board8, string_to_square("h1"), 4);  // white rook on h1
    set_piece(board8, string_to_square("f8"), -4); // black rook on f8 (attacks f1)
    board8.white_can_castle_kingside = true;
    
    // Castling would move king through attacked f1 square
    Move castle_through_check(string_to_square("e1"), string_to_square("g1"));
    assert(!is_legal_move(board8, castle_through_check));
    
    cout << "✓ Legal move validation tests passed" << endl;
}

// FEN parsing (test-only)
Board parse_fen(const string& fen) {
    Board board;
    istringstream iss(fen);
    string pieces, turn, castling, en_passant, halfmove, fullmove;
    
    iss >> pieces >> turn >> castling >> en_passant >> halfmove >> fullmove;
    
    // Parse piece placement
    int square = 56; // Start at a8
    for (char c : pieces) {
        if (c == '/') {
            square -= 16; // Move to next rank down
        } else if (isdigit(c)) {
            square += (c - '0'); // Skip empty squares
        } else {
            // Place piece
            int piece = 0;
            switch (tolower(c)) {
                case 'p': piece = 1; break;
                case 'n': piece = 2; break;
                case 'b': piece = 3; break;
                case 'r': piece = 4; break;
                case 'q': piece = 5; break;
                case 'k': piece = 6; break;
            }
            if (islower(c)) piece = -piece; // Black piece
            set_piece(board, square, piece);
            square++;
        }
    }
    
    // Parse turn
    board.white_to_move = (turn == "w");
    
    // Parse castling rights
    board.white_can_castle_kingside = castling.find('K') != string::npos;
    board.white_can_castle_queenside = castling.find('Q') != string::npos;
    board.black_can_castle_kingside = castling.find('k') != string::npos;
    board.black_can_castle_queenside = castling.find('q') != string::npos;
    
    // Parse en passant
    if (en_passant != "-") {
        board.en_passant_square = string_to_square(en_passant);
    } else {
        board.en_passant_square = -1;
    }
    
    return board;
}


// Perft function
long long perft(const Board& board, int depth) {
    if (depth == 0) return 1;
    
    vector<Move> moves = generate_all_legal_moves(board);
    long long count = 0;
    
    for (const Move& move : moves) {
        Board temp_board = board;
        make_move_simple(temp_board, move);
        count += perft(temp_board, depth - 1);
    }
    
    return count;
}

struct PerftResult {
    string fen;
    vector<long long> depths;
};

PerftResult parse_perft_line(const string& line) {
    PerftResult result;
    
    // Find FEN part (everything before first semicolon)
    size_t semicolon_pos = line.find(';');
    if (semicolon_pos == string::npos) return result;
    
    result.fen = line.substr(0, semicolon_pos);
    
    // Parse depth results
    string remaining = line.substr(semicolon_pos + 1);
    istringstream iss(remaining);
    string token;
    
    while (iss >> token) {
        if (token.substr(0, 2) == ";D") {
            // Extract depth number
            int depth = stoi(token.substr(2));
            
            // Get the count
            long long count;
            iss >> count;
            
            // Ensure we have enough space in vector
            while (result.depths.size() <= depth) {
                result.depths.push_back(0);
            }
            result.depths[depth] = count;
        }
    }
    
    return result;
}

void test_uci_position_parsing() {
    cout << "Testing UCI position parsing..." << endl;
    
    // Test 1: Starting position
    Board board1 = parse_uci_position("position startpos");
    Board expected1 = create_starting_position();
    
    // Compare boards
    for (int i = 0; i < 64; i++) {
        assert(board1.squares[i] == expected1.squares[i]);
    }
    assert(board1.white_to_move == expected1.white_to_move);
    assert(board1.white_can_castle_kingside == expected1.white_can_castle_kingside);
    assert(board1.white_can_castle_queenside == expected1.white_can_castle_queenside);
    assert(board1.black_can_castle_kingside == expected1.black_can_castle_kingside);
    assert(board1.black_can_castle_queenside == expected1.black_can_castle_queenside);
    
    // Test 2: Starting position with moves
    Board board2 = parse_uci_position("position startpos moves e2e4 e7e5");
    
    // Expected: e2-e4, e7-e5 moves made
    assert(get_piece(board2, string_to_square("e2")) == 0);  // pawn moved from e2
    assert(get_piece(board2, string_to_square("e4")) == 1);  // white pawn on e4
    assert(get_piece(board2, string_to_square("e7")) == 0);  // pawn moved from e7
    assert(get_piece(board2, string_to_square("e5")) == -1); // black pawn on e5
    assert(board2.white_to_move == true); // white to move after black's move
    
    // Test 3: Castling move
    Board board3 = parse_uci_position("position startpos moves e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 e1g1");
    
    // Should have castled king and rook
    assert(get_piece(board3, string_to_square("e1")) == 0);  // king moved
    assert(get_piece(board3, string_to_square("g1")) == 6);  // king on g1
    assert(get_piece(board3, string_to_square("h1")) == 0);  // rook moved
    assert(get_piece(board3, string_to_square("f1")) == 4);  // rook on f1
    
    cout << "✓ UCI position parsing tests passed" << endl;
}

void test_uci_move_format() {
    cout << "Testing UCI move format conversion..." << endl;
    
    // Test 1: Simple move
    Move move1(string_to_square("e2"), string_to_square("e4"));
    assert(move_to_uci(move1) == "e2e4");
    
    Move parsed1 = uci_to_move("e2e4");
    assert(parsed1.from == string_to_square("e2"));
    assert(parsed1.to == string_to_square("e4"));
    assert(parsed1.promotion == 0);
    
    // Test 2: Promotion move
    Move move2(string_to_square("e7"), string_to_square("e8"), 5); // queen promotion
    assert(move_to_uci(move2) == "e7e8q");
    
    Move parsed2 = uci_to_move("e7e8q");
    assert(parsed2.from == string_to_square("e7"));
    assert(parsed2.to == string_to_square("e8"));
    assert(parsed2.promotion == 5);
    
    // Test 3: Knight promotion
    Move move3(string_to_square("a7"), string_to_square("b8"), 2); // knight promotion
    assert(move_to_uci(move3) == "a7b8n");
    
    Move parsed3 = uci_to_move("a7b8n");
    assert(parsed3.from == string_to_square("a7"));
    assert(parsed3.to == string_to_square("b8"));
    assert(parsed3.promotion == 2);
    
    // Test 4: Castling moves
    Move move4(string_to_square("e1"), string_to_square("g1")); // kingside castling
    assert(move_to_uci(move4) == "e1g1");
    
    Move parsed4 = uci_to_move("e1g1");
    assert(parsed4.from == string_to_square("e1"));
    assert(parsed4.to == string_to_square("g1"));
    assert(parsed4.promotion == 0);
    
    cout << "✓ UCI move format conversion tests passed" << endl;
}

void test_perft() {
    cout << "Testing perft (comprehensive move generation validation)..." << endl;
    
    // Try to read the full perft suite
    ifstream file("src/perftsuite.epd");
    if (!file.is_open()) {
        cout << "Warning: Could not open src/perftsuite.epd, running basic tests only" << endl;
        
        // Fallback to basic tests
        string start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        Board board = parse_fen(start_fen);
        assert(perft(board, 1) == 20);
        assert(perft(board, 2) == 400);
        assert(perft(board, 3) == 8902);
        cout << "✓ Basic perft tests passed" << endl;
        return;
    }
    
    int passed = 0, failed = 0;
    int position_count = 0;
    string line;
    
    cout << "Loading full perft suite..." << endl;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        PerftResult test = parse_perft_line(line);
        if (test.fen.empty()) continue;
        
        position_count++;
        Board board = parse_fen(test.fen);
        
        cout << "Position " << position_count << ": ";
        
        // Test depths 1-3 (depth 4+ would be too slow for comprehensive testing)
        for (int depth = 1; depth <= 3 && depth < test.depths.size(); depth++) {
            if (test.depths[depth] == 0) continue;
            
            long long result = perft(board, depth);
            if (result == test.depths[depth]) {
                cout << "D" << depth << ":✓ ";
                passed++;
            } else {
                cout << "D" << depth << ":✗(" << result << "!=" << test.depths[depth] << ") ";
                failed++;
                
                // Show failing position for debugging
                if (failed <= 5) {
                    cout << endl << "  FAILED FEN: " << test.fen << endl;
                    cout << "  Expected D" << depth << ": " << test.depths[depth] << ", Got: " << result << endl;
                }
            }
        }
        cout << endl;
        
        // Stop after first few failures for debugging
        if (failed > 3) {
            cout << "Stopping after multiple failures for debugging..." << endl;
            break;
        }
    }
    
    file.close();
    
    cout << "Perft comprehensive suite results: " << passed << " passed, " << failed << " failed across " << position_count << " positions" << endl;
    
    if (failed > 0) {
        cout << "✗ Some perft tests failed!" << endl;
        assert(false);
    } else {
        cout << "✓ All " << position_count << " positions passed - move generation is bulletproof!" << endl;
    }
}

int main() {
    cout << "=== Chess Engine TDD Tests ===" << endl;
    
    test_square_utilities();
    test_board_initialization();
    test_board_manipulation();
    test_pawn_moves();
    test_knight_moves();
    test_bishop_moves();
    test_rook_moves();
    test_queen_moves();
    test_king_moves();
    test_en_passant_moves();
    test_castling_moves();
    test_pawn_promotion();
    test_check_detection();
    test_legal_move_validation();
    test_uci_position_parsing();
    test_uci_move_format();
    test_perft();
    
    cout << "\n✓ All tests passed!" << endl;
    return 0;
}