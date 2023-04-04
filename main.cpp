#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>

enum PieceType {
    EMPTY = 0,
    PAWN = 1,
    ROOK = 2,
    KNIGHT = 3,
    BISHOP = 4,
    QUEEN = 5,
    KING = 6
};

enum Player {
    WHITE = 0,
    BLACK = 1
};

struct Piece {
    PieceType type;
    Player player;
};

class ChessBoard {
public:
    Player get_current_player() const{
        return current_player;
    }
    ChessBoard() : current_player(WHITE) {
        board = std::vector<std::vector<Piece>>(8, std::vector<Piece>(8, {EMPTY, WHITE}));

        for (int i = 0; i < 8; i++) {
            board[1][i] = {PAWN, BLACK};
            board[6][i] = {PAWN, WHITE};
        }

        init_pieces(0, BLACK);
        init_pieces(7, WHITE);
    }

    void print_board() {
        for (int row = 0; row < 8; ++row) {
            std::cout << 8 - row << " ";  // 在行开头显示行坐标
            for (int col = 0; col < 8; ++col) {
                std::cout << piece_to_char(board[row][col]) << " ";
            }
            std::cout << std::endl;
        }
        
        // 在棋盘下方显示列坐标
        std::cout << "  ";
        for (char col_label = 'a'; col_label <= 'h'; ++col_label) {
            std::cout << col_label << " ";
        }
        std::cout << std::endl;
    }

