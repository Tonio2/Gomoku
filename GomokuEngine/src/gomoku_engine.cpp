#include "gomoku_engine.h"

#include "gomoku_pattern_reconizer.h"

std::vector<std::string> structure_names = {
    "NONE",
    "FIVE_OR_MORE",
    "OPEN_ONE",
    "ONE",
    "OPEN_TWO",
    "TWO",
    "OPEN_THREE",
    "THREE",
    "OPEN_FOUR",
    "FOUR",
    "COUNT_STRUCTURE_TYPE"};

std::vector<std::string> player_names = {".", "X", "O"};

std::ostream &operator<<(std::ostream &stream, Player player)
{
    stream << player_names[player];
    return stream;
}

std::ostream &operator<<(std::ostream &stream, StructureType structure_type)
{
    stream << structure_names[structure_type];
    return stream;
}

// Definitions of GomokuGame methods
GomokuGame::GomokuGame(uint width, uint height)
    : board(width, height),
      current_player(X),
      players_scores({0, 0, 0}),
      winner(E),
      players_reconizers({
          GomokuPatternReconizer(E),
          GomokuPatternReconizer(X),
          GomokuPatternReconizer(O),
      })
{
    players_reconizers[X].find_patterns_in_board(*this);
    players_reconizers[O].find_patterns_in_board(*this);
}

GomokuGame::~GomokuGame() {}

Player GomokuGame::get_board_value(int row, int col) const
{
    return board(row, col);
}

CellChange GomokuGame::set_board_value(int row, int col, Player value)
{
    CellChange cell_change;

    cell_change.row = row;
    cell_change.col = col;

    cell_change.old_value = board(row, col);
    board(row, col) = value;
    cell_change.new_value = value;

    return cell_change;
}

Player GomokuGame::other_player(Player player) const
{
    return (player == X) ? O : X;
}

void GomokuGame::modify_player_score(Player player, int score)
{
    players_scores[player] += score;
}

