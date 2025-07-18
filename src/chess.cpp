#include "chess.h"
#include <sstream>
#include <cctype>

Board::Board() : white_to_move(true), en_passant_square(-1), 
                 white_can_castle_kingside(false), white_can_castle_queenside(false),
                 black_can_castle_kingside(false), black_can_castle_queenside(false) {
    for (int i = 0; i < 64; i++) {
        squares[i] = 0;
    }
}

Move::Move(int f, int t, int p) : from(f), to(t), promotion(p) {}

bool is_valid_square(int square) {
    return square >= 0 && square < 64;
}

string square_to_string(int square) {
    if (!is_valid_square(square)) return "";
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);
    return string(1, file) + string(1, rank);
}

int string_to_square(const string& str) {
    if (str.length() != 2) return -1;
    int file = str[0] - 'a';
    int rank = str[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return rank * 8 + file;
}

Board create_starting_position() {
    Board board;
    
    // White pieces
    board.squares[0] = 4;   // rook
    board.squares[1] = 2;   // knight
    board.squares[2] = 3;   // bishop
    board.squares[3] = 5;   // queen
    board.squares[4] = 6;   // king
    board.squares[5] = 3;   // bishop
    board.squares[6] = 2;   // knight
    board.squares[7] = 4;   // rook
    
    // White pawns
    for (int i = 8; i < 16; i++) {
        board.squares[i] = 1;
    }
    
    // Empty squares (already 0 from constructor)
    
    // Black pawns
    for (int i = 48; i < 56; i++) {
        board.squares[i] = -1;
    }
    
    // Black pieces
    board.squares[56] = -4;  // rook
    board.squares[57] = -2;  // knight
    board.squares[58] = -3;  // bishop
    board.squares[59] = -5;  // queen
    board.squares[60] = -6;  // king
    board.squares[61] = -3;  // bishop
    board.squares[62] = -2;  // knight
    board.squares[63] = -4;  // rook
    
    board.white_to_move = true;
    board.en_passant_square = -1;
    board.white_can_castle_kingside = true;
    board.white_can_castle_queenside = true;
    board.black_can_castle_kingside = true;
    board.black_can_castle_queenside = true;
    return board;
}

int get_piece(const Board& board, int square) {
    if (!is_valid_square(square)) return 0;
    return board.squares[square];
}

void set_piece(Board& board, int square, int piece) {
    if (is_valid_square(square)) {
        board.squares[square] = piece;
    }
}

bool is_white_to_move(const Board& board) {
    return board.white_to_move;
}

vector<Move> generate_pawn_moves(const Board& board, int square) {
    vector<Move> moves;
    
    int piece = get_piece(board, square);
    if (abs(piece) != 1) return moves; // Not a pawn
    
    bool is_white = piece > 0;
    int direction = is_white ? 8 : -8;  // White moves up (+8), black moves down (-8)
    int start_rank = is_white ? 1 : 6;  // Starting ranks for pawns
    
    // Forward move
    int forward_square = square + direction;
    if (is_valid_square(forward_square) && get_piece(board, forward_square) == 0) {
        // Check if this is a promotion move
        int target_rank = forward_square / 8;
        int promotion_rank = is_white ? 7 : 0; // 8th rank for white, 1st rank for black
        
        if (target_rank == promotion_rank) {
            // Generate all 4 promotion moves
            moves.push_back(Move(square, forward_square, 5)); // Queen
            moves.push_back(Move(square, forward_square, 4)); // Rook
            moves.push_back(Move(square, forward_square, 3)); // Bishop
            moves.push_back(Move(square, forward_square, 2)); // Knight
        } else {
            moves.push_back(Move(square, forward_square));
            
            // Double move from starting position
            if (square / 8 == start_rank) {
                int double_square = square + 2 * direction;
                if (is_valid_square(double_square) && get_piece(board, double_square) == 0) {
                    moves.push_back(Move(square, double_square));
                }
            }
        }
    }
    
    // Captures
    for (int file_offset = -1; file_offset <= 1; file_offset += 2) {
        int capture_square = square + direction + file_offset;
        if (is_valid_square(capture_square)) {
            // Check if we're not wrapping around the board
            if (abs((capture_square % 8) - (square % 8)) == 1) {
                int target_piece = get_piece(board, capture_square);
                if (target_piece != 0 && (target_piece > 0) != is_white) {
                    // Check if this is a promotion capture
                    int target_rank = capture_square / 8;
                    int promotion_rank = is_white ? 7 : 0; // 8th rank for white, 1st rank for black
                    
                    if (target_rank == promotion_rank) {
                        // Generate all 4 promotion capture moves
                        moves.push_back(Move(square, capture_square, 5)); // Queen
                        moves.push_back(Move(square, capture_square, 4)); // Rook
                        moves.push_back(Move(square, capture_square, 3)); // Bishop
                        moves.push_back(Move(square, capture_square, 2)); // Knight
                    } else {
                        moves.push_back(Move(square, capture_square));
                    }
                }
            }
        }
    }
    
    // En passant capture
    if (board.en_passant_square != -1) {
        // Check if we're on the correct rank for en passant
        int current_rank = square / 8;
        int en_passant_rank = is_white ? 4 : 3; // 5th rank for white, 4th rank for black
        
        if (current_rank == en_passant_rank) {
            // Check if the en passant square is diagonally adjacent
            int file_diff = abs((board.en_passant_square % 8) - (square % 8));
            int rank_diff = abs((board.en_passant_square / 8) - (square / 8));
            
            if (file_diff == 1 && rank_diff == 1) {
                moves.push_back(Move(square, board.en_passant_square));
            }
        }
    }
    
    return moves;
}

vector<Move> generate_knight_moves(const Board& board, int square) {
    vector<Move> moves;
    
    int piece = get_piece(board, square);
    if (abs(piece) != 2) return moves; // Not a knight
    
    bool is_white = piece > 0;
    
    // Knight moves: L-shaped moves (2 squares in one direction, 1 in perpendicular)
    int knight_offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    
    for (int i = 0; i < 8; i++) {
        int target_square = square + knight_offsets[i];
        
        if (is_valid_square(target_square)) {
            // Check if the move doesn't wrap around the board
            int from_file = square % 8;
            int from_rank = square / 8;
            int to_file = target_square % 8;
            int to_rank = target_square / 8;
            
            // Knight moves are valid if the file and rank changes are correct
            int file_diff = abs(to_file - from_file);
            int rank_diff = abs(to_rank - from_rank);
            
            if ((file_diff == 2 && rank_diff == 1) || (file_diff == 1 && rank_diff == 2)) {
                int target_piece = get_piece(board, target_square);
                
                // Can move to empty square or capture opponent piece
                if (target_piece == 0 || (target_piece > 0) != is_white) {
                    moves.push_back(Move(square, target_square));
                }
            }
        }
    }
    
    return moves;
}

vector<Move> generate_bishop_moves(const Board& board, int square) {
    vector<Move> moves;
    
    int piece = get_piece(board, square);
    if (abs(piece) != 3) return moves; // Not a bishop
    
    bool is_white = piece > 0;
    
    // Bishop moves diagonally: 4 diagonal directions
    int diagonal_offsets[] = {-9, -7, 7, 9}; // down-left, down-right, up-left, up-right
    
    for (int dir = 0; dir < 4; dir++) {
        int offset = diagonal_offsets[dir];
        
        for (int i = 1; i < 8; i++) {
            int target_square = square + offset * i;
            
            if (!is_valid_square(target_square)) break;
            
            // Check if we've wrapped around the board
            int from_file = square % 8;
            int to_file = target_square % 8;
            int from_rank = square / 8;
            int to_rank = target_square / 8;
            int file_diff = abs(to_file - from_file);
            int rank_diff = abs(to_rank - from_rank);
            
            if (file_diff != i || rank_diff != i) break; // We've wrapped around or invalid diagonal
            
            int target_piece = get_piece(board, target_square);
            
            if (target_piece == 0) {
                // Empty square - can move here
                moves.push_back(Move(square, target_square));
            } else if ((target_piece > 0) != is_white) {
                // Opponent piece - can capture
                moves.push_back(Move(square, target_square));
                break; // Can't move beyond this piece
            } else {
                // Own piece - blocked
                break;
            }
        }
    }
    
    return moves;
}

vector<Move> generate_rook_moves(const Board& board, int square) {
    vector<Move> moves;
    
    int piece = get_piece(board, square);
    if (abs(piece) != 4) return moves; // Not a rook
    
    bool is_white = piece > 0;
    
    // Rook moves horizontally and vertically: 4 directions
    int rook_offsets[] = {-8, -1, 1, 8}; // up, left, right, down
    
    for (int dir = 0; dir < 4; dir++) {
        int offset = rook_offsets[dir];
        
        for (int i = 1; i < 8; i++) {
            int target_square = square + offset * i;
            
            if (!is_valid_square(target_square)) break;
            
            // Check if we've wrapped around the board for horizontal moves
            if (offset == -1 || offset == 1) {
                int from_rank = square / 8;
                int to_rank = target_square / 8;
                if (from_rank != to_rank) break; // Wrapped around ranks
            }
            
            int target_piece = get_piece(board, target_square);
            
            if (target_piece == 0) {
                // Empty square - can move here
                moves.push_back(Move(square, target_square));
            } else if ((target_piece > 0) != is_white) {
                // Opponent piece - can capture
                moves.push_back(Move(square, target_square));
                break; // Can't move beyond this piece
            } else {
                // Own piece - blocked
                break;
            }
        }
    }
    
    return moves;
}

vector<Move> generate_queen_moves(const Board& board, int square) {
    vector<Move> moves;
    
    int piece = get_piece(board, square);
    if (abs(piece) != 5) return moves; // Not a queen
    
    // Queen moves like both rook and bishop - temporarily change the piece type
    Board temp_board = board;
    bool is_white = piece > 0;
    int rook_piece = is_white ? 4 : -4;
    int bishop_piece = is_white ? 3 : -3;
    
    // Get rook moves by temporarily making it a rook
    temp_board.squares[square] = rook_piece;
    vector<Move> rook_moves = generate_rook_moves(temp_board, square);
    
    // Get bishop moves by temporarily making it a bishop
    temp_board.squares[square] = bishop_piece;
    vector<Move> bishop_moves = generate_bishop_moves(temp_board, square);
    
    // Combine both move sets
    moves.insert(moves.end(), rook_moves.begin(), rook_moves.end());
    moves.insert(moves.end(), bishop_moves.begin(), bishop_moves.end());
    
    return moves;
}

vector<Move> generate_king_moves(const Board& board, int square) {
    vector<Move> moves;
    
    int piece = get_piece(board, square);
    if (abs(piece) != 6) return moves; // Not a king
    
    bool is_white = piece > 0;
    
    // King moves one square in any direction: 8 directions
    int king_offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    
    for (int i = 0; i < 8; i++) {
        int target_square = square + king_offsets[i];
        
        if (!is_valid_square(target_square)) continue;
        
        // Check if we've wrapped around the board
        int from_file = square % 8;
        int to_file = target_square % 8;
        int file_diff = abs(to_file - from_file);
        
        if (file_diff > 1) continue; // Wrapped around
        
        int target_piece = get_piece(board, target_square);
        
        // Can move to empty square or capture opponent piece
        if (target_piece == 0 || (target_piece > 0) != is_white) {
            moves.push_back(Move(square, target_square));
        }
    }
    
    // Castling moves
    if (is_white) {
        // White kingside castling
        if (board.white_can_castle_kingside && square == 4) { // King on e1
            if (get_piece(board, 5) == 0 && get_piece(board, 6) == 0 && get_piece(board, 7) == 4) {
                moves.push_back(Move(square, 6)); // King moves to g1
            }
        }
        
        // White queenside castling
        if (board.white_can_castle_queenside && square == 4) { // King on e1
            if (get_piece(board, 1) == 0 && get_piece(board, 2) == 0 && get_piece(board, 3) == 0 && get_piece(board, 0) == 4) {
                moves.push_back(Move(square, 2)); // King moves to c1
            }
        }
    } else {
        // Black kingside castling
        if (board.black_can_castle_kingside && square == 60) { // King on e8
            if (get_piece(board, 61) == 0 && get_piece(board, 62) == 0 && get_piece(board, 63) == -4) {
                moves.push_back(Move(square, 62)); // King moves to g8
            }
        }
        
        // Black queenside castling
        if (board.black_can_castle_queenside && square == 60) { // King on e8
            if (get_piece(board, 57) == 0 && get_piece(board, 58) == 0 && get_piece(board, 59) == 0 && get_piece(board, 56) == -4) {
                moves.push_back(Move(square, 58)); // King moves to c8
            }
        }
    }
    
    return moves;
}

bool is_in_check(const Board& board, bool white_king) {
    // Find the king
    int king_square = -1;
    int king_piece = white_king ? 6 : -6;
    
    for (int i = 0; i < 64; i++) {
        if (get_piece(board, i) == king_piece) {
            king_square = i;
            break;
        }
    }
    
    if (king_square == -1) {
        return false; // No king found
    }
    
    // Check if any opponent piece can attack the king square
    for (int i = 0; i < 64; i++) {
        int piece = get_piece(board, i);
        if (piece == 0) continue; // Empty square
        
        bool is_opponent = (piece > 0) != white_king;
        if (!is_opponent) continue; // Own piece
        
        // Generate moves for this opponent piece and see if any attack the king
        vector<Move> moves;
        int piece_type = abs(piece);
        
        switch (piece_type) {
            case 1: // Pawn
                moves = generate_pawn_moves(board, i);
                break;
            case 2: // Knight
                moves = generate_knight_moves(board, i);
                break;
            case 3: // Bishop
                moves = generate_bishop_moves(board, i);
                break;
            case 4: // Rook
                moves = generate_rook_moves(board, i);
                break;
            case 5: // Queen
                moves = generate_queen_moves(board, i);
                break;
            case 6: // King
                moves = generate_king_moves(board, i);
                break;
        }
        
        // Check if any move targets the king square
        for (const Move& move : moves) {
            if (move.to == king_square) {
                return true; // King is in check
            }
        }
    }
    
    return false; // King is not in check
}

void make_move_simple(Board& board, const Move& move) {
    int piece = get_piece(board, move.from);
    int captured_piece = get_piece(board, move.to);
    
    // Basic move: remove piece from source, place on destination
    set_piece(board, move.from, 0);
    
    // Handle promotion
    if (move.promotion != 0) {
        int promoted_piece = (piece > 0) ? move.promotion : -move.promotion;
        set_piece(board, move.to, promoted_piece);
    } else {
        set_piece(board, move.to, piece);
    }
    
    // Handle special moves
    int piece_type = abs(piece);
    bool is_white = piece > 0;
    
    // Handle castling
    if (piece_type == 6) { // King
        if (abs(move.to - move.from) == 2) { // Castling move
            if (move.to > move.from) { // Kingside
                int rook_from = is_white ? 7 : 63;
                int rook_to = is_white ? 5 : 61;
                int rook = get_piece(board, rook_from);
                set_piece(board, rook_from, 0);
                set_piece(board, rook_to, rook);
            } else { // Queenside
                int rook_from = is_white ? 0 : 56;
                int rook_to = is_white ? 3 : 59;
                int rook = get_piece(board, rook_from);
                set_piece(board, rook_from, 0);
                set_piece(board, rook_to, rook);
            }
        }
    }
    
    // Handle en passant capture
    if (piece_type == 1 && move.to == board.en_passant_square) {
        // Remove the captured pawn
        int captured_pawn_square = is_white ? move.to - 8 : move.to + 8;
        set_piece(board, captured_pawn_square, 0);
    }
    
    // Update en passant square
    if (piece_type == 1 && abs(move.to - move.from) == 16) {
        // Pawn double move - set en passant square
        board.en_passant_square = is_white ? move.from + 8 : move.from - 8;
    } else {
        board.en_passant_square = -1; // Clear en passant
    }
    
    // Update castling rights (simplified - any king/rook move disables)
    if (piece_type == 6) { // King moved
        if (is_white) {
            board.white_can_castle_kingside = false;
            board.white_can_castle_queenside = false;
        } else {
            board.black_can_castle_kingside = false;
            board.black_can_castle_queenside = false;
        }
    } else if (piece_type == 4) { // Rook moved
        if (is_white) {
            if (move.from == 0) board.white_can_castle_queenside = false;
            if (move.from == 7) board.white_can_castle_kingside = false;
        } else {
            if (move.from == 56) board.black_can_castle_queenside = false;
            if (move.from == 63) board.black_can_castle_kingside = false;
        }
    }
    
    // Update turn
    board.white_to_move = !board.white_to_move;
}

bool is_legal_move(const Board& board, const Move& move) {
    int piece = get_piece(board, move.from);
    int piece_type = abs(piece);
    bool is_white = piece > 0;
    
    // Special handling for castling
    if (piece_type == 6 && abs(move.to - move.from) == 2) {
        // This is a castling move - check if king is in check, passes through check, or ends in check
        
        // 1. King cannot be in check before castling
        if (is_in_check(board, is_white)) {
            return false;
        }
        
        // 2. King cannot pass through check
        int intermediate_square = (move.from + move.to) / 2; // Square between from and to
        Board temp_board = board;
        set_piece(temp_board, move.from, 0);
        set_piece(temp_board, intermediate_square, piece);
        if (is_in_check(temp_board, is_white)) {
            return false;
        }
        
        // 3. King cannot end in check (handled by normal check below)
    }
    
    // Copy the board
    Board temp_board = board;
    
    // Make the move on the copy
    make_move_simple(temp_board, move);
    
    // Check if the move leaves our king in check
    bool our_king_in_check = is_in_check(temp_board, board.white_to_move);
    
    return !our_king_in_check;
}

// UCI interface functions
string move_to_uci(const Move& move) {
    string result = square_to_string(move.from) + square_to_string(move.to);
    
    // Add promotion piece if applicable
    if (move.promotion != 0) {
        switch (move.promotion) {
            case 2: result += "n"; break; // knight
            case 3: result += "b"; break; // bishop  
            case 4: result += "r"; break; // rook
            case 5: result += "q"; break; // queen
        }
    }
    
    return result;
}

Move uci_to_move(const string& uci_str) {
    if (uci_str.length() < 4) return Move(-1, -1); // Invalid
    
    int from = string_to_square(uci_str.substr(0, 2));
    int to = string_to_square(uci_str.substr(2, 2));
    int promotion = 0;
    
    // Check for promotion
    if (uci_str.length() == 5) {
        char promo = uci_str[4];
        switch (promo) {
            case 'n': promotion = 2; break; // knight
            case 'b': promotion = 3; break; // bishop
            case 'r': promotion = 4; break; // rook
            case 'q': promotion = 5; break; // queen
        }
    }
    
    return Move(from, to, promotion);
}


Board parse_uci_position(const string& position_command) {
    istringstream iss(position_command);
    string token;
    iss >> token; // Skip "position"
    
    Board board;
    
    if (iss >> token) {
        if (token == "startpos") {
            board = create_starting_position();
        }
        // For 4k engine, only support startpos (no FEN parsing needed)
    }
    
    // Check for moves
    if (iss >> token && token == "moves") {
        // Apply moves
        while (iss >> token) {
            Move move = uci_to_move(token);
            if (move.from != -1 && move.to != -1) {
                make_move_simple(board, move);
            }
        }
    }
    
    return board;
}

vector<Move> generate_all_legal_moves(const Board& board) {
    vector<Move> all_moves;
    
    for (int square = 0; square < 64; square++) {
        int piece = get_piece(board, square);
        if (piece == 0) continue; // Empty square
        
        bool is_white = piece > 0;
        if (is_white != board.white_to_move) continue; // Wrong color
        
        vector<Move> piece_moves;
        int piece_type = abs(piece);
        
        switch (piece_type) {
            case 1: // Pawn
                piece_moves = generate_pawn_moves(board, square);
                break;
            case 2: // Knight
                piece_moves = generate_knight_moves(board, square);
                break;
            case 3: // Bishop
                piece_moves = generate_bishop_moves(board, square);
                break;
            case 4: // Rook
                piece_moves = generate_rook_moves(board, square);
                break;
            case 5: // Queen
                piece_moves = generate_queen_moves(board, square);
                break;
            case 6: // King
                piece_moves = generate_king_moves(board, square);
                break;
        }
        
        // Filter out illegal moves
        for (const Move& move : piece_moves) {
            if (is_legal_move(board, move)) {
                all_moves.push_back(move);
            }
        }
    }
    
    return all_moves;
}