
#include <cstdlib>
#include <cstdio>
#include <gtest/gtest.h>

#include "direct_fourier_transform.h"
 
TEST(DerpTest, EqualDerp)
{
    ASSERT_EQ(1.0, 1.0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