void GomokuGame::display_board() const
{
    std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
    std::cout << " ";
    for (char coordinate : boardCoordinates)
    {
        std::cout << " " << coordinate;
    }
    std::cout << std::endl;
    for (int row = 0; row < get_board_height(); ++row)
    {
        std::cout << boardCoordinates[row] << " ";
        for (int col = 0; col < get_board_width(); ++col)
        {
            std::cout << get_board_value(row, col) << ' ';
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<int>> GomokuGame::get_patterns_count()
{
    TIMER
    std::vector<std::vector<int>> patterns_count(3, std::vector<int>(8, 0));
    patterns_count[1] = players_reconizers[X].get_pattern_count();
    patterns_count[2] = players_reconizers[O].get_pattern_count();
    return patterns_count;
}

void GomokuGame::print_patterns()
{
    players_reconizers[X].print_patterns();
    players_reconizers[O].print_patterns();
}

bool GomokuGame::is_game_over() const
{
    return winner != E;
}

bool GomokuGame::coordinates_are_valid(int row, int col) const
{
    return board.is_in_bound(row, col);
}

MoveResult GomokuGame::make_move(int row, int col)
{
    TIMER
    MoveResult move_result;
    const int old_black_score = get_player_score(X);
    const int old_white_score = get_player_score(O);

    const int old_open_three_count = players_reconizers[current_player].get_pattern_count()[StructureType::OPEN_THREE];

    if (!coordinates_are_valid(row, col))
    {
        throw std::invalid_argument("Invalid coordinates");
    }
    if (get_board_value(row, col) != E)
    {
        throw std::invalid_argument("Cell is already occupied");
    }

    bool captured = capture(row, col, current_player, move_result);

    move_result.black_score_change = get_player_score(X) - old_black_score;
    move_result.white_score_change = get_player_score(O) - old_white_score;

    const CellChange cell_change = set_board_value(row, col, current_player);
    move_result.cell_changes.push_back(cell_change);

    players_reconizers[X].update_patterns_with_move(*this, move_result);
    players_reconizers[O].update_patterns_with_move(*this, move_result);

    if (!captured)
    {
        const int new_open_three_count = players_reconizers[current_player].get_pattern_count()[StructureType::OPEN_THREE];
        if (new_open_three_count - old_open_three_count > 1)
        {
            Player p = current_player;
            reverse_move(move_result);
            current_player = p;
            throw std::invalid_argument("Invalid move: more than one open three");
        }
    }

    if (check_win(current_player))
        winner = current_player;

    current_player = other_player(current_player);

    return move_result;
}

void GomokuGame::reverse_move(const MoveResult &move)
{
    TIMER
    modify_player_score(X, -move.black_score_change);
    modify_player_score(O, -move.white_score_change);

    current_player = other_player(current_player);

    for (const CellChange &cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.old_value);
    }

    players_reconizers[X].update_patterns_with_move(*this, move);
    players_reconizers[O].update_patterns_with_move(*this, move);

    winner = E;
}

void GomokuGame::reapply_move(const MoveResult &move)
{
    modify_player_score(X, move.black_score_change);
    modify_player_score(O, move.white_score_change);

    for (const CellChange &cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.new_value);
    }

    players_reconizers[X].update_patterns_with_move(*this, move);
    players_reconizers[O].update_patterns_with_move(*this, move);

    CellChange cell = move.cell_changes.back(); // why ?

    if (check_win(current_player))
        winner = current_player;

    current_player = other_player(current_player);
}

std::vector<std::pair<std::pair<int, int>, int>> GomokuGame::findRelevantMoves() const
{
    TIMER
    std::vector<std::pair<std::pair<int, int>, int>> relevantMoves;

    // Directions to check around each cell (8 directions).
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (int row = 0; row < board.get_height(); ++row)
    {
        for (int col = 0; col < board.get_width(); ++col)
        {
            if (get_board_value(row, col) == E)
            { // Empty cell
                bool foundStone = false;
                for (const auto &dir : directions)
                {
                    for (int step = 1; step <= 2; ++step)
                    {
                        int newRow = row + step * dir.first;
                        int newCol = col + step * dir.second;

                        if (coordinates_are_valid(newRow, newCol) && get_board_value(newRow, newCol) != E)
                        {
                            relevantMoves.push_back({{row, col}, 0});
                            foundStone = true;
                            break; // No need to check further if one stone is found near this cell.
                        }
                    }
                    if (foundStone)
                    {
                        break; // No need to check further if one stone is found near this cell.
                    }
                }
            }
        }
    }

    return relevantMoves;
}

bool GomokuGame::try_direction_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult &move_result)
{
    Player otherPlayer = other_player(player);
    std::vector<Player> expected_value = {otherPlayer, otherPlayer, player};

    for (uint i = 1; i < 4; i++)
    {
        int x = row + i * row_dir;
        int y = col + i * col_dir;
        if (!coordinates_are_valid(x, y) or get_board_value(x, y) != expected_value[i - 1])
            return false;
    }
    move_result.cell_changes.push_back(
        set_board_value(row + row_dir, col + col_dir, E));

    move_result.cell_changes.push_back(
        set_board_value(row + 2 * row_dir, col + 2 * col_dir, E));

    modify_player_score(player, 2);
    return true;
}

bool GomokuGame::try_cardinal_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult &move_result)
{
    bool ret = try_direction_for_capture(row, col, row_dir, col_dir, player, move_result);
    ret |= try_direction_for_capture(row, col, -row_dir, -col_dir, player, move_result);

    return ret;
}

bool GomokuGame::capture(uint row, uint col, Player player, MoveResult &move_result)
{
    bool ret = false;
    ret |= try_cardinal_for_capture(row, col, 1, 0, player, move_result);
    ret |= try_cardinal_for_capture(row, col, 0, 1, player, move_result);
    ret |= try_cardinal_for_capture(row, col, 1, 1, player, move_result);
    ret |= try_cardinal_for_capture(row, col, 1, -1, player, move_result);
    return ret;
}

int GomokuGame::get_board_width() const
{
    return board.get_width();
}

int GomokuGame::get_board_height() const
{
    return board.get_height();
}

bool GomokuGame::check_win(Player player)
{
    if (get_player_score(player) >= 10)
    {
        return true;
    }
    else if (players_reconizers[player].get_pattern_count()
                 [StructureType::FIVE_OR_MORE])
    {
        return true;
    }
    return false;
}

Player GomokuGame::get_winner() const
{
    return winner;
}

Player GomokuGame::get_current_player() const
{
    return current_player;
}

int GomokuGame::get_player_score(Player player) const
{
    return players_scores[player];
}
