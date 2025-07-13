#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <climits>
#include <chrono>

using namespace std;

// Piece constants
const int EMPTY = 0;
const int PAWN = 1, KNIGHT = 2, BISHOP = 3, ROOK = 4, QUEEN = 5, KING = 6;
const int WHITE = 1, BLACK = -1;

// Board representation (8x8 = 64 squares)
int board[64];
int side_to_move = WHITE;
int castling_rights = 0; // KQkq bits
int en_passant_square = -1;

// Time management
auto start_time = chrono::steady_clock::now();
int time_limit_ms = 1000; // Default 1 second

// Check if we're running out of time
bool time_up() {
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - start_time).count();
    return elapsed >= time_limit_ms;
}

// Convert algebraic notation to square index (a1=0, h8=63)
int square_from_string(const string& sq) {
    if (sq.length() != 2) return -1;
    int file = sq[0] - 'a';
    int rank = sq[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return rank * 8 + file;
}

// Convert square index to algebraic notation
string square_to_string(int sq) {
    if (sq < 0 || sq > 63) return "";
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return string(1, file) + string(1, rank);
}

// Initialize board to starting position
void init_board() {
    // Clear board
    for (int i = 0; i < 64; i++) {
        board[i] = EMPTY;
    }
    
    // White pieces
    board[0] = WHITE * ROOK;   board[1] = WHITE * KNIGHT; board[2] = WHITE * BISHOP; board[3] = WHITE * QUEEN;
    board[4] = WHITE * KING;   board[5] = WHITE * BISHOP; board[6] = WHITE * KNIGHT; board[7] = WHITE * ROOK;
    for (int i = 8; i < 16; i++) board[i] = WHITE * PAWN;
    
    // Black pieces
    for (int i = 48; i < 56; i++) board[i] = BLACK * PAWN;
    board[56] = BLACK * ROOK; board[57] = BLACK * KNIGHT; board[58] = BLACK * BISHOP; board[59] = BLACK * QUEEN;
    board[60] = BLACK * KING; board[61] = BLACK * BISHOP; board[62] = BLACK * KNIGHT; board[63] = BLACK * ROOK;
    
    side_to_move = WHITE;
    castling_rights = 15; // KQkq all allowed initially
    en_passant_square = -1;
}

// Simple move structure
struct Move {
    int from, to;
    int piece, capture;
    int promotion = 0;
    
    Move(int f, int t, int p, int c = 0) : from(f), to(t), piece(p), capture(c) {}
};

// Check if a square is attacked by the given side
bool is_square_attacked(int square, int attacking_side) {
    for (int from = 0; from < 64; from++) {
        int piece = board[from];
        if (piece == EMPTY || (piece > 0) != (attacking_side > 0)) continue;
        
        int piece_type = abs(piece);
        int piece_color = piece > 0 ? WHITE : BLACK;
        
        if (piece_type == PAWN) {
            int direction = piece_color == WHITE ? 8 : -8;
            for (int file_offset : {-1, 1}) {
                if ((from % 8 == 0 && file_offset == -1) || (from % 8 == 7 && file_offset == 1)) continue;
                int to = from + direction + file_offset;
                if (to == square) return true;
            }
        } else if (piece_type == KNIGHT) {
            int knight_moves[] = {-17, -15, -10, -6, 6, 10, 15, 17};
            for (int delta : knight_moves) {
                int to = from + delta;
                if (to < 0 || to > 63) continue;
                int from_file = from % 8, to_file = to % 8;
                if (abs(from_file - to_file) > 2) continue;
                if (to == square) return true;
            }
        } else if (piece_type == KING) {
            int king_moves[] = {-9, -8, -7, -1, 1, 7, 8, 9};
            for (int delta : king_moves) {
                int to = from + delta;
                if (to < 0 || to > 63) continue;
                int from_file = from % 8, to_file = to % 8;
                if (abs(from_file - to_file) > 1) continue;
                if (to == square) return true;
            }
        } else {
            // Sliding pieces
            vector<int> directions;
            if (piece_type == BISHOP || piece_type == QUEEN) {
                directions.insert(directions.end(), {-9, -7, 7, 9});
            }
            if (piece_type == ROOK || piece_type == QUEEN) {
                directions.insert(directions.end(), {-8, -1, 1, 8});
            }
            
            for (int delta : directions) {
                for (int to = from + delta; to >= 0 && to < 64; to += delta) {
                    if (abs(delta) != 8) {
                        int from_file = (to - delta) % 8, to_file = to % 8;
                        if (abs(from_file - to_file) > 1) break;
                    }
                    
                    if (to == square) return true;
                    if (board[to] != EMPTY) break; // Blocked
                }
            }
        }
    }
    return false;
}

// Check if current side is in check
bool is_in_check() {
    // Find king
    int king_square = -1;
    for (int sq = 0; sq < 64; sq++) {
        if (board[sq] == side_to_move * KING) {
            king_square = sq;
            break;
        }
    }
    
    if (king_square == -1) return false; // No king found
    return is_square_attacked(king_square, -side_to_move);
}

// Check if a move is legal (doesn't leave own king in check)
bool is_legal_move(const Move& move) {
    // Make the move
    int original_piece = board[move.to];
    board[move.from] = EMPTY;
    board[move.to] = move.promotion ? move.promotion : move.piece;
    side_to_move = -side_to_move;
    
    // Check if own king is in check
    bool legal = !is_in_check();
    
    // Unmake the move
    side_to_move = -side_to_move;
    board[move.from] = move.piece;
    board[move.to] = original_piece;
    
    return legal;
}

// Generate all legal moves for current position
vector<Move> generate_moves() {
    vector<Move> pseudo_moves;
    
    for (int from = 0; from < 64; from++) {
        int piece = board[from];
        if (piece == EMPTY || (piece > 0) != (side_to_move > 0)) continue;
        
        int piece_type = abs(piece);
        int piece_color = piece > 0 ? WHITE : BLACK;
        
        if (piece_type == PAWN) {
            int direction = piece_color == WHITE ? 8 : -8;
            int start_rank = piece_color == WHITE ? 1 : 6;
            int promo_rank = piece_color == WHITE ? 7 : 0;
            
            // Forward moves
            int to = from + direction;
            if (to >= 0 && to < 64 && board[to] == EMPTY) {
                if (to / 8 == promo_rank) {
                    pseudo_moves.push_back(Move(from, to, piece, 0));
                    pseudo_moves.back().promotion = piece_color * QUEEN;
                } else {
                    pseudo_moves.push_back(Move(from, to, piece));
                    
                    // Double push from starting rank
                    if (from / 8 == start_rank) {
                        to = from + 2 * direction;
                        if (to >= 0 && to < 64 && board[to] == EMPTY) {
                            pseudo_moves.push_back(Move(from, to, piece));
                        }
                    }
                }
            }
            
            // Captures
            for (int file_offset : {-1, 1}) {
                if ((from % 8 == 0 && file_offset == -1) || (from % 8 == 7 && file_offset == 1)) continue;
                to = from + direction + file_offset;
                if (to >= 0 && to < 64) {
                    int target = board[to];
                    if (target != EMPTY && (target > 0) != (piece_color > 0)) {
                        if (to / 8 == promo_rank) {
                            pseudo_moves.push_back(Move(from, to, piece, target));
                            pseudo_moves.back().promotion = piece_color * QUEEN;
                        } else {
                            pseudo_moves.push_back(Move(from, to, piece, target));
                        }
                    }
                }
            }
        } else if (piece_type == KNIGHT) {
            int knight_moves[] = {-17, -15, -10, -6, 6, 10, 15, 17};
            for (int delta : knight_moves) {
                int to = from + delta;
                if (to < 0 || to > 63) continue;
                // Check if move crosses board edge
                int from_file = from % 8, to_file = to % 8;
                if (abs(from_file - to_file) > 2) continue;
                
                int target = board[to];
                if (target == EMPTY || (target > 0) != (piece_color > 0)) {
                    pseudo_moves.push_back(Move(from, to, piece, target));
                }
            }
        } else if (piece_type == KING) {
            int king_moves[] = {-9, -8, -7, -1, 1, 7, 8, 9};
            for (int delta : king_moves) {
                int to = from + delta;
                if (to < 0 || to > 63) continue;
                // Check if move crosses board edge
                int from_file = from % 8, to_file = to % 8;
                if (abs(from_file - to_file) > 1) continue;
                
                int target = board[to];
                if (target == EMPTY || (target > 0) != (piece_color > 0)) {
                    pseudo_moves.push_back(Move(from, to, piece, target));
                }
            }
        } else {
            // Sliding pieces (BISHOP, ROOK, QUEEN)
            vector<int> directions;
            if (piece_type == BISHOP || piece_type == QUEEN) {
                directions.insert(directions.end(), {-9, -7, 7, 9});
            }
            if (piece_type == ROOK || piece_type == QUEEN) {
                directions.insert(directions.end(), {-8, -1, 1, 8});
            }
            
            for (int delta : directions) {
                for (int to = from + delta; to >= 0 && to < 64; to += delta) {
                    // Check if move crosses board edge
                    if (abs(delta) != 8) {
                        int from_file = (to - delta) % 8, to_file = to % 8;
                        if (abs(from_file - to_file) > 1) break;
                    }
                    
                    int target = board[to];
                    if (target == EMPTY) {
                        pseudo_moves.push_back(Move(from, to, piece));
                    } else {
                        if ((target > 0) != (piece_color > 0)) {
                            pseudo_moves.push_back(Move(from, to, piece, target));
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // Filter to only legal moves
    vector<Move> legal_moves;
    for (const Move& move : pseudo_moves) {
        if (is_legal_move(move)) {
            legal_moves.push_back(move);
        }
    }
    
    return legal_moves;
}

// Make a move on the board
void make_move(const Move& move) {
    board[move.from] = EMPTY;
    board[move.to] = move.promotion ? move.promotion : move.piece;
    side_to_move = -side_to_move;
}

// Unmake a move
void unmake_move(const Move& move) {
    board[move.from] = move.piece;
    board[move.to] = move.capture;
    side_to_move = -side_to_move;
}

// Simple material evaluation
int evaluate() {
    int material = 0;
    int piece_values[] = {0, 100, 320, 330, 500, 900, 0}; // P, N, B, R, Q, K
    
    for (int sq = 0; sq < 64; sq++) {
        int piece = board[sq];
        if (piece != EMPTY) {
            int value = piece_values[abs(piece)];
            material += piece > 0 ? value : -value;
        }
    }
    
    return side_to_move == WHITE ? material : -material;
}

// Alpha-beta search
int search(int depth, int alpha, int beta) {
    if (depth == 0 || time_up()) {
        return evaluate();
    }
    
    vector<Move> moves = generate_moves();
    if (moves.empty()) {
        if (is_in_check()) {
            return -30000 + depth; // Checkmate (prefer faster mate)
        } else {
            return 0; // Stalemate
        }
    }
    
    int best_score = -40000;
    for (const Move& move : moves) {
        make_move(move);
        int score = -search(depth - 1, -beta, -alpha);
        unmake_move(move);
        
        best_score = max(best_score, score);
        alpha = max(alpha, score);
        if (beta <= alpha) {
            break; // Beta cutoff
        }
    }
    
    return best_score;
}

// Find best move
Move find_best_move(int depth) {
    vector<Move> moves = generate_moves();
    if (moves.empty()) {
        return Move(-1, -1, 0); // No legal moves
    }
    
    Move best_move = moves[0];
    int best_score = -40000;
    
    for (const Move& move : moves) {
        make_move(move);
        int score = -search(depth - 1, -40000, 40000);
        unmake_move(move);
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    return best_move;
}

// Parse UCI position command
void parse_position(const string& command) {
    if (command.find("startpos") != string::npos) {
        init_board();
        
        // Look for moves
        size_t moves_pos = command.find("moves");
        if (moves_pos != string::npos) {
            string moves_str = command.substr(moves_pos + 5);
            istringstream iss(moves_str);
            string move_str;
            
            while (iss >> move_str) {
                if (move_str.length() >= 4) {
                    int from = square_from_string(move_str.substr(0, 2));
                    int to = square_from_string(move_str.substr(2, 2));
                    
                    if (from >= 0 && to >= 0) {
                        Move move(from, to, board[from], board[to]);
                        if (move_str.length() == 5) {
                            // Promotion
                            char promo = move_str[4];
                            int promo_piece = 0;
                            switch (promo) {
                                case 'q': promo_piece = side_to_move * QUEEN; break;
                                case 'r': promo_piece = side_to_move * ROOK; break;
                                case 'b': promo_piece = side_to_move * BISHOP; break;
                                case 'n': promo_piece = side_to_move * KNIGHT; break;
                            }
                            move.promotion = promo_piece;
                        }
                        make_move(move);
                    }
                }
            }
        }
    }
}

// Main UCI loop
int main() {
    string line;
    
    while (getline(cin, line)) {
        istringstream iss(line);
        string command;
        iss >> command;
        
        if (command == "uci") {
            cout << "id name Agent4k" << endl;
            cout << "id author Claude" << endl;
            cout << "uciok" << endl;
        }
        else if (command == "isready") {
            cout << "readyok" << endl;
        }
        else if (command == "position") {
            parse_position(line);
        }
        else if (command == "go") {
            // Parse time parameters
            time_limit_ms = 1000; // Default 1 second
            istringstream go_iss(line);
            string token;
            while (go_iss >> token) {
                if (token == "wtime" && side_to_move == WHITE) {
                    go_iss >> token;
                    time_limit_ms = stoi(token) / 20; // Use 1/20th of remaining time
                } else if (token == "btime" && side_to_move == BLACK) {
                    go_iss >> token;
                    time_limit_ms = stoi(token) / 20; // Use 1/20th of remaining time
                } else if (token == "movetime") {
                    go_iss >> token;
                    time_limit_ms = stoi(token);
                }
            }
            
            start_time = chrono::steady_clock::now();
            Move best = find_best_move(4); // Search depth 4
            if (best.from >= 0) {
                string move_str = square_to_string(best.from) + square_to_string(best.to);
                if (best.promotion) {
                    char promo_char = 'q'; // Always promote to queen for simplicity
                    switch (abs(best.promotion)) {
                        case QUEEN: promo_char = 'q'; break;
                        case ROOK: promo_char = 'r'; break;
                        case BISHOP: promo_char = 'b'; break;
                        case KNIGHT: promo_char = 'n'; break;
                    }
                    move_str += promo_char;
                }
                cout << "bestmove " << move_str << endl;
            } else {
                cout << "bestmove 0000" << endl;
            }
        }
        else if (command == "quit") {
            break;
        }
    }
    
    return 0;
}