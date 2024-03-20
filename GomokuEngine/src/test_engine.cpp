#include "utils.h"

void test_condition(bool condition, const std::string &message)
{
    // Print the description then print "...NOK" to std::cout in red color or "...OK" in green color if true
    std::string color = condition ? "\033[1;32m" : "\033[1;31m";
    std::string reset = "\033[0m";
    std::cout << color << message << (condition ? "...OK" : "...NOK") << reset << std::endl;
}

void test_capture()
{
    GomokuGame game(19, 19);
    std::string move_str = "25,34,16,07,66,75,57,48,97,A6,88,79,E4,F3,D5,C6,EA,F9,DB";
    std::vector<std::string> moves = split(move_str, ',');
    std::cout << "Testing capture in ↗: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "33,24,42,51,84,75,93,A2,C4,B5,D3,E2,F5,E6,G4,H3,3D,2E,4C";
    moves = split(move_str, ',');
    std::cout << "Testing capture in ↙: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "24,13,35,46,53,42,64,75,93,82,A4,B5,D3,C2,E4,F5,5D,4C,6E";
    moves = split(move_str, ',');
    std::cout << "Testing capture in ↘: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "5E,6F,4D,3C,7D,8E,6C,5B,9C,AD,8B,7A,BB,CC,AA,99,CA,DB,B9";
    moves = split(move_str, ',');
    std::cout << "Testing capture in ↖: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "24,25,23,22,54,55,53,52,84,85,83,82,B4,B5,B3,B2,E4,E5,E3";
    moves = split(move_str, ',');
    std::cout << "Testing capture in ←: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "33,32,34,35,53,52,54,55,73,72,74,75,93,92,94,95,B3,B2,B4";
    moves = split(move_str, ',');
    std::cout << "Testing capture in →: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "32,42,22,12,34,44,24,14,36,46,26,16,38,48,28,18,3A,4A,2A";
    moves = split(move_str, ',');
    std::cout << "Testing capture in ↑: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    game = GomokuGame(19, 19);
    move_str = "23,13,33,43,25,15,35,45,27,17,37,47,29,19,39,49,2B,1B,3B";
    moves = split(move_str, ',');
    std::cout << "Testing capture in ↓: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == true, move_str);

    // Capture is blocked by wall
    game = GomokuGame(19, 19);
    move_str = "25,34,16,07,66,75,57,48,97,A6,88,79,E4,F3,D5,C6,AH,BG,9I";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ↗: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "33,24,42,51,84,75,93,A2,C4,B5,D3,E2,F5,E6,G4,H3,11,02,20";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ↙: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "24,13,35,46,53,42,64,75,93,82,A4,B5,D3,C2,E4,F5,BH,AG,CI";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ↘: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "5E,6F,4D,3C,7D,8E,6C,5B,9C,AD,8B,7A,BB,CC,AA,99,41,52,30";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ↖: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "24,25,23,22,54,55,53,52,84,85,83,82,B4,B5,B3,B2,A1,A2,A0";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ←: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "33,32,34,35,53,52,54,55,73,72,74,75,93,92,94,95,AH,AG,AI";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in →: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "32,42,22,12,34,44,24,14,36,46,26,16,38,48,28,18,17,27,07";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ↑: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);

    game = GomokuGame(19, 19);
    move_str = "23,13,33,43,25,15,35,45,27,17,37,47,29,19,39,49,H8,G8,I8";
    moves = split(move_str, ',');
    std::cout << "Testing capture blocked by wall in ↓: " << std::endl;
    apply_moves(game, moves);
    test_condition(game.is_game_over() == false, move_str);
}

void test_engine()
{
    std::cout << "Testing engine..." << std::endl;
    std::cout << "Testing capture..." << std::endl;
    test_capture();
}