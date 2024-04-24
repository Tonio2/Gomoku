#include "engine/gomoku_engine.h"
#include "utils/gomoku_utilities.h"
#include "gtest/gtest.h"

void test_game_status(std::string move_str, bool expected_game_over,
                      Player winner = Player::EMPTY) {
  GomokuGame game(19, 19);

  apply_moves(game, move_str);

  if (expected_game_over)
    EXPECT_TRUE(game.is_game_over());
  else
    EXPECT_FALSE(game.is_game_over());

  if (winner != Player::EMPTY)
    EXPECT_EQ(game.get_winner(), winner);
}

TEST(EngineTest, Creation) {
  GomokuGame game(10, 10);

  ASSERT_FALSE(game.is_game_over());
  ASSERT_EQ(game.get_winner(), Player::EMPTY);
  EXPECT_EQ(game.get_board_width(), 10);
  EXPECT_EQ(game.get_board_height(), 10);
}

TEST(EngineTest, ValidCoordinatesTest) {
  GomokuGame game(10, 10);

  EXPECT_FALSE(game.coordinates_are_valid(-1, -1));
  EXPECT_FALSE(game.coordinates_are_valid(10, -1));
  EXPECT_FALSE(game.coordinates_are_valid(-1, 10));
  EXPECT_FALSE(game.coordinates_are_valid(10, 10));
  EXPECT_TRUE(game.coordinates_are_valid(0, 0));
  EXPECT_TRUE(game.coordinates_are_valid(9, 0));
  EXPECT_TRUE(game.coordinates_are_valid(0, 9));
  EXPECT_TRUE(game.coordinates_are_valid(9, 9));
}

TEST(GomokuGameTest, MakeMoveTest) {
  GomokuGame game(10, 10);

  MoveResult result = game.make_move(5, 5);

  ASSERT_EQ(result.cell_changes.size(), 1);
  ASSERT_EQ(result.cell_changes[0].row, 5);
  ASSERT_EQ(result.cell_changes[0].col, 5);
  ASSERT_EQ(result.cell_changes[0].old_value, Player::EMPTY);
  ASSERT_EQ(result.cell_changes[0].new_value, Player::BLACK);
  ASSERT_EQ(game.get_current_player(), Player::WHITE);
}

TEST(GomokuGameTest, MakeMoveInvalidCoordinatesTest) {
  GomokuGame game(10, 10);

  ASSERT_THROW(game.make_move(-1, 5), std::invalid_argument);
}

TEST(GomokuGameTest, MakeMoveOccupiedCellTest) {
  GomokuGame game(10, 10);
  game.make_move(5, 5);

  ASSERT_THROW(game.make_move(5, 5), std::invalid_argument);
}

TEST(CaptureTest, InUpRightDirection) {
  std::string move_str =
      "25,34,16,07,66,75,57,48,97,A6,88,79,E4,F3,D5,C6,EA,F9,DB";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InDownLeftDirection) {
  std::string move_str =
      "33,24,42,51,84,75,93,A2,C4,B5,D3,E2,F5,E6,G4,H3,3D,2E,4C";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InDownRightDirection) {
  std::string move_str =
      "24,13,35,46,53,42,64,75,93,82,A4,B5,D3,C2,E4,F5,5D,4C,6E";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InUpLeftDirection) {
  std::string move_str =
      "5E,6F,4D,3C,7D,8E,6C,5B,9C,AD,8B,7A,BB,CC,AA,99,CA,DB,B9";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InLeftDirection) {
  std::string move_str =
      "24,25,23,22,54,55,53,52,84,85,83,82,B4,B5,B3,B2,E4,E5,E3";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InRightDirection) {
  std::string move_str =
      "33,32,34,35,53,52,54,55,73,72,74,75,93,92,94,95,B3,B2,B4";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InUpDirection) {
  std::string move_str =
      "32,42,22,12,34,44,24,14,36,46,26,16,38,48,28,18,3A,4A,2A";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, InDownDirection) {
  std::string move_str =
      "23,13,33,43,25,15,35,45,27,17,37,47,29,19,39,49,2B,1B,3B";
  test_game_status(move_str, true, Player::WHITE);
}

TEST(CaptureTest, BlockedByWall_InUpRightDirection) {
  std::string move_str =
      "25,34,16,07,66,75,57,48,97,A6,88,79,E4,F3,D5,C6,AH,BG,9I";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InDownLeftDirection) {
  std::string move_str =
      "33,24,42,51,84,75,93,A2,C4,B5,D3,E2,F5,E6,G4,H3,11,02,20";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InDownRightDirection) {
  std::string move_str =
      "24,13,35,46,53,42,64,75,93,82,A4,B5,D3,C2,E4,F5,BH,AG,CI";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InUpLeftDirection) {
  std::string move_str =
      "5E,6F,4D,3C,7D,8E,6C,5B,9C,AD,8B,7A,BB,CC,AA,99,41,52,30";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InLeftDirection) {
  std::string move_str =
      "24,25,23,22,54,55,53,52,84,85,83,82,B4,B5,B3,B2,A1,A2,A0";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InRightDirection) {
  std::string move_str =
      "33,32,34,35,53,52,54,55,73,72,74,75,93,92,94,95,AH,AG,AI";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InUpDirection) {
  std::string move_str =
      "32,42,22,12,34,44,24,14,36,46,26,16,38,48,28,18,17,27,07";
  test_game_status(move_str, false);
}

