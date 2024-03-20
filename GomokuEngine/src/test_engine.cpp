#include "utils.h"

void test_condition(bool condition, const std::string &message)
{
    // Print the description then print "...NOK" to std::cout in red color or "...OK" in green color if true
    std::string color = condition ? "\033[1;32m" : "\033[1;31m";
    std::string reset = "\033[0m";
    std::cout << color << message << (condition ? "...OK" : "...NOK") << reset << std::endl;
}

void test_is_game_over(std::string move_str, bool expected_result, std::string description)
{
    GomokuGame game(19, 19);
    std::vector<std::string> moves = split(move_str, ',');
    apply_moves(game, moves);
    test_condition(game.is_game_over() == expected_result, description);
    std::cout << move_str << std::endl;
}

void test_move_is_possible(std::string move_str, bool expected_result, std::string description)
{
    GomokuGame game(19, 19);
    std::vector<std::string> moves = split(move_str, ',');
    bool failed = false;
    try
    {
        apply_moves(game, moves);
    }
    catch (std::invalid_argument &e)
    {
        failed = true;
    }
    test_condition(failed == !expected_result, description);
    std::cout << move_str << std::endl;
}

void test_capture()
{
    std::string move_str = "25,34,16,07,66,75,57,48,97,A6,88,79,E4,F3,D5,C6,EA,F9,DB";
    test_is_game_over(move_str, true, "Testing capture in ↗");

    move_str = "33,24,42,51,84,75,93,A2,C4,B5,D3,E2,F5,E6,G4,H3,3D,2E,4C";
    test_is_game_over(move_str, true, "Testing capture in ↙");

    move_str = "24,13,35,46,53,42,64,75,93,82,A4,B5,D3,C2,E4,F5,5D,4C,6E";
    test_is_game_over(move_str, true, "Testing capture in ↘");

    move_str = "5E,6F,4D,3C,7D,8E,6C,5B,9C,AD,8B,7A,BB,CC,AA,99,CA,DB,B9";
    test_is_game_over(move_str, true, "Testing capture in ↖");

    move_str = "24,25,23,22,54,55,53,52,84,85,83,82,B4,B5,B3,B2,E4,E5,E3";
    test_is_game_over(move_str, true, "Testing capture in ←");

    move_str = "33,32,34,35,53,52,54,55,73,72,74,75,93,92,94,95,B3,B2,B4";
    test_is_game_over(move_str, true, "Testing capture in →");

    move_str = "32,42,22,12,34,44,24,14,36,46,26,16,38,48,28,18,3A,4A,2A";
    test_is_game_over(move_str, true, "Testing capture in ↑");

    move_str = "23,13,33,43,25,15,35,45,27,17,37,47,29,19,39,49,2B,1B,3B";
    test_is_game_over(move_str, true, "Testing capture in ↓");

    // Capture is blocked by wall
    move_str = "25,34,16,07,66,75,57,48,97,A6,88,79,E4,F3,D5,C6,AH,BG,9I";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ↗");

    move_str = "33,24,42,51,84,75,93,A2,C4,B5,D3,E2,F5,E6,G4,H3,11,02,20";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ↙");

    move_str = "24,13,35,46,53,42,64,75,93,82,A4,B5,D3,C2,E4,F5,BH,AG,CI";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ↘");

    move_str = "5E,6F,4D,3C,7D,8E,6C,5B,9C,AD,8B,7A,BB,CC,AA,99,41,52,30";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ↖");

    move_str = "24,25,23,22,54,55,53,52,84,85,83,82,B4,B5,B3,B2,A1,A2,A0";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ←");

    move_str = "33,32,34,35,53,52,54,55,73,72,74,75,93,92,94,95,AH,AG,AI";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in →");

    move_str = "32,42,22,12,34,44,24,14,36,46,26,16,38,48,28,18,17,27,07";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ↑");

    move_str = "23,13,33,43,25,15,35,45,27,17,37,47,29,19,39,49,H8,G8,I8";
    test_is_game_over(move_str, false, "Testing capture blocked by wall in ↓");
}

void test_5_in_a_row()
{
    std::string move_str = "35,85,36,86,37,87,38,88,39";
    test_is_game_over(move_str, true, "Testing 5 in a row");

    move_str = "35,75,36,76,37,77,38,78,3A,7A,39";
    test_is_game_over(move_str, true, "Testing 6 in a row");

    move_str = "30,90,31,91,32,92,33,A3,34";
    test_is_game_over(move_str, true, "Testing 5 blocked by a left wall");

    move_str = "3E,8E,3F,8F,3G,8G,3H,8H,3I";
    test_is_game_over(move_str, true, "Testing 5 blocked by a right wall");

    move_str = "34,84,35,85,36,86,37,87,26,46,38";
    test_is_game_over(move_str, false, "Testing 5 in a row with potential capture");

    move_str = "34,84,35,85,36,86,37,45,23,87,38";
    test_is_game_over(move_str, false, "Testing 5 in a row with potential capture on extreme left");

    move_str = "34,84,35,85,36,86,37,46,28,83,33";
    test_is_game_over(move_str, false, "Testing 5 in a row with potential capture on extreme right");

    move_str = "34,84,35,85,36,86,37,87,39,46,26,9B,38";
    test_is_game_over(move_str, false, "Testing 6 in a row with potential capture");

    move_str = "13,63,14,64,15,65,16,66,05,25,17";
    test_is_game_over(move_str, true, "Testing 5 in a row with potential capture blocked by a wall");

    move_str = "77,55,78,76,79,66,44,69,66,7A,55,88,33";
    test_is_game_over(move_str, true, "Testing 5 in a row with potential capture blocked by a stone");
}

void test_double_3()
{
    std::string move_str = "44,A4,45,A5,56,CA,66,CB,46";
    test_move_is_possible(move_str, false, "Making a double 3 should fail");

    move_str = "43,93,44,94,45,C7,56,C8,66,C6,46";
    test_move_is_possible(move_str, true, "Making a fork with an open 4 and an open 3 should suceed");

    move_str = "43,35,44,25,55,B7,15,B8,65,FC,45";
    test_move_is_possible(move_str, true, "Making a double 3 with capture should suceed");
}

void test_draw()
{
    std::string move_str;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            char row = boardCoordinates[i];
            char col = boardCoordinates[j];
            move_str += row;
            move_str += col;
            move_str += ",";
        }
    }
    move_str.pop_back();

    test_is_game_over(move_str, true, "Should draw");
}

void test_engine()
{
    std::cout << "Testing engine..." << std::endl;
    std::cout << "Testing win by capture..." << std::endl;
    test_capture();
    std::cout << "Testing win by 5 in a row..." << std::endl;
    test_5_in_a_row();
    std::cout << "Testing double 3..." << std::endl;
    test_double_3();
    std::cout << "Testing draw..." << std::endl;
    test_draw();
}