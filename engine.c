#include <stdio.h>
#include <string.h>

// --- Constants for Board and Pieces ---
#define O 'O' // Off-board
#define E ' ' // Empty square

// White pieces (uppercase)
#define WP 'P'
#define WN 'N'
#define WB 'B'
#define WR 'R'
#define WQ 'Q'
#define WK 'K'

// Black pieces (lowercase)
#define BP 'p'
#define BN 'n'
#define BB 'b'
#define BR 'r'
#define BQ 'q'
#define BK 'k'

// --- Global Game State Variables ---
char b[120]; // 0x88 board representation
char s;      // Side to move: 0 for White, 1 for Black (0=W, 1=B)
char ep;     // En passant target square (0 if none)
char cr;     // Castling rights: bitmask (WK=1, WQ=2, BK=4, BQ=8)
char k[2];   // King squares: k[0] for White King, k[1] for Black King

// --- Move Representation (unsigned int) ---
// Bits:
// 0-6: From square (7 bits)
// 7-13: To square (7 bits)
// 14-16: Promotion piece type (3 bits, e.g., N, B, R, Q)
// 17-21: Flags (5 bits, e.g., capture, en passant, castling, double pawn push)
typedef unsigned int M;

// Macros to pack/unpack moves
#define FROM(m) ((m) & 0x7F)
#define TO(m) (((m) >> 7) & 0x7F)
#define PROM(m) (((m) >> 14) & 0x7)
#define FLAG(m) (((m) >> 17) & 0x1F)
#define MOVE(f, t, p, fl) ((M)((f) | ((t) << 7) | ((p) << 14) | ((fl) << 17)))

// Move flags (5 bits)
#define M_NORMAL 0x00
#define M_CAPTURE 0x01
#define M_DBL_PUSH 0x02
#define M_EP 0x03
#define M_KS_CASTLE 0x04
#define M_QS_CASTLE 0x05
#define M_PROM 0x08 // This flag indicates a promotion, PROM bits specify the piece type

// Promotion piece types (for PROM bits, 0-7)
#define P_KNIGHT 0
#define P_BISHOP 1
#define P_ROOK 2
#define P_QUEEN 3

// --- Move List ---
M ml[256]; // Array to store generated pseudo-legal moves
int mc;    // Move count

// --- State Stack for Unmake Move ---
typedef struct {
    char ep_s; // Previous en passant square
    char cr_s; // Previous castling rights
    char c_s;  // Captured piece (E if no capture)
} S;

S ss[256]; // State stack (max 256 moves, more than enough for a game)
int si;    // Stack index

// --- Piece Direction Arrays (static const to potentially save space) ---
static const int knight_dirs[] = {-21, -19, -12, -8, 8, 12, 19, 21};
static const int king_dirs[] = {-11, -10, -9, -1, 1, 9, 10, 11};
static const int rook_dirs[] = {-10, -1, 1, 10};
static const int bishop_dirs[] = {-11, -9, 9, 11};

// --- Function Prototypes (Forward Declarations) ---
// Core Board Operations
void make_move(M m);
void unmake_move(M m);
void init_board();
// void print_board(); // Removed for debugging

// Piece Type Checks
int is_own_piece(char piece);
int is_opponent_piece(char piece);

// Attack and Legality
int is_sq_attacked(int sq, int attacker_side);

// Move Generation
void add_legal_move(M m);
void gen_pawn_moves(int sq);
void gen_knight_moves(int sq);
void gen_king_moves(int sq);
void gen_sliding_moves(int sq, int* dirs, int num_dirs);
void gen_moves();

// UCI String Conversions
char* sq_to_str(int sq);
char* move_to_str(M m);
int str_to_sq(char* s);
M parse_move(char* move_str);


// --- Helper Functions for Piece Type Checks ---
// Returns 1 if piece belongs to the current side to move, 0 otherwise
int is_own_piece(char piece) {
    if (piece == E || piece == O) return 0;
    if (s == 0) return (piece >= 'A' && piece <= 'Z'); // White's turn, check for uppercase
    return (piece >= 'a' && piece <= 'z'); // Black's turn, check for lowercase
}