    bool move_piece(int from_row, int from_col, int to_row, int to_col) {
        if (from_row < 0 || from_row >= 8 || from_col < 0 || from_col >= 8 ||
            to_row < 0 || to_row >= 8 || to_col< 0 || to_col >= 8) {
            return false;
        }
        Piece moving_piece = board[from_row][from_col];
        if (moving_piece.type == EMPTY || moving_piece.player != current_player) {
            return false;
        }

        // Verify if the target position has a piece of the same color
        Piece target_piece = board[to_row][to_col];
        if (target_piece.type != EMPTY && target_piece.player == moving_piece.player) {
            return false;
        }

        // Validate the move according to the piece type
        if (!is_move_valid_for_piece(moving_piece, from_row, from_col, to_row, to_col)) {
            return false;
        }

        if (board[to_row][to_col].type == KING) {
            std::cout << (current_player == WHITE ? "White" : "Black") << " wins!" << std::endl;
            exit(0);
        }

        if (moving_piece.type == PAWN && (to_row == 0 || to_row == 7)) {
            PieceType promoted_piece_type = prompt_pawn_promotion();
            moving_piece.type = promoted_piece_type;
        }

        // Move the piece and set the original position to empty
        board[to_row][to_col] = moving_piece;
        board[from_row][from_col] = {EMPTY, moving_piece.player};
        current_player = current_player == WHITE ? BLACK : WHITE;
        return true;
    }
    bool is_in_check(Player player) {
        int king_row = -1;
        int king_col = -1;

        // 寻找当前玩家的国王
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (board[row][col].type == KING && board[row][col].player == player) {
                    king_row = row;
                    king_col = col;
                    break;
                }
            }
            if (king_row != -1) {
                break;
            }
        }

        // 检查对手的棋子是否有合法的捕获国王的移动
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (board[row][col].player != player) {
                    if (is_move_valid_for_piece(board[row][col], row, col, king_row, king_col)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
private:
    std::vector<std::vector<Piece>> board;
    Player current_player;

    PieceType prompt_pawn_promotion() {
        std::cout << "Pawn promotion! Choose a piece (q/r/b/n): ";
        char choice;
        std::cin >> choice;
        choice = std::tolower(choice);

        while (choice != 'q' && choice != 'r' && choice != 'b' && choice != 'n') {
            std::cout << "Invalid choice. Please choose a piece (q/r/b/n): ";
            std::cin >> choice;
            choice = std::tolower(choice);
        }

        switch (choice) {
            case 'q':
                return QUEEN;
            case 'r':
                return ROOK;
            case 'b':
                return BISHOP;
            case 'n':
                return KNIGHT;
            default:
                return EMPTY;
        }
    }

    void init_pieces(int row, Player player) {
        board[row][0] = {ROOK, player};
        board[row][1] = {KNIGHT, player};
        board[row][2] = {BISHOP, player};
        board[row][3] = {QUEEN, player};
        board[row][4] = {KING, player};
        board[row][5] = {BISHOP, player};
        board[row][6] = {KNIGHT, player};
        board[row][7] = {ROOK, player};
    }

    char piece_to_char(Piece piece) {
        std::map<PieceType, char> white_map = {
            {EMPTY, '.'}, {PAWN, 'P'}, {ROOK, 'R'}, {KNIGHT, 'N'},
            {BISHOP, 'B'}, {QUEEN, 'Q'}, {KING, 'K'}
        };
        std::map<PieceType, char> black_map = {
            {EMPTY, '.'}, {PAWN, 'p'}, {ROOK, 'r'}, {KNIGHT, 'n'},
            {BISHOP, 'b'}, {QUEEN, 'q'}, {KING, 'k'}
        };

        return piece.player == WHITE ? white_map[piece.type] : black_map[piece.type];
    }

    bool is_path_clear(int from_row, int from_col, int to_row, int to_col) {
        int row_step = from_row < to_row ? 1 : -1;
        int col_step = from_col < to_col ? 1 : -1;

        if (from_row == to_row) {
            row_step = 0;
        } else if (from_col == to_col) {
            col_step = 0;
        }
        // std::cout<<col_step<<std::endl;
        int row = from_row + row_step;
        int col = from_col + col_step;

        while (row != to_row || col != to_col) {
            // std::cout<<row<<','<<col<<std::endl;
            if (board[row][col].type != EMPTY) {
                return false;
            }
            row += row_step;
            col += col_step;
        }
        return true;
    }

    bool is_move_valid_for_piece(Piece piece, int from_row, int from_col, int to_row, int to_col) {
        int row_diff = std::abs(to_row - from_row);
        int col_diff = std::abs(to_col - from_col);

        switch (piece.type) {
            case PAWN: {
                // Consider player direction, starting position, and capture logic for pawns
                int direction = piece.player == WHITE ? -1 : 1;
                bool is_move_forward = to_row - from_row == direction;
                bool is_capture = board[to_row][to_col].type != EMPTY;

                if (col_diff == 0) {
                    if (is_move_forward) {
                        if (!is_capture) {
                            return true;
                        }
                    } else if (row_diff == 2 && !is_capture && (from_row == (piece.player == WHITE ? 6 : 1))) {
                        return is_path_clear(from_row, from_col, to_row, to_col);
                    }
                } else if (col_diff == 1 && is_move_forward && is_capture) {
                    return true;
                }
                return false;
            }
            case ROOK: {
                if (row_diff == 0 || col_diff == 0) {
                    return is_path_clear(from_row, from_col, to_row, to_col);
                }
                return false;
            }
            case KNIGHT: {
                return (row_diff == 1 && col_diff == 2) || (row_diff == 2 && col_diff == 1);
            }
            case BISHOP: {
                if (row_diff == col_diff) {
                    return is_path_clear(from_row, from_col, to_row, to_col);
                }
                return false;
            }
            case QUEEN: {
                if (row_diff == col_diff || row_diff == 0 || col_diff == 0) {
                    return is_path_clear(from_row, from_col, to_row, to_col);
                }
                return false;
            }
            case KING: {
                return row_diff <= 1 && col_diff <= 1;
            }
            default:
                return false;
        }
    }
    

};
int main() {
    ChessBoard board;
    board.print_board();

    while (true) {
        std::string from_coords, to_coords;
        std::cin >> from_coords >> to_coords;

        // 将字母和数字坐标转换为数组索引
        int from_col = from_coords[0] - 'a';
        int from_row_index = 8 - (from_coords[1] - '0');
        int to_col = to_coords[0] - 'a';
        int to_row_index = 8 - (to_coords[1] - '0');
        Player current_player = board.get_current_player();
        // std::cout<<current_player<<std::endl;
        Player opponent = current_player == WHITE ? BLACK : WHITE;
        if (board.move_piece(from_row_index, from_col, to_row_index, to_col)) {
            board.print_board();
            if (board.is_in_check(opponent)) {
                std::cout << (opponent == WHITE ? "White" : "Black") << " is in check!" << std::endl;
            }
        } else {
            std::cout << "Invalid move. Please try again." << std::endl;
        }
    }

    return 0;
}