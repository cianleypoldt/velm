#include "velm/ndarray.h"
#include "velm/velm.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <iostream>

using velm_DR::ndarray;

// Test default initialization and fill
TEST(NdArrayTest, InitializationAndFill) {
    ndarray<int, 4> arr4d(2, 3, 4, 2);
    ndarray<int, 3> arr3d(2, 3, 4);
    ndarray<int, 2> arr2d(4, 5);

    // default should be zero
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                for (int l = 0; l < 2; ++l) {
                    EXPECT_EQ(arr4d(i, j, k, l), 0);
                }
                EXPECT_EQ(arr3d(i, j, k), 0);
            }
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_EQ(arr2d(i, j), 0);
        }
    }

    // fill
    arr4d.fill(10);
    arr3d.fill(7);
    arr2d.fill(-3);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                for (int l = 0; l < 2; ++l) {
                    EXPECT_EQ(arr4d(i, j, k, l), 10);
                }
                EXPECT_EQ(arr3d(i, j, k), 7);
            }
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_EQ(arr2d(i, j), -3);
        }
    }
}

// Test offset_of_index
TEST(NdArrayTest, OffsetCalculation) {
    ndarray<int, 4> arr4d(2, 3, 4, 5);
    EXPECT_EQ(arr4d.offset_of_index(0, 0, 0, 0), 0);
    EXPECT_EQ(arr4d.offset_of_index(1, 0, 0, 0), 60);
    EXPECT_EQ(arr4d.offset_of_index(0, 1, 0, 0), 20);
    EXPECT_EQ(arr4d.offset_of_index(0, 0, 1, 0), 5);
    EXPECT_EQ(arr4d.offset_of_index(0, 0, 0, 1), 1);

    ndarray<int, 3> arr(2, 3, 4);
    EXPECT_EQ(arr.offset_of_index(0, 0, 0), 0);
    EXPECT_EQ(arr.offset_of_index(1, 0, 0), 12);
    EXPECT_EQ(arr.offset_of_index(0, 1, 0), 4);
    EXPECT_EQ(arr.offset_of_index(0, 0, 1), 1);

    ndarray<int, 2> arr2(4, 5);
    EXPECT_EQ(arr2.offset_of_index(0, 0), 0);
    EXPECT_EQ(arr2.offset_of_index(1, 0), 5);
    EXPECT_EQ(arr2.offset_of_index(0, 1), 1);
}

// Test copy constructor
TEST(NdArrayTest, CopyConstructor) {
    ndarray<int, 4> arr4d(2, 3, 4, 2);
    arr4d.fill(12);
    ndarray<int, 4> copy4d(arr4d);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                for (int l = 0; l < 2; ++l) {
                    EXPECT_EQ(copy4d(i, j, k, l), 12);
                }
            }
        }
    }

    ndarray<int, 3> arr(2, 3, 4);
    arr.fill(9);
    ndarray<int, 3> copy(arr);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                EXPECT_EQ(copy(i, j, k), 9);
            }
        }
    }
}

// Test move constructor
TEST(NdArrayTest, MoveConstructor) {
    ndarray<int, 4> arr4d(2, 3, 4, 2);
    arr4d.fill(8);
    ndarray<int, 4> moved4d(std::move(arr4d));
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                for (int l = 0; l < 2; ++l) {
                    EXPECT_EQ(moved4d(i, j, k, l), 8);
                }
            }
        }
    }
    EXPECT_EQ(arr4d.begin(), nullptr);

    ndarray<int, 3> arr(2, 3, 4);
    arr.fill(5);
    ndarray<int, 3> moved(std::move(arr));
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                EXPECT_EQ(moved(i, j, k), 5);
            }
        }
    }
    EXPECT_EQ(arr.begin(), nullptr);
}

// Test copy assignment
TEST(NdArrayTest, CopyAssignment) {
    ndarray<int, 4> arr4d(2, 2, 2, 2);
    arr4d.fill(15);
    ndarray<int, 4> copy4d(2, 2, 2, 2);
    (void) (copy4d = arr4d);  // suppress [[nodiscard]]
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                for (int l = 0; l < 2; ++l) {
                    EXPECT_EQ(copy4d(i, j, k, l), 15);
                }
            }
        }
    }

    ndarray<int, 2> arr(4, 5);
    arr.fill(11);
    ndarray<int, 2> copy(4, 5);
    (void) (copy = arr);  // suppress [[nodiscard]]
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_EQ(copy(i, j), 11);
        }
    }
}

