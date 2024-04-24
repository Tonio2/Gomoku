#include "timer/Timer.h"
#include "gtest/gtest.h"

TEST(TimerTest, Existence) {
  { Timer t("first_timer"); }

  double t = Timer::getAccumulatedTime("first_timer");
  EXPECT_LE(t, 0.1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
