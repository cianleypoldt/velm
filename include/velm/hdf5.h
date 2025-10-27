#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#ifdef USE_MOCK_HDF5
// Forward declaration for mock implementation
struct H5FileWrapper;
#else
// Forward declaration for H5 namespace
namespace H5 {
class H5File;
}
#endif

namespace velm {

using hdf5_dataset = int;

class hdf5_file {
  public:
    explicit hdf5_file(const char * filename);
    ~hdf5_file();

    // Move-only semantics
    hdf5_file(const hdf5_file &)             = delete;
    hdf5_file & operator=(const hdf5_file &) = delete;
    hdf5_file(hdf5_file && other) noexcept;
    hdf5_file & operator=(hdf5_file && other) noexcept;

    [[nodiscard]] std::vector<std::string> list_datasets() const;
    [[nodiscard]] std::vector<std::size_t> get_dataset_shape(const char * name) const;

    [[nodiscard]] const char * get_filename() const { return filename_; }

    void load_dataset(void * buffer, const char * name) const;

  private:
#ifdef USE_MOCK_HDF5
    H5FileWrapper * file_;
#else
    H5::H5File * file_;
#endif
    const char * filename_;
};

}  // namespace velm
