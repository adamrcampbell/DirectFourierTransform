
#include <cstdlib>
#include <cstdio>
#include <gtest/gtest.h>

#include "direct_fourier_transform.h"
 
// Dummy tests to use as examples
// TEST(SquareRootTest, PositiveNos) { 
//     ASSERT_EQ(6, squareRoot(36.0));
//     ASSERT_EQ(18.0, squareRoot(324.0));
//     ASSERT_EQ(25.4, squareRoot(645.16));
//     ASSERT_EQ(0, squareRoot(0.0));
// }
// 
// TEST(SquareRootTest, NegativeNos) {
//     ASSERT_EQ(-1.0, squareRoot(-15.0));
//     ASSERT_EQ(-1.0, squareRoot(-0.2));
// }
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
