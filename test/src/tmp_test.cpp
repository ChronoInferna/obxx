#include <gtest/gtest.h>

#include "obxx/order_book.hpp"

TEST(TmpAddTest, CheckValues)
{
  // ASSERT_EQ(obxx::add(1, 2), 3);
  EXPECT_TRUE(true);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