// Returns 1 if piece belongs to the opponent side, 0 otherwise
int is_opponent_piece(char piece) {
    if (piece == E || piece == O) return 0;
    if (s == 0) return (piece >= 'a' && piece <= 'z'); // White's turn, check for lowercase
    return (piece >= 'A' && piece <= 'Z'); // Black's turn, check for uppercase
}

// --- Function to add a move to the move list ---
// This function will now also check for legality before adding
void add_legal_move(M m) {
    // fprintf(stderr, "DEBUG: Considering move %s (from %d to %d, flag %d, prom %d)\n", move_to_str(m), FROM(m), TO(m), FLAG(m), PROM(m));
    make_move(m);
    int attacked = is_sq_attacked(k[!s], s);
    if (!attacked) {
        ml[mc++] = m;
        // fprintf(stderr, "DEBUG: Added legal move %s. Total moves: %d\n", move_to_str(m), mc);
    } else {
        // fprintf(stderr, "DEBUG: Move %s is ILLEGAL (king attacked). Not added.\n", move_to_str(m));
    }
    unmake_move(m);
}

// --- Board Initialization ---
void init_board() {
    int i;
    // Initialize all squares to off-board
    for (i = 0; i < 120; i++) {
        b[i] = O;
    }

    // Initialize valid squares to empty
    for (i = 0; i < 8; i++) {
        int j;
        for (j = 0; j < 8; j++) {
            b[21 + i * 10 + j] = E;
        }
    }

    // Set up starting position pieces
    // R N B Q K B N R
    b[21] = WR; b[22] = WN; b[23] = WB; b[24] = WQ; b[25] = WK; b[26] = WB; b[27] = WN; b[28] = WR;
    // P P P P P P P P
    for (i = 0; i < 8; i++) b[31 + i] = WP;

    // p p p p p p p p
    for (i = 0; i < 8; i++) b[81 + i] = BP;
    // r n b q k b n r
    b[91] = BR; b[92] = BN; b[93] = BB; b[94] = BQ; b[95] = BK; b[96] = BB; b[97] = BN; b[98] = BR;

    // Set initial game state
    s = 0;    // White to move
    ep = 0;   // No en passant target
    cr = 15;  // All castling rights (WK=1, WQ=2, BK=4, BQ=8)
    k[0] = 25; // White King on e1 (0x88: 21+4=25)
    k[1] = 95; // Black King on e8 (0x88: 91+4=95)
    si = 0;   // Reset state stack index
}

// --- Make Move ---
void make_move(M m) {
    char from = FROM(m);
    char to = TO(m);
    char flag = FLAG(m);
    char prom_piece_type = PROM(m);

    // Save state for unmake
    ss[si].ep_s = ep;
    ss[si].cr_s = cr;
    ss[si].c_s = E; // Assume no capture initially
    si++;

    ep = 0; // Reset en passant target by default for next move

    // Handle castling rights
    // If king moves, lose castling rights for that side
    if (b[from] == WK) cr &= ~3; // Lose WK (1) and WQ (2) rights
    else if (b[from] == BK) cr &= ~12; // Lose BK (4) and BQ (8) rights
    // If rook moves or is captured, lose corresponding castling rights
    if (from == 21 || to == 21) cr &= ~2; // White A1 (WQ)
    if (from == 28 || to == 28) cr &= ~1; // White H1 (WK)
    if (from == 91 || to == 91) cr &= ~8; // Black A8 (BQ)
    if (from == 98 || to == 98) cr &= ~4; // Black H8 (BK)

    // Handle capture
    if (b[to] != E) {
        ss[si-1].c_s = b[to]; // Save captured piece
    }

    // Move piece
    b[to] = b[from];
    b[from] = E;

    // Update king position if king moved
    if (b[to] == WK) k[0] = to;
    else if (b[to] == BK) k[1] = to;

    // Handle special move flags
    switch (flag) {
        case M_DBL_PUSH:
            ep = (s == 0) ? (to - 10) : (to + 10); // Set en passant target
            break;
        case M_EP:
            // Captured pawn is not on 'to' square
            if (s == 0) { // White's move, black pawn captured
                ss[si-1].c_s = BP; // Save captured pawn
                b[to + 10] = E; // Remove black pawn
            } else { // Black's move, white pawn captured
                ss[si-1].c_s = WP; // Save captured pawn
                b[to - 10] = E; // Remove white pawn
            }
            break;
        case M_KS_CASTLE:
            if (s == 0) { // White kingside
                b[28] = E; b[26] = WR; // Move rook H1 to F1
            } else { // Black kingside
                b[98] = E; b[96] = BR; // Move rook H8 to F8
            }
            break;
        case M_QS_CASTLE:
            if (s == 0) { // White queenside
                b[21] = E; b[24] = WR; // Move rook A1 to D1
            } else { // Black queenside
                b[91] = E; b[94] = BR; // Move rook A8 to D8
            }
            break;
    }

    // Handle promotion (if M_PROM flag is set, regardless of other flags)
    if (flag & M_PROM) {
        if (s == 0) { // White promotion
            if (prom_piece_type == P_KNIGHT) b[to] = WN;
            else if (prom_piece_type == P_BISHOP) b[to] = WB;
            else if (prom_piece_type == P_ROOK) b[to] = WR;
            else if (prom_piece_type == P_QUEEN) b[to] = WQ;
        } else { // Black promotion
            if (prom_piece_type == P_KNIGHT) b[to] = BN;
            else if (prom_piece_type == P_BISHOP) b[to] = BB;
            else if (prom_piece_type == P_ROOK) b[to] = BR;
            else if (prom_piece_type == P_QUEEN) b[to] = BQ;
        }
    }

    // Switch side to move
    s = !s;
}