// Test move assignment
TEST(NdArrayTest, MoveAssignment) {
    ndarray<int, 4> arr4d(2, 2, 2, 2);
    arr4d.fill(17);
    ndarray<int, 4> target4d(2, 2, 2, 2);
    (void) (target4d = std::move(arr4d));  // suppress [[nodiscard]]
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                for (int l = 0; l < 2; ++l) {
                    EXPECT_EQ(target4d(i, j, k, l), 17);
                }
            }
        }
    }
    EXPECT_EQ(arr4d.begin(), nullptr);

    ndarray<int, 2> arr(4, 5);
    arr.fill(13);
    ndarray<int, 2> target(4, 5);
    (void) (target = std::move(arr));  // suppress [[nodiscard]]
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_EQ(target(i, j), 13);
        }
    }
    EXPECT_EQ(arr.begin(), nullptr);
}

// Test at bounds checking
TEST(NdArrayTest, AtBoundsChecking) {
    ndarray<int, 4> arr4d(2, 2, 2, 2);
    EXPECT_DEATH((void) arr4d.at(2, 0, 0, 0), ".*");
    EXPECT_DEATH((void) arr4d.at(0, 2, 0, 0), ".*");
    EXPECT_DEATH((void) arr4d.at(0, 0, 2, 0), ".*");
    EXPECT_DEATH((void) arr4d.at(0, 0, 0, 2), ".*");

    ndarray<int, 3> arr(2, 2, 2);
    EXPECT_DEATH((void) arr.at(2, 0, 0), ".*");
    EXPECT_DEATH((void) arr.at(0, 2, 0), ".*");
    EXPECT_DEATH((void) arr.at(0, 0, 2), ".*");
}

// Test basic resize functionality
TEST(NdArrayTest, BasicResize) {
    // Test 2D resize
    ndarray<int, 2> arr2d(3, 4);
    arr2d.fill(5);

    // Check original dimensions
    EXPECT_EQ(arr2d.dims[0], 3);
    EXPECT_EQ(arr2d.dims[1], 4);

    // Resize to larger dimensions
    arr2d.resize(5, 6);

    // Verify new dimensions
    EXPECT_EQ(arr2d.dims[0], 5);
    EXPECT_EQ(arr2d.dims[1], 6);
    EXPECT_EQ(arr2d.total_elements(), 30);

    // Resize to smaller dimensions
    arr2d.resize(2, 3);

    // Verify new dimensions
    EXPECT_EQ(arr2d.dims[0], 2);
    EXPECT_EQ(arr2d.dims[1], 3);
    EXPECT_EQ(arr2d.total_elements(), 6);

    // Test 3D resize
    ndarray<float, 3> arr3d(2, 3, 4);
    arr3d.fill(1.5f);

    // Check original dimensions
    EXPECT_EQ(arr3d.dims[0], 2);
    EXPECT_EQ(arr3d.dims[1], 3);
    EXPECT_EQ(arr3d.dims[2], 4);

    // Resize 3D array
    arr3d.resize(3, 2, 5);

    // Verify new dimensions
    EXPECT_EQ(arr3d.dims[0], 3);
    EXPECT_EQ(arr3d.dims[1], 2);
    EXPECT_EQ(arr3d.dims[2], 5);
    EXPECT_EQ(arr3d.total_elements(), 30);
}

// Test that data is preserved correctly during resize
TEST(NdArrayTest, ResizeDataPreservation) {
    // Test 2D array
    ndarray<int, 2> arr2d(3, 4);

    // Fill with unique values
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            arr2d(i, j) = i * 10 + j;
        }
    }

    // Resize to larger dimensions
    arr2d.resize(5, 6);

    // Check that original data is preserved
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_EQ(arr2d(i, j), i * 10 + j);
        }
    }

    // Check that new elements are initialized to zero
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (i >= 3 || j >= 4) {
                EXPECT_EQ(arr2d(i, j), 0);
            }
        }
    }

    // Now resize to smaller dimensions
    ndarray<int, 2> arr2d_shrink(4, 5);

    // Fill with unique values
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            arr2d_shrink(i, j) = i * 100 + j;
        }
    }

    // Shrink the array
    arr2d_shrink.resize(2, 3);

    // Check that remaining data is preserved
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(arr2d_shrink(i, j), i * 100 + j);
        }
    }

    // Test 3D array
    ndarray<double, 3> arr3d(2, 3, 4);

    // Fill with unique values
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                arr3d(i, j, k) = i * 100 + j * 10 + k;
            }
        }
    }

    // Resize in various ways
    arr3d.resize(3, 2, 5);

    // Check that original data is preserved
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {  // Note: now only 2 in the middle dimension
            for (int k = 0; k < 4; ++k) {
                EXPECT_EQ(arr3d(i, j, k), i * 100 + j * 10 + k);
            }
        }
    }
}

