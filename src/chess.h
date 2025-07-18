#pragma once
#include <vector>
#include <string>
using namespace std;

// Piece values: 0=empty, 1=pawn, 2=knight, 3=bishop, 4=rook, 5=queen, 6=king
// Positive = white, negative = black
struct Board {
    int squares[64];
    bool white_to_move;
    int en_passant_square; // -1 if no en passant possible, otherwise the target square
    bool white_can_castle_kingside;
    bool white_can_castle_queenside;
    bool black_can_castle_kingside;
    bool black_can_castle_queenside;
    
    Board();
};

struct Move {
    int from;
    int to;
    int promotion; // 0=no promotion, 2=knight, 3=bishop, 4=rook, 5=queen
    
    Move(int f, int t, int p = 0);
};

// Basic board functions
bool is_valid_square(int square);
string square_to_string(int square);
int string_to_square(const string& str);
Board create_starting_position();
int get_piece(const Board& board, int square);
void set_piece(Board& board, int square, int piece);
bool is_white_to_move(const Board& board);

// Move generation functions
vector<Move> generate_pawn_moves(const Board& board, int square);
vector<Move> generate_knight_moves(const Board& board, int square);
vector<Move> generate_bishop_moves(const Board& board, int square);
vector<Move> generate_rook_moves(const Board& board, int square);
vector<Move> generate_queen_moves(const Board& board, int square);
vector<Move> generate_king_moves(const Board& board, int square);
vector<Move> generate_all_moves(const Board& board);

// Check detection functions
bool is_in_check(const Board& board, bool white_king);

// Move execution functions
void make_move_simple(Board& board, const Move& move);

// Legal move validation
bool is_legal_move(const Board& board, const Move& move);

// UCI interface functions
string move_to_uci(const Move& move);
Move uci_to_move(const string& uci_str);
Board parse_uci_position(const string& position_command);
vector<Move> generate_all_legal_moves(const Board& board);

// Evaluation and search functions
int evaluate_position(const Board& board);
Move search_best_move(const Board& board, int depth);