// --- Unmake Move ---
void unmake_move(M m) {
    si--; // Pop state from stack
    char from = FROM(m);
    char to = TO(m);
    char flag = FLAG(m);
    // char prom_piece_type = PROM(m); // Not directly used in unmake, but available

    // Restore piece that moved
    if (flag & M_PROM) {
        // If it was a promotion, the piece on 'to' is the promoted piece.
        // We need to put the original pawn back on 'from'.
        b[from] = (s == 0) ? BP : WP; // Restore pawn to 'from' square (s is already flipped)
    } else {
        // Normal move, just move the piece back
        b[from] = b[to];
    }

    // Restore captured piece to 'to' square
    b[to] = ss[si].c_s; // This will be E if no capture, or the captured piece

    // Handle special unmake logic
    switch (flag) {
        case M_EP:
            // Restore captured pawn for en passant
            if (s == 0) { // White's move, black pawn captured (s is already flipped)
                b[to + 10] = BP; // Restore black pawn
            } else { // Black's move, white pawn captured (s is already flipped)
                b[to - 10] = WP; // Restore white pawn
            }
            break;
        case M_KS_CASTLE:
            if (s == 0) { // White kingside (s is already flipped)
                b[28] = WR; b[26] = E; // Move rook F1 back to H1
            } else { // Black kingside (s is already flipped)
                b[98] = BR; b[96] = E; // Move rook F8 back to H8
            }
            break;
        case M_QS_CASTLE:
            if (s == 0) { // White queenside
                b[21] = WR; b[24] = E; // Move rook D1 back to A1
            } else { // Black queenside
                b[91] = BR; b[94] = E; // Move rook A8 back to D8
            }
            break;
    }

    // Restore general game state
    ep = ss[si].ep_s;
    cr = ss[si].cr_s;
    s = !s; // Switch side back

    // Update king position if king moved
    if (b[from] == WK) k[0] = from;
    else if (b[from] == BK) k[1] = from;
}