// Test that indexing remains consistent after resizing
TEST(NdArrayTest, ResizeIndexingConsistency) {
    // Test 2D array
    ndarray<int, 2> arr2d(3, 4);

    // Fill with unique identifiable values
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            arr2d(i, j) = (i + 1) * 1000 + (j + 1);
        }
    }

    // Save some values for comparison
    int val_0_0 = arr2d(0, 0);
    int val_1_2 = arr2d(1, 2);
    int val_2_3 = arr2d(2, 3);

    // Resize larger
    arr2d.resize(5, 6);

    // Check that indexing remains consistent
    EXPECT_EQ(arr2d(0, 0), val_0_0);
    EXPECT_EQ(arr2d(1, 2), val_1_2);
    EXPECT_EQ(arr2d(2, 3), val_2_3);

    // Resize smaller, but still containing original test points
    arr2d.resize(4, 4);

    // Verify again
    EXPECT_EQ(arr2d(0, 0), val_0_0);
    EXPECT_EQ(arr2d(1, 2), val_1_2);
    EXPECT_EQ(arr2d(2, 3), val_2_3);

    // Test 3D array with multiple resizing operations
    ndarray<float, 3> arr3d(2, 3, 4);

    // Fill with unique identifiable values
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                arr3d(i, j, k) = (i + 1) * 10000.0f + (j + 1) * 100.0f + (k + 1);
            }
        }
    }

    // Save reference values
    float val3d_0_0_0 = arr3d(0, 0, 0);
    float val3d_1_1_1 = arr3d(1, 1, 1);
    float val3d_1_2_3 = arr3d(1, 2, 3);

    // First resize
    arr3d.resize(4, 3, 4);

    // Check indexing consistency
    EXPECT_EQ(arr3d(0, 0, 0), val3d_0_0_0);
    EXPECT_EQ(arr3d(1, 1, 1), val3d_1_1_1);
    EXPECT_EQ(arr3d(1, 2, 3), val3d_1_2_3);

    // Second resize
    arr3d.resize(4, 5, 6);

    // Check indexing consistency again
    EXPECT_EQ(arr3d(0, 0, 0), val3d_0_0_0);
    EXPECT_EQ(arr3d(1, 1, 1), val3d_1_1_1);
    EXPECT_EQ(arr3d(1, 2, 3), val3d_1_2_3);

    // Third resize - reduce size but keep test indices
    arr3d.resize(2, 3, 4);

    // Final consistency check
    EXPECT_EQ(arr3d(0, 0, 0), val3d_0_0_0);
    EXPECT_EQ(arr3d(1, 1, 1), val3d_1_1_1);
    EXPECT_EQ(arr3d(1, 2, 3), val3d_1_2_3);
}

// Test edge cases in resize
TEST(NdArrayTest, ResizeEdgeCases) {
    // Test resizing to zero in one dimension
    ndarray<int, 3> arr3d(2, 3, 4);
    arr3d.fill(42);

    // Resize with a zero dimension
    arr3d.resize(2, 0, 4);

    // Verify dimensions
    EXPECT_EQ(arr3d.dims[0], 2);
    EXPECT_EQ(arr3d.dims[1], 0);
    EXPECT_EQ(arr3d.dims[2], 4);
    EXPECT_EQ(arr3d.total_elements(), 0);

    // Test resizing back from zero
    arr3d.resize(2, 3, 4);

    // Verify new dimensions
    EXPECT_EQ(arr3d.dims[0], 2);
    EXPECT_EQ(arr3d.dims[1], 3);
    EXPECT_EQ(arr3d.dims[2], 4);
    EXPECT_EQ(arr3d.total_elements(), 24);

    // All elements should be zero-initialized
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                EXPECT_EQ(arr3d(i, j, k), 0);
            }
        }
    }

    // Test resize to same dimensions
    ndarray<int, 2> arr2d(3, 4);

    // Fill with identifiable values
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            arr2d(i, j) = i * 10 + j;
        }
    }

    // Get pointer to original data for comparison
    int * original_data = arr2d.data;

    // Resize to same dimensions
    arr2d.resize(3, 4);

    // Verify data has been preserved and copied to a new array
    // (implementation should allocate new memory even for same dimensions)
    EXPECT_NE(arr2d.data, original_data);

    // Verify all values are preserved
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_EQ(arr2d(i, j), i * 10 + j);
        }
    }
}
