#ifdef VELM_ENABLE_HDF5
#    include "velm/hdf5.h"
#    include "velm/ndarray.h"

#    include <gtest/gtest.h>

#    include <complex>
#    include <filesystem>
#    include <string>
#    include <vector>

namespace fs = std::filesystem;

class HDF5Test : public ::testing::Test {
  protected:
    static std::string_view GetTestFilePath() {
        // Construct the path to the test file
        static std::string path = (fs::path(__FILE__).parent_path() / "res" / "test_file.h5").string();
        return path;
    }
};

TEST_F(HDF5Test, FileOpening) {
    // Test that we can open the test file
    EXPECT_NO_THROW({ velm::hdf5_file file(GetTestFilePath()); });
}

TEST_F(HDF5Test, MoveSemantics) {
    // Test move constructor
    velm::hdf5_file file1(GetTestFilePath());
    velm::hdf5_file file2(std::move(file1));

    // Test that file2 has the content now
    EXPECT_EQ(file2.get_filename(), GetTestFilePath());

    // Test move assignment
    velm::hdf5_file file3(GetTestFilePath());
    file3 = std::move(file2);

    // Verify the content has moved
    EXPECT_EQ(file3.get_filename(), GetTestFilePath());
}

TEST_F(HDF5Test, ListDatasets) {
    velm::hdf5_file file(GetTestFilePath());

    // List all datasets in the file
    auto datasets = file.list_datasets();

    // Verify we have datasets
    EXPECT_FALSE(datasets.empty());

    // Log the found datasets for debugging
    std::cout << "Found datasets: ";
    for (const auto & ds : datasets) {
        std::cout << ds << " ";
    }
    std::cout << '\n';
}

TEST_F(HDF5Test, GetDatasetShape) {
    velm::hdf5_file file(GetTestFilePath());

    // Get all datasets
    auto datasets = file.list_datasets();

    // Check the shape of each dataset
    for (const auto & ds : datasets) {
        std::vector<std::size_t> shape = file.get_dataset_shape(ds);

        // Verify the shape is non-empty
        EXPECT_FALSE(shape.empty());

        // Log the shape for debugging
        std::cout << "Dataset " << ds << " shape: ";
        for (size_t dim : shape) {
            std::cout << dim << " ";
        }
        std::cout << '\n';
    }
}

TEST_F(HDF5Test, LoadDataset) {
    velm::hdf5_file file(GetTestFilePath());

    // Get all datasets
    auto datasets = file.list_datasets();

    // Try to load each dataset
    for (const auto & ds : datasets) {
        // Get dataset shape first
        std::vector<std::size_t> shape = file.get_dataset_shape(ds);
        if (shape.empty()) {
            continue;
        }

        // Calculate total number of elements
        size_t total_elements = 1;
        for (size_t dim : shape) {
            total_elements *= dim;
        }

        // Allocate buffer for the data
        // Note: We're using a generic buffer here since we don't know the dataset type
        // In real code, you'd need to know or determine the dataset type
        std::vector<char> buffer(total_elements * sizeof(double));

        // Load the dataset
        EXPECT_NO_THROW({ file.load_dataset(buffer.data(), ds); });
    }
}

// Compound data type test
TEST_F(HDF5Test, CompoundDataType) {
    velm::hdf5_file file(GetTestFilePath());

    // Look for the field dataset which contains complex data
    std::string_view dataset_name = "/background/field";

    try {
        // Get shape of the field dataset
        std::vector<std::size_t> shape = file.get_dataset_shape(dataset_name);
        if (!shape.empty()) {
            // Calculate total elements
            size_t total_elements = 1;
            for (size_t dim : shape) {
                total_elements *= dim;
            }

            // Allocate buffer for complex numbers (r and i components)
            std::vector<std::complex<double>> buffer(total_elements);

            // Load the dataset
            file.load_dataset(buffer.data(), dataset_name);

            // Verify data is loaded
            bool has_nonzero = false;
            for (const auto & val : buffer) {
                if (val.real() != 0.0 || val.imag() != 0.0) {
                    has_nonzero = true;
                    break;
                }
            }

            // We expect at least some non-zero values
            EXPECT_TRUE(has_nonzero);

            // Print some sample values for verification
            std::cout << "Sample complex values from " << dataset_name << ":\n";
            for (size_t i = 0; i < std::min(size_t(5), total_elements); ++i) {
                std::cout << buffer[i].real() << " + " << buffer[i].imag() << "i\n";
            }
        }
    } catch (const std::exception & e) {
        // Log and fail if there's an error
        std::cerr << "Exception testing compound data type: " << e.what() << '\n';
        FAIL() << "Exception during compound data type test: " << e.what();
    }
}

// Test error handling
TEST_F(HDF5Test, ErrorHandling) {
    // Test opening a non-existent file
    EXPECT_ANY_THROW({ velm::hdf5_file file(std::string_view("this_file_does_not_exist.h5")); });

    velm::hdf5_file file(GetTestFilePath());

    // Test getting shape of non-existent dataset
    EXPECT_ANY_THROW({ file.get_dataset_shape("non_existent_dataset"); });

    // Test loading non-existent dataset
    std::vector<char> buffer(10);
    EXPECT_ANY_THROW({ file.load_dataset(buffer.data(), "non_existent_dataset"); });
}

// Test for integration with ndarray
TEST_F(HDF5Test, NdarrayIntegration) {
    velm::hdf5_file file(GetTestFilePath());

    // Try to load data into ndarray
    try {
        // Look for a dataset
        auto datasets = file.list_datasets();
        if (!datasets.empty()) {
            std::string_view dataset_name = datasets[0];

            // Get shape
            std::vector<std::size_t> shape = file.get_dataset_shape(dataset_name);
            if (shape.size() == 1) {
                // Create ndarray with appropriate dimensions
                size_t                      dim = shape[0];
                velm_DR::ndarray<double, 1> arr(dim);

                // Load data
                file.load_dataset(arr.data, dataset_name);

                // Check that data was loaded
                bool has_nonzero = false;
                for (size_t i = 0; i < dim; ++i) {
                    if (arr(i) != 0.0) {
                        has_nonzero = true;
                        break;
                    }
                }

                // Print some sample values
                std::cout << "Sample values from " << dataset_name << " loaded into ndarray:\n";
                for (size_t i = 0; i < std::min(size_t(5), dim); ++i) {
                    std::cout << arr(i) << " ";
                }
                std::cout << '\n';
            } else if (shape.size() == 2) {
                // Create 2D ndarray
                size_t                      dim1 = shape[0];
                size_t                      dim2 = shape[1];
                velm_DR::ndarray<double, 2> arr(dim1, dim2);

                // Load data
                file.load_dataset(arr.data, dataset_name);

                // Check and print some sample values
                std::cout << "Sample values from " << dataset_name << " loaded into 2D ndarray:\n";
                for (size_t i = 0; i < std::min(size_t(2), dim1); ++i) {
                    for (size_t j = 0; j < std::min(size_t(2), dim2); ++j) {
                        std::cout << arr(i, j) << " ";
                    }
                    std::cout << '\n';
                }
            }
        }
    } catch (const std::exception & e) {
        std::cerr << "Exception in ndarray integration test: " << e.what() << '\n';
        FAIL() << "Exception during ndarray integration test: " << e.what();
    }
}
#endif
