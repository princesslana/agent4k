#include "engine.h"

// Define the global variables that are declared extern in engine.h
int b[64];
int s = WHITE;
int castling_rights = 0;
int en_passant_square = -1;
chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
int time_limit_ms = 1000;

bool time_up(){
auto now = chrono::steady_clock::now();
auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - start_time).count();
return elapsed >= time_limit_ms;
}
int square_from_string(const string& sq){
if (sq.length() != 2) return -1;
int file = sq[0] - 'a';
int rank = sq[1] - '1';
if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
return rank * 8 + file;
}
string square_to_string(int sq){
if (sq < 0 || sq > 63) return "";
char file = 'a' + (sq % 8);
char rank = '1' + (sq / 8);
return string(1, file) + string(1, rank);
}
void init_board(){
for (int i = 0; i < 64; i++){
b[i] = EMPTY;
}
b[0] = WHITE * ROOK; b[1] = WHITE * KNIGHT; b[2] = WHITE * BISHOP; b[3] = WHITE * QUEEN;
b[4] = WHITE * KING; b[5] = WHITE * BISHOP; b[6] = WHITE * KNIGHT; b[7] = WHITE * ROOK;
for (int i = 8; i < 16; i++) b[i] = WHITE * PAWN;
for (int i = 48; i < 56; i++) b[i] = BLACK * PAWN;
b[56] = BLACK * ROOK; b[57] = BLACK * KNIGHT; b[58] = BLACK * BISHOP; b[59] = BLACK * QUEEN;
b[60] = BLACK * KING; b[61] = BLACK * BISHOP; b[62] = BLACK * KNIGHT; b[63] = BLACK * ROOK;
s = WHITE;
castling_rights = 15;
en_passant_square = -1;
}

Position get_position() {
Position pos;
for (int i = 0; i < 64; i++) pos.board[i] = b[i];
pos.side = s;
pos.castling_rights = castling_rights;
pos.en_passant_square = en_passant_square;
return pos;
}

void set_position(const Position& pos) {
for (int i = 0; i < 64; i++) b[i] = pos.board[i];
s = pos.side;
castling_rights = pos.castling_rights;
en_passant_square = pos.en_passant_square;
}

Position apply_move(const Position& pos, const Move& m) {
Position new_pos = pos;
new_pos.board[m.f] = EMPTY;
new_pos.board[m.t] = m.promotion ? m.promotion : m.p;
new_pos.side = -new_pos.side;

// Handle en passant square
new_pos.en_passant_square = -1;
if (abs(m.p) == PAWN && abs(m.t - m.f) == 16) {
new_pos.en_passant_square = (m.f + m.t) / 2;
}

return new_pos;
}
bool is_square_attacked(int square, int attacking_side){
for (int f = 0; f < 64; f++){
int p = b[f];
if (p == EMPTY || (p > 0) != (attacking_side > 0)) continue;
int piece_type = abs(p);
int piece_color = p > 0 ? WHITE : BLACK;
if (piece_type == PAWN){
int direction = (piece_color == WHITE) ? 8 : -8;
int left_attack = f + direction - 1;
int right_attack = f + direction + 1;
if ((left_attack == square && (f % 8) > 0) || (right_attack == square && (f % 8) < 7)){
return true;
}
} else if (piece_type == KNIGHT){
int knight_moves[] = {-17, -15, -10, -6, 6, 10, 15, 17};
for (int i = 0; i < 8; i++){
if (f + knight_moves[i] == square){
int from_row = f / 8, from_col = f % 8;
int to_row = square / 8, to_col = square % 8;
if (abs(from_row - to_row) <= 2 && abs(from_col - to_col) <= 2){
return true;
}
}
}
} else if (piece_type == BISHOP || piece_type == QUEEN){
int dirs[] = {-9, -7, 7, 9};
for (int d = 0; d < 4; d++){
for (int i = 1; i < 8; i++){
int sq = f + dirs[d] * i;
if (sq < 0 || sq >= 64) break;
if (abs((sq % 8) - (f % 8)) != i || abs((sq / 8) - (f / 8)) != i) break;
if (sq == square) return true;
if (b[sq] != EMPTY) break;
}
}
} else if (piece_type == ROOK || piece_type == QUEEN){
int dirs[] = {-8, -1, 1, 8};
for (int d = 0; d < 4; d++){
for (int i = 1; i < 8; i++){
int sq = f + dirs[d] * i;
if (sq < 0 || sq >= 64) break;
if (dirs[d] == -1 || dirs[d] == 1){
if (sq / 8 != f / 8) break;
}
if (sq == square) return true;
if (b[sq] != EMPTY) break;
}
}
} else if (piece_type == KING){
int dirs[] = {-9, -8, -7, -1, 1, 7, 8, 9};
for (int d = 0; d < 8; d++){
int sq = f + dirs[d];
if (sq < 0 || sq >= 64) continue;
if (abs((sq % 8) - (f % 8)) > 1 || abs((sq / 8) - (f / 8)) > 1) continue;
if (sq == square) return true;
}
}
}
return false;
}
bool is_in_check(){
for (int i = 0; i < 64; i++){
if (b[i] == s * KING){
return is_square_attacked(i, -s);
}
}
return false;
}

