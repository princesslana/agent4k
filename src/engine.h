#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <climits>
#include <chrono>

using namespace std;

// Constants
const int EMPTY = 0;
const int PAWN = 1, KNIGHT = 2, BISHOP = 3, ROOK = 4, QUEEN = 5, KING = 6;
const int WHITE = 1, BLACK = -1;

// Global game state
extern int b[64];
extern int s;
extern int castling_rights;
extern int en_passant_square;
extern chrono::steady_clock::time_point start_time;
extern int time_limit_ms;

// Core structures
struct Move {
    int f, t;
    int p, capture;
    int promotion = 0;
    Move(int f, int t, int p, int c = 0) : f(f), t(t), p(p), capture(c) {}
};

struct Position {
    int board[64];
    int side;
    int castling_rights;
    int en_passant_square;
};

// Core functions
bool time_up();
int square_from_string(const string& sq);
string square_to_string(int sq);
void init_board();
Position get_position();
void set_position(const Position& pos);
Position apply_move(const Position& pos, const Move& m);
bool is_square_attacked(int square, int attacking_side);
bool is_in_check();
bool is_legal_move(const Move& m);
vector<Move> generate_moves();
int evaluate();
int search(int d, int a, int B);
Move find_best_move(int d);
void parse_position(const string& command);