// --- Check if a square is attacked by a given side ---
// sq: The square to check for attacks
// attacker_side: The side whose pieces are doing the attacking (0=White, 1=Black)
int is_sq_attacked(int sq, int attacker_side) {
    char piece;
    int target_sq;
    int i, dir;

    // Determine opponent's piece types based on attacker_side
    // If attacker_side is White (0), we look for White pieces.
    // If attacker_side is Black (1), we look for Black pieces.
    char att_pawn = (attacker_side == 0) ? WP : BP;
    char att_knight = (attacker_side == 0) ? WN : BN;
    char att_king = (attacker_side == 0) ? WK : BK;
    char att_rook = (attacker_side == 0) ? WR : BR;
    char att_bishop = (attacker_side == 0) ? WB : BB;
    char att_queen = (attacker_side == 0) ? WQ : BQ;

    // 1. Check for Pawn attacks
    if (attacker_side == 0) { // White pawns attack "up-right" and "up-left" relative to their position
        if (b[sq - 11] == att_pawn || b[sq - 9] == att_pawn) return 1;
    } else { // Black pawns attack "down-right" and "down-left" relative to their position
        if (b[sq + 11] == att_pawn || b[sq + 9] == att_pawn) return 1;
    }

    // 2. Check for Knight attacks
    for (i = 0; i < 8; i++) {
        target_sq = sq + knight_dirs[i];
        if (b[target_sq] == att_knight) return 1;
    }

    // 3. Check for King attacks
    for (i = 0; i < 8; i++) {
        target_sq = sq + king_dirs[i];
        if (b[target_sq] == att_king) return 1;
    }

    // 4. Check for Sliding Piece attacks (Rooks, Bishops, Queens)
    // Rook/Queen (straight lines)
    for (i = 0; i < 4; i++) {
        dir = rook_dirs[i];
        target_sq = sq + dir;
        while (b[target_sq] != O) { // Iterate along the ray
            piece = b[target_sq];
            if (piece != E) { // Found a piece
                if (piece == att_rook || piece == att_queen) return 1;
                break; // Blocker found, stop checking this ray
            }
            target_sq += dir;
        }
    }

    // Bishop/Queen (diagonal lines)
    for (i = 0; i < 4; i++) {
        dir = bishop_dirs[i];
        target_sq = sq + dir;
        while (b[target_sq] != O) { // Iterate along the ray
            piece = b[target_sq];
            if (piece != E) { // Found a piece
                if (piece == att_bishop || piece == att_queen) return 1;
                break; // Blocker found, stop checking this ray
            }
            target_sq += dir;
        }
    }

    return 0; // No attacks found
}

// --- Pseudo-legal Move Generation Functions ---