bool is_legal_move(const Move& m) {
    Position current_pos = get_position();
    Position new_pos = apply_move(current_pos, m);
    set_position(new_pos);
    
    // Check if the original side's king is in check (not the opponent's)
    bool legal = !is_square_attacked(
        [&]() {
            for (int i = 0; i < 64; i++) {
                if (new_pos.board[i] == current_pos.side * KING) {
                    return i;
                }
            }
            return -1;
        }(),
        -current_pos.side
    );
    
    set_position(current_pos);
    return legal;
}

vector<Move> generate_moves(){
vector<Move> moves;
for (int f = 0; f < 64; f++){
int p = b[f];
if (p == EMPTY || (p > 0) != (s > 0)) continue;
int piece_type = abs(p);
if (piece_type == PAWN){
int direction = (s == WHITE) ? 8 : -8;
int forward = f + direction;
if (forward >= 0 && forward < 64 && b[forward] == EMPTY){
if ((s == WHITE && forward >= 56) || (s == BLACK && forward <= 7)){
moves.push_back(Move(f, forward, p, 0));
moves.back().promotion = s * QUEEN;
} else {
moves.push_back(Move(f, forward, p, 0));
}
if (((s == WHITE && f < 16) || (s == BLACK && f >= 48)) && b[forward + direction] == EMPTY){
moves.push_back(Move(f, forward + direction, p, 0));
}
}
for (int dc = -1; dc <= 1; dc += 2){
int capture = f + direction + dc;
if (capture >= 0 && capture < 64 && (f % 8) + dc >= 0 && (f % 8) + dc < 8){
if (b[capture] != EMPTY && (b[capture] > 0) != (s > 0)){
if ((s == WHITE && capture >= 56) || (s == BLACK && capture <= 7)){
moves.push_back(Move(f, capture, p, b[capture]));
moves.back().promotion = s * QUEEN;
} else {
moves.push_back(Move(f, capture, p, b[capture]));
}
} else if (capture == en_passant_square){
moves.push_back(Move(f, capture, p, s == WHITE ? BLACK * PAWN : WHITE * PAWN));
}
}
}
} else if (piece_type == KNIGHT){
int knight_moves[] = {-17, -15, -10, -6, 6, 10, 15, 17};
for (int i = 0; i < 8; i++){
int t = f + knight_moves[i];
if (t >= 0 && t < 64){
int from_row = f / 8, from_col = f % 8;
int to_row = t / 8, to_col = t % 8;
if (abs(from_row - to_row) <= 2 && abs(from_col - to_col) <= 2){
if (b[t] == EMPTY || (b[t] > 0) != (s > 0)){
moves.push_back(Move(f, t, p, b[t]));
}
}
}
}
} else if (piece_type == BISHOP || piece_type == QUEEN){
int dirs[] = {-9, -7, 7, 9};
for (int d = 0; d < 4; d++){
for (int i = 1; i < 8; i++){
int t = f + dirs[d] * i;
if (t < 0 || t >= 64) break;
if (abs((t % 8) - (f % 8)) != i || abs((t / 8) - (f / 8)) != i) break;
if (b[t] != EMPTY){
if ((b[t] > 0) != (s > 0)){
moves.push_back(Move(f, t, p, b[t]));
}
break;
}
moves.push_back(Move(f, t, p, 0));
}
}
}
if (piece_type == ROOK || piece_type == QUEEN){
int dirs[] = {-8, -1, 1, 8};
for (int d = 0; d < 4; d++){
for (int i = 1; i < 8; i++){
int t = f + dirs[d] * i;
if (t < 0 || t >= 64) break;
if (dirs[d] == -1 || dirs[d] == 1){
if (t / 8 != f / 8) break;
}
if (b[t] != EMPTY){
if ((b[t] > 0) != (s > 0)){
moves.push_back(Move(f, t, p, b[t]));
}
break;
}
moves.push_back(Move(f, t, p, 0));
}
}
}
if (piece_type == KING){
int dirs[] = {-9, -8, -7, -1, 1, 7, 8, 9};
for (int d = 0; d < 8; d++){
int t = f + dirs[d];
if (t >= 0 && t < 64){
if (abs((t % 8) - (f % 8)) <= 1 && abs((t / 8) - (f / 8)) <= 1){
if (b[t] == EMPTY || (b[t] > 0) != (s > 0)){
moves.push_back(Move(f, t, p, b[t]));
}
}
}
}
}
}
vector<Move> legal_moves;
for (const Move& m : moves){
if (is_legal_move(m)){
legal_moves.push_back(m);
}
}
return legal_moves;
}
int evaluate(){
int score = 0;
int piece_values[] = {0, 100, 320, 330, 500, 900, 20000};
for (int i = 0; i < 64; i++){
if (b[i] != EMPTY){
int piece_type = abs(b[i]);
int piece_value = piece_values[piece_type];
if (b[i] > 0){
score += piece_value;
} else {
score -= piece_value;
}
}
}
return s == WHITE ? score : -score;
}
int search(int d, int a, int B){
if (time_up()) return 0;
if (d == 0) return evaluate();
vector<Move> moves = generate_moves();
if (moves.empty()) return is_in_check() ? -20000 : 0;
for (const Move& m : moves){
Position current_pos = get_position();
Position new_pos = apply_move(current_pos, m);
set_position(new_pos);
int score = -search(d - 1, -B, -a);
set_position(current_pos);
if (score >= B) return B;
if (score > a) a = score;
}
return a;
}
Move find_best_move(int d){
vector<Move> moves = generate_moves();
if (moves.empty()) return Move(0, 0, 0);
Move best_move = moves[0];
int best_score = INT_MIN;
for (const Move& m : moves){
Position current_pos = get_position();
Position new_pos = apply_move(current_pos, m);
set_position(new_pos);
int score = -search(d - 1, INT_MIN, INT_MAX);
set_position(current_pos);
if (score > best_score){
best_score = score;
best_move = m;
}
}
return best_move;
}
void parse_position(const string& command){
istringstream iss(command);
string token;
iss >> token;
if (token != "position") return;
iss >> token;
if (token == "startpos"){
init_board();
} else if (token == "fen"){
init_board();
}
iss >> token;
if (token == "moves"){
while (iss >> token){
if (token.length() >= 4){
int from = square_from_string(token.substr(0, 2));
int to = square_from_string(token.substr(2, 2));
if (from != -1 && to != -1){
vector<Move> moves = generate_moves();
for (const Move& m : moves){
if (m.f == from && m.t == to){
Position current_pos = get_position();
Position new_pos = apply_move(current_pos, m);
set_position(new_pos);
break;
}
}
}
}
}
}
}