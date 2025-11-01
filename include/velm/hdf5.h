#pragma once
#include <string_view>
#ifdef VELM_ENABLE_HDF5

#    include <cstddef>
#    include <cstring>
#    include <vector>

namespace H5 {
class H5File;
}
#endif

namespace velm {

using hdf5_dataset = int;

class hdf5_file {
  public:
    hdf5_file(const std::string_view & file_name);
    ~hdf5_file();

    // Move-only semantics
    hdf5_file(const hdf5_file &)             = delete;
    hdf5_file & operator=(const hdf5_file &) = delete;
    hdf5_file(hdf5_file && other) noexcept;
    hdf5_file & operator=(hdf5_file && other) noexcept;

    [[nodiscard]] std::vector<std::string_view> list_datasets() const;
    [[nodiscard]] std::vector<std::size_t>      get_dataset_shape(const std::string_view & name) const;

    [[nodiscard]] std::string_view get_filename() const { return filename_; }

    void load_dataset(void * buffer, const std::string_view & name) const;

  private:
    H5::H5File *     file_;
    std::string_view filename_;
};

}  // namespace velm