void gen_pawn_moves(int sq) {
    int target_sq;
    int pawn_push_dir, pawn_dbl_push_dir;
    int pawn_capture_left_dir, pawn_capture_right_dir;

    if (s == 0) { // White pawns
        pawn_push_dir = 10;
        pawn_dbl_push_dir = 20;
        pawn_capture_left_dir = 9;
        pawn_capture_right_dir = 11;

        // Single push
        target_sq = sq + pawn_push_dir;
        if (b[target_sq] == E) {
            if (target_sq >= 91 && target_sq <= 98) { // Promotion rank (rank 8)
                add_legal_move(MOVE(sq, target_sq, P_QUEEN, M_PROM));
                add_legal_move(MOVE(sq, target_sq, P_ROOK, M_PROM));
                add_legal_move(MOVE(sq, target_sq, P_BISHOP, M_PROM));
                add_legal_move(MOVE(sq, target_sq, P_KNIGHT, M_PROM));
            } else {
                add_legal_move(MOVE(sq, target_sq, 0, M_NORMAL));
            }

            // Double push
            if (sq >= 31 && sq <= 38) { // On starting rank (rank 2)
                target_sq = sq + pawn_dbl_push_dir;
                if (b[sq + pawn_push_dir] == E && b[target_sq] == E) { // Check intermediate square too
                    add_legal_move(MOVE(sq, target_sq, 0, M_DBL_PUSH));
                }
            }
        }

        // Captures
        target_sq = sq + pawn_capture_left_dir;
        if (b[target_sq] != O && is_opponent_piece(b[target_sq])) {
            if (target_sq >= 91 && target_sq <= 98) { // Promotion rank
                add_legal_move(MOVE(sq, target_sq, P_QUEEN, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_ROOK, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_BISHOP, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_KNIGHT, M_PROM | M_CAPTURE));
            } else {
                add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
            }
        }
        target_sq = sq + pawn_capture_right_dir;
        if (b[target_sq] != O && is_opponent_piece(b[target_sq])) {
            if (target_sq >= 91 && target_sq <= 98) { // Promotion rank
                add_legal_move(MOVE(sq, target_sq, P_QUEEN, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_ROOK, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_BISHOP, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_KNIGHT, M_PROM | M_CAPTURE));
            } else {
                add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
            }
        }

        // En passant capture
        if (ep != 0) {
            if (sq + pawn_capture_left_dir == ep) add_legal_move(MOVE(sq, ep, 0, M_EP));
            if (sq + pawn_capture_right_dir == ep) add_legal_move(MOVE(sq, ep, 0, M_EP));
        }

    } else { // Black pawns
        pawn_push_dir = -10;
        pawn_dbl_push_dir = -20;
        pawn_capture_left_dir = -9;
        pawn_capture_right_dir = -11;

        // Single push
        target_sq = sq + pawn_push_dir;
        if (b[target_sq] == E) {
            if (target_sq >= 21 && target_sq <= 28) { // Promotion rank (rank 1)
                add_legal_move(MOVE(sq, target_sq, P_QUEEN, M_PROM));
                add_legal_move(MOVE(sq, target_sq, P_ROOK, M_PROM));
                add_legal_move(MOVE(sq, target_sq, P_BISHOP, M_PROM));
                add_legal_move(MOVE(sq, target_sq, P_KNIGHT, M_PROM));
            } else {
                add_legal_move(MOVE(sq, target_sq, 0, M_NORMAL));
            }

            // Double push
            if (sq >= 81 && sq <= 88) { // On starting rank (rank 7)
                target_sq = sq + pawn_dbl_push_dir;
                if (b[sq + pawn_push_dir] == E && b[target_sq] == E) { // Check intermediate square too
                    add_legal_move(MOVE(sq, target_sq, 0, M_DBL_PUSH));
                }
            }
        }

        // Captures
        target_sq = sq + pawn_capture_left_dir;
        if (b[target_sq] != O && is_opponent_piece(b[target_sq])) {
            if (target_sq >= 21 && target_sq <= 28) { // Promotion rank
                add_legal_move(MOVE(sq, target_sq, P_QUEEN, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_ROOK, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_BISHOP, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_KNIGHT, M_PROM | M_CAPTURE));
            } else {
                add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
            }
        }
        target_sq = sq + pawn_capture_right_dir;
        if (b[target_sq] != O && is_opponent_piece(b[target_sq])) {
            if (target_sq >= 21 && target_sq <= 28) { // Promotion rank
                add_legal_move(MOVE(sq, target_sq, P_QUEEN, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_ROOK, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_BISHOP, M_PROM | M_CAPTURE));
                add_legal_move(MOVE(sq, target_sq, P_KNIGHT, M_PROM | M_CAPTURE));
            } else {
                add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
            }
        }

        // En passant capture
        if (ep != 0) {
            if (sq + pawn_capture_left_dir == ep) add_legal_move(MOVE(sq, ep, 0, M_EP));
            if (sq + pawn_capture_right_dir == ep) add_legal_move(MOVE(sq, ep, 0, M_EP));
        }
    }
}

void gen_knight_moves(int sq) {
    int i, target_sq;
    for (i = 0; i < 8; i++) {
        target_sq = sq + knight_dirs[i];
        if (b[target_sq] == O) continue; // Off-board
        if (is_own_piece(b[target_sq])) continue; // Cannot capture own piece

        if (b[target_sq] == E) {
            add_legal_move(MOVE(sq, target_sq, 0, M_NORMAL));
        } else { // Capture
            add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
        }
    }
}

