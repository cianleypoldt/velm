#include <string>
#include <string_view>
#if 1

#    include "velm/hdf5.h"

#    include <H5Cpp.h>
#    include <H5Fpublic.h>
#    include <H5Ppublic.h>
#    include <H5version.h>

namespace velm {

hdf5_file::hdf5_file(const std::string_view & file_name) : filename_(file_name) {
    file_ = new H5::H5File(std::string(file_name).c_str(), H5F_ACC_RDONLY);
}

hdf5_file::~hdf5_file() {
    delete file_;
}

hdf5_file::hdf5_file(hdf5_file && other) noexcept : file_(other.file_), filename_(other.filename_) {
    other.file_ = nullptr;

    other.filename_ = "";
}

hdf5_file & hdf5_file::operator=(hdf5_file && other) noexcept {
    if (this != &other) {
        delete file_;
        file_           = other.file_;
        filename_       = other.filename_;
        other.file_     = nullptr;
        other.filename_ = "";
    }
    return *this;
}

std::vector<std::string_view> hdf5_file::list_datasets() const {
    std::vector<std::string_view> dataset_names;
    H5::Group                     root = file_->openGroup("/");

    for (hsize_t i = 0; i < root.getNumObjs(); i++) {
        std::string obj_name = root.getObjnameByIdx(i);
        H5O_type_t  obj_type = root.childObjType(obj_name);
        if (obj_type == H5O_TYPE_DATASET) {
            dataset_names.push_back(obj_name);
        }
    }
    return dataset_names;
}

std::vector<std::size_t> hdf5_file::get_dataset_shape(const std::string_view & name) const {
    auto dataset = file_->openDataSet(std::string(name));
    dataset.getSpace();
    auto                     dataspace = dataset.getSpace();
    hsize_t                  rank      = dataspace.getSimpleExtentNdims();
    std::vector<std::size_t> shape(rank);
    std::vector<hsize_t>     h5_shape(rank);

    dataspace.getSimpleExtentDims(h5_shape.data(), nullptr);

    for (std::size_t i = 0; i < rank; ++i) {
        shape[i] = static_cast<std::size_t>(h5_shape[i]);
    }

    return shape;
}

void hdf5_file::load_dataset(void * buffer, const std::string_view & name) const {
    auto dataset = file_->openDataSet(std::string(name));

    auto datatype = dataset.getDataType();

    if (buffer) {
        dataset.read(buffer, datatype);
    }
}
};  // namespace velm
#endif