TEST(CaptureTest, BlockedByWall_InDownDirection) {
  std::string move_str =
      "23,13,33,43,25,15,35,45,27,17,37,47,29,19,39,49,H8,G8,I8";
  test_game_status(move_str, false);
}

TEST(FiveInRowTest, WinWithFive) {
  std::string move_str = "35,85,36,86,37,87,38,88,39";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, ShouldDetectFiveInRow) {
  std::string move_str = "77,55,78,76,79,54,56,65,53,54,87,67,97,43,32,7A,58,"
                         "68,85,A7,57,67,59,5A,55";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, TestSixInRow) {
  std::string move_str = "35,75,36,76,37,77,38,78,3A,7A,39";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, TestFiveInRowBlockedByLeftWall) {
  std::string move_str = "30,90,31,91,32,92,33,A3,34";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, TestFiveInRowBlockedByRightWall) {
  std::string move_str = "3E,8E,3F,8F,3G,8G,3H,8H,3I";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, TestFiveInRowWithPotentialCapture) {
  std::string move_str = "34,84,35,85,36,86,37,87,26,46,38";
  test_game_status(move_str, false);
}

TEST(FiveInRowTest, TestNotDetectFiveInRowWhenCaptureIsPossible) {
  std::string move_str = "99,88,98,9A,79,A9,AB,8B,7C,97,A6,7A,8A,9C,69,59,6B,"
                         "99,B9,BE,AD,9B,7B,9B,8D,98";
  test_game_status(move_str, false);
}

TEST(FiveInRowTest, TestFiveInRowWithPotentialCaptureOnExtremeLeft) {
  std::string move_str = "34,84,35,85,36,86,37,45,23,87,38";
  test_game_status(move_str, false);
}

TEST(FiveInRowTest, TestFiveInRowWithPotentialCaptureOnExtremeRight) {
  std::string move_str = "34,84,35,85,36,86,37,46,28,83,33";
  test_game_status(move_str, false);
}

TEST(FiveInRowTest, TestSixInRowWithPotentialCapture) {
  std::string move_str = "34,84,35,85,36,86,37,87,39,46,26,9B,38";
  test_game_status(move_str, false);
}

TEST(FiveInRowTest, TestFiveInRowWithPotentialCaptureBlockedByWall) {
  std::string move_str = "13,63,14,64,15,65,16,66,05,25,17";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, TestFiveInRowWithPotentialCaptureBlockedByStone) {
  std::string move_str = "77,55,78,76,79,66,44,69,66,7A,55,88,33";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(FiveInRowTest, TestShouldWinIfFiveInRowAndOpponentWinByCaptureNextTurn) {
  std::string move_str = "88,66,89,87,8A,65,67,76,64,65,98,78,A8,54,43,8B,69,"
                         "79,96,B8,A9,78,7A,77,75,58,7A,6B,95,97,94,AA,B7,7B,"
                         "5B,9B,AB,A6,B5,97,C6,A7,93,92,A4,D5,D7,A8,C6";
  test_game_status(move_str, true, Player::BLACK);
}

TEST(weirdWinCondition, ShouldNotWinIfPossibleCapture){
    std::string move_str = "99,77,9A,98,9C,9B,CC,99,F9,88,9A,AA,97,99,8C,BB,A9,"
                           "8B,B8,C7,66,C8" test_game_status(move_str, false)}

TEST(DoubleThreeTest, TestMakeDoubleThreeThrowError) {
  GomokuGame game(19, 19);

  std::string move_str = "44,A4,45,A5,56,CA,66,CB";
  apply_moves(game, move_str);

  ASSERT_THROW(game.make_move(4, 6), std::invalid_argument);
}

TEST(DoubleThreeTest, TestMakingForkWithOpenFourAndOpenThreeShouldSucceed) {
  GomokuGame game(19, 19);

  std::string move_str = "43,93,44,94,45,C7,56,C8,66,C6";
  apply_moves(game, move_str);

  ASSERT_NO_THROW({ game.make_move(4, 6); });
}

TEST(DoubleThreeTest, TestMakingDoubleThreeWithCaptureShouldSucceed) {
  GomokuGame game(19, 19);

  std::string move_str = "43,35,44,25,55,B7,15,B8,65,FC";
  apply_moves(game, move_str);

  ASSERT_NO_THROW({ game.make_move(4, 5); });
}

TEST(DrawTest, FillAllBoardShouldDraw_SquareBoard) {
  int w = 10;
  int h = 10;
  GomokuGame game(w, h);

  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      int o = c;
      if (r % 4 == 3)
        o = (c + 1) % w;
      game.make_move(r, o);
    }
  }

  ASSERT_TRUE(game.is_game_over());
  ASSERT_EQ(game.get_winner(), Player::EMPTY);
}

TEST(DrawTest, FillAllBoardShouldDraw_HeightBoard) {
  int w = 7;
  int h = 11;
  GomokuGame game(w, h);

  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      int o = c;
      if (r % 4 == 3)
        o = (c + 1) % w;
      game.make_move(r, o);
    }
  }

  ASSERT_TRUE(game.is_game_over());
  ASSERT_EQ(game.get_winner(), Player::EMPTY);
}

TEST(DrawTest, FillAllBoardShouldDraw_WidthBoard) {
  int w = 11;
  int h = 7;
  GomokuGame game(w, h);

  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      int o = c;
      if (r % 4 == 3)
        o = (c + 1) % w;
      game.make_move(r, o);
    }
  }

  ASSERT_TRUE(game.is_game_over());
  ASSERT_EQ(game.get_winner(), Player::EMPTY);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