void gen_king_moves(int sq) {
    int i, target_sq;
    for (i = 0; i < 8; i++) {
        target_sq = sq + king_dirs[i];
        if (b[target_sq] == O) continue; // Off-board
        if (is_own_piece(b[target_sq])) continue; // Cannot capture own piece

        if (b[target_sq] == E) {
            add_legal_move(MOVE(sq, target_sq, 0, M_NORMAL));
        } else { // Capture
            add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
        }
    }

    // Castling (legality check will filter if king or path is attacked)
    if (s == 0) { // White
        if (cr & 1) { // WK (kingside)
            if (b[26] == E && b[27] == E) { // f1 and g1 empty
                // Check if king's path is attacked
                if (!is_sq_attacked(25, 1) && !is_sq_attacked(26, 1) && !is_sq_attacked(27, 1)) {
                    add_legal_move(MOVE(25, 27, 0, M_KS_CASTLE)); // e1g1
                }
            }
        }
        if (cr & 2) { // WQ (queenside)
            if (b[22] == E && b[23] == E && b[24] == E) { // b1, c1, d1 empty
                // Check if king's path is attacked
                if (!is_sq_attacked(25, 1) && !is_sq_attacked(24, 1) && !is_sq_attacked(23, 1)) {
                    add_legal_move(MOVE(25, 23, 0, M_QS_CASTLE)); // e1c1
                }
            }
        }
    } else { // Black
        if (cr & 4) { // BK (kingside)
            if (b[96] == E && b[97] == E) { // f8 and g8 empty
                // Check if king's path is attacked
                if (!is_sq_attacked(95, 0) && !is_sq_attacked(96, 0) && !is_sq_attacked(97, 0)) {
                    add_legal_move(MOVE(95, 97, 0, M_KS_CASTLE)); // e8g8
                }
            }
        }
        if (cr & 8) { // BQ (queenside)
            if (b[92] == E && b[93] == E && b[94] == E) { // b8, c8, d8 empty
                // Check if king's path is attacked
                if (!is_sq_attacked(95, 0) && !is_sq_attacked(94, 0) && !is_sq_attacked(93, 0)) {
                    add_legal_move(MOVE(95, 93, 0, M_QS_CASTLE)); // e8c8
                }
            }
        }
    }
}

// Generic sliding piece move generation
void gen_sliding_moves(int sq, int* dirs, int num_dirs) {
    int i, dir, target_sq;
    for (i = 0; i < num_dirs; i++) {
        dir = dirs[i];
        target_sq = sq + dir;
        while (b[target_sq] != O) { // Iterate along the ray until off-board
            if (is_own_piece(b[target_sq])) break; // Blocked by own piece
            if (b[target_sq] == E) {
                add_legal_move(MOVE(sq, target_sq, 0, M_NORMAL));
            } else { // Capture
                add_legal_move(MOVE(sq, target_sq, 0, M_CAPTURE));
                break; // Blocked by opponent piece (captured)
            }
            target_sq += dir;
        }
    }
}

// --- Main Legal Move Generation Function ---
void gen_moves() {
    mc = 0; // Reset move count
    int sq;
    char piece;

    for (sq = 21; sq <= 98; sq++) { // Iterate through all 0x88 board squares
        piece = b[sq];
        if (piece == O || piece == E) continue; // Skip off-board and empty squares

        if (s == 0) { // White to move
            if (piece == WP) gen_pawn_moves(sq);
            else if (piece == WN) gen_knight_moves(sq);
            else if (piece == WB) gen_sliding_moves(sq, (int*)bishop_dirs, 4);
            else if (piece == WR) gen_sliding_moves(sq, (int*)rook_dirs, 4);
            else if (piece == WQ) {
                gen_sliding_moves(sq, (int*)rook_dirs, 4);
                gen_sliding_moves(sq, (int*)bishop_dirs, 4);
            }
            else if (piece == WK) gen_king_moves(sq);
        } else { // Black to move
            if (piece == BP) gen_pawn_moves(sq);
            else if (piece == BN) gen_knight_moves(sq);
            else if (piece == BB) gen_sliding_moves(sq, (int*)bishop_dirs, 4);
            else if (piece == BR) gen_sliding_moves(sq, (int*)rook_dirs, 4);
            else if (piece == BQ) {
                gen_sliding_moves(sq, (int*)rook_dirs, 4);
                gen_sliding_moves(sq, (int*)bishop_dirs, 4);
            }
            else if (piece == BK) gen_king_moves(sq);
        }
    }
}

// --- Square to String Conversion ---
// Converts a 0x88 square index to algebraic notation (e.g., 21 -> "a1")
char* sq_to_str(int sq) {
    static char s[3]; // e.g., "a1\0"
    s[0] = (sq % 10) - 1 + 'a'; // File
    s[1] = (sq / 10) - 2 + '1'; // Rank
    s[2] = '\0';
    return s;
}

// --- Move to String Conversion ---
// Converts an internal M move to UCI algebraic notation (e.g., "e2e4", "e7e8q")
char* move_to_str(M m) {
    static char s[6]; // Max "g1h1q\0" or "a1a1\0" for castling
    char from_buf[3]; // Local buffer for from_sq string
    char to_buf[3];   // Local buffer for to_sq string
    char prom_char = '\0';

    // Copy results from sq_to_str to local buffers
    strcpy(from_buf, sq_to_str(FROM(m)));
    strcpy(to_buf, sq_to_str(TO(m)));

    // Determine promotion character if applicable
    if (FLAG(m) & M_PROM) {
        switch (PROM(m)) {
            case P_KNIGHT: prom_char = 'n'; break;
            case P_BISHOP: prom_char = 'b'; break;
            case P_ROOK: prom_char = 'r'; break;
            case P_QUEEN: prom_char = 'q'; break;
        }
    }

    // Format the string using local buffers
    s[0] = from_buf[0];
    s[1] = from_buf[1];
    s[2] = to_buf[0];
    s[3] = to_buf[1];
    s[4] = prom_char;
    s[5] = '\0';

    return s;
}

// --- String to Square Conversion ---
// Converts algebraic notation (e.g., "a1") to a 0x88 square index
int str_to_sq(char* s) {
    int file = s[0] - 'a' + 1;
    int rank = s[1] - '1' + 2;
    return rank * 10 + file;
}

// --- Parse Move from String ---
// Takes a UCI move string (e.g., "e2e4", "e7e8q") and finds the corresponding M from ml
// Returns 0 if move not found (invalid move string or not in generated list)
M parse_move(char* move_str) {
    int from_sq = str_to_sq(move_str);
    int to_sq = str_to_sq(move_str + 2);
    char prom_char = (strlen(move_str) == 5) ? move_str[4] : '\0';
    int prom_type = 0; // Default to no promotion

    if (prom_char != '\0') {
        switch (prom_char) {
            case 'n': prom_type = P_KNIGHT; break;
            case 'b': prom_type = P_BISHOP; break;
            case 'r': prom_type = P_ROOK; break;
            case 'q': prom_type = P_QUEEN; break;
            default: return 0; // Invalid promotion character
        }
    }

    // Generate moves for the current position to find a match
    // This is crucial because the flags (capture, double push, castling, EP)
    // are determined by the board state and are set during gen_moves.
    // We need to find the exact move object that matches from, to, and promotion.
    gen_moves(); // Regenerate legal moves for current position

    for (int i = 0; i < mc; i++) {
        M current_move = ml[i];
        if (FROM(current_move) == from_sq && TO(current_move) == to_sq) {
            // Check promotion for pawn moves
            if ((FLAG(current_move) & M_PROM) && PROM(current_move) == prom_type) {
                return current_move;
            }
            // For non-promotion moves, ensure no promotion was expected
            if (!(FLAG(current_move) & M_PROM) && prom_char == '\0') {
                return current_move;
            }
        }
    }

    return 0; // Move not found in the legal move list
}


// --- Main UCI Loop ---
int main() {
    char l[256];
    setbuf(stdin, NULL); // Ensure stdin is unbuffered
    setbuf(stdout, NULL); // Ensure stdout is unbuffered for UCI communication

    init_board(); // Initialize board at startup

    while (fgets(l, 256, stdin)) {
        if (strncmp(l, "uci", 3) == 0) {
            printf("id name Agent4k\nid author Aider\nuciok\n");
        } else if (strncmp(l, "isready", 7) == 0) {
            printf("readyok\n");
        } else if (strncmp(l, "position startpos", 17) == 0) {
            init_board(); // Reset to startpos
            char *moves_str = strstr(l, "moves");
            if (moves_str) {
                moves_str += 6; // Move past "moves "
                char *token = strtok(moves_str, " \n");
                while (token) {
                    M move = parse_move(token);
                    if (move != 0) { // Check if parse_move found a valid move
                        make_move(move);
                    } else {
                        break;
                    }
                    token = strtok(NULL, " \n");
                }
            }
        } else if (strncmp(l, "go", 2) == 0) {
            gen_moves(); // Generate all legal moves for the current position

            if (mc > 0) {
                // For now, just pick the first legal move
                M best_move = ml[0];
                printf("bestmove %s\n", move_to_str(best_move));
            } else {
                // No legal moves, implies checkmate or stalemate
                // For now, just output (none)
                printf("bestmove (none)\n");
            }
        } else if (strncmp(l, "quit", 4) == 0) {
            break;
        }
    }
    return 0;
}
