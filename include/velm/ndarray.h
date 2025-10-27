#pragma once
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace velm_DR {

/*
 * vulnerabilities:
 *  1. Abort is the only error handling in place currently
 *  2. If malloc fails in assignment operators, memory could be leaked
 *
 */

template <typename T, std::size_t N> class ndarray {
  public:
    T * data = nullptr;

    std::size_t dims[N];
    std::size_t strides[N];

    template <typename... Idx> ndarray(Idx... idx);
    ~ndarray();

    template <typename... Idx> [[nodiscard]] T &       at(Idx... idx);
    template <typename... Idx> [[nodiscard]] const T & at(Idx... idx) const;

    template <typename... Idx> [[nodiscard]] std::size_t offset_of_index(const Idx &... idx) const;
    [[nodiscard]] std::size_t                            total_elements() const;

    template <typename... Idx> void resize(Idx... idx);

    void                    fill(T value);
    [[nodiscard]] T *       begin();
    [[nodiscard]] const T * begin() const;
    [[nodiscard]] T *       end();
    [[nodiscard]] const T * end() const;

    template <typename... Idx> [[nodiscard]] T &       operator()(Idx... idx);
    template <typename... Idx> [[nodiscard]] const T & operator()(Idx... idx) const;
    [[nodiscard]] ndarray &                            operator=(const ndarray & B);
    [[nodiscard]] ndarray &                            operator=(ndarray && B) noexcept;
    ndarray(const ndarray & B);
    ndarray(ndarray && B) noexcept;
};

template <typename T, std::size_t N> template <typename... Idx> ndarray<T, N>::ndarray(Idx... idx) {
    static_assert(sizeof...(Idx) == N, "Number of indices must match grid dimension");
    std::size_t indices[N] = { static_cast<std::size_t>(idx)... };
    for (std::size_t i = 0; i < N; ++i) {
        dims[i] = indices[i];
    }

    strides[N - 1] = 1;
    for (std::size_t i = N - 1; i > 0; --i) {
        strides[i - 1] = strides[i] * dims[i];
    }

    // allocate memory
    data = static_cast<T *>(malloc(total_elements() * sizeof(T)));
    if (data) {
        // Initialize memory to zero
        memset(data, 0, total_elements() * sizeof(T));
    } else {
        abort();
    }
}

template <typename T, std::size_t N> ndarray<T, N>::~ndarray() {
    free(data);
}

template <typename T, std::size_t N> template <typename... Idx> [[nodiscard]] T & ndarray<T, N>::at(Idx... idx) {
    std::size_t indices[N] = { static_cast<std::size_t>(idx)... };
    for (std::size_t i = 0; i < N; ++i) {
        if (indices[i] >= dims[i]) {
            abort();
        }
    }
    return data[offset_of_index(idx...)];
}

template <typename T, std::size_t N> template <typename... Idx> const T & ndarray<T, N>::at(Idx... idx) const {
    std::size_t indices[N] = { static_cast<std::size_t>(idx)... };
    for (std::size_t i = 0; i < N; ++i) {
        if (indices[i] >= dims[i]) {
            abort();
        }
    }
    return data[offset_of_index(idx...)];
}

template <typename T, std::size_t N> template <typename... Idx>
std::size_t ndarray<T, N>::offset_of_index(const Idx &... idx) const {
    static_assert(sizeof...(Idx) == N, "Number of indices must match grid dimension");
    std::size_t indices[N] = { static_cast<std::size_t>(idx)... };
    std::size_t offset     = 0;
    for (std::size_t i = 0; i < N; ++i) {
        offset += indices[i] * strides[i];
    }
    return offset;
}

template <typename T, std::size_t N> std::size_t ndarray<T, N>::total_elements() const {
    std::size_t element_count = 1;
    for (std::size_t i = 0; i < N; ++i) {
        element_count *= dims[i];
    }
    return element_count;
}

template <typename T, std::size_t N> T * ndarray<T, N>::begin() {
    return data;
}

template <typename T, std::size_t N> const T * ndarray<T, N>::begin() const {
    return data;
}

template <typename T, std::size_t N> T * ndarray<T, N>::end() {
    return data + total_elements();
}

template <typename T, std::size_t N> const T * ndarray<T, N>::end() const {
    return data + total_elements();
}

template <typename T, std::size_t N> void ndarray<T, N>::fill(T value) {
    T * p = begin();
    for (std::size_t i = 0; i < total_elements(); ++i) {
        *(p++) = value;
    }
}

template <typename T, std::size_t N> template <typename... Idx> void ndarray<T, N>::resize(Idx... idx) {
    static_assert(sizeof...(Idx) == N, "Number of indices must match grid dimension");
    std::size_t indices[N] = { static_cast<std::size_t>(idx)... };
    std::size_t old_count  = total_elements();
    std::size_t new_count  = 1;

    // calculate new dimensions and strides
    std::size_t new_dims[N];
    std::size_t new_strides[N];

    for (std::size_t i = 0; i < N; ++i) {
        new_dims[i] = indices[i];
        new_count *= new_dims[i];
    }

    new_strides[N - 1] = 1;
    for (std::size_t i = N - 1; i > 0; --i) {
        new_strides[i - 1] = new_strides[i] * new_dims[i];
    }

    // allocate new memory
    T * new_mem = static_cast<T *>(malloc(new_count * sizeof(T)));
    if (!new_mem) {
        abort();
    }
    // Zero-initialize memory
    memset(new_mem, 0, new_count * sizeof(T));

    // Copy existing data to new memory
    if (data != nullptr) {
        // copy elements that exist in both old and new arrays
        for (std::size_t i = 0; i < old_count; ++i) {
            // convert flat index to multi-dimensional indices
            std::size_t remaining = i;
            std::size_t indices_old[N];

            for (std::size_t dim = 0; dim < N; ++dim) {
                indices_old[dim] = remaining / strides[dim];
                remaining %= strides[dim];
            }

            // check if indices are within bounds of new dimensions
            bool in_bounds = true;
            for (std::size_t dim = 0; dim < N; ++dim) {
                if (indices_old[dim] >= new_dims[dim]) {
                    in_bounds = false;
                    break;
                }
            }

            // if within bounds, copy to new array
            if (in_bounds) {
                std::size_t new_offset = 0;
                for (std::size_t dim = 0; dim < N; ++dim) {
                    new_offset += indices_old[dim] * new_strides[dim];
                }
                new_mem[new_offset] = data[i];
            }
        }

        // Delete old data
        free(data);
    }

    // Update member variables
    data = new_mem;
    for (std::size_t i = 0; i < N; ++i) {
        dims[i]    = new_dims[i];
        strides[i] = new_strides[i];
    }
}

template <typename T, std::size_t N> template <typename... Idx> T & ndarray<T, N>::operator()(Idx... idx) {
    return data[offset_of_index(idx...)];
}

template <typename T, std::size_t N> template <typename... Idx> const T & ndarray<T, N>::operator()(Idx... idx) const {
    return data[offset_of_index(idx...)];
}

template <typename T, std::size_t N> ndarray<T, N> & ndarray<T, N>::operator=(const ndarray & B) {
    // self-assignment check
    if (this != &B) {
        bool same_dims = true;
        for (std::size_t i = 0; i < N && same_dims; ++i) {
            if (dims[i] != B.dims[i]) {
                same_dims = false;
            }
        }

        // if dimensions are different, deallocate and reallocate
        if (!same_dims) {
            free(data);
            for (std::size_t i = 0; i < N; ++i) {
                dims[i]    = B.dims[i];
                strides[i] = B.strides[i];
            }
            data = static_cast<T *>(malloc(total_elements() * sizeof(T)));
            if (!data) {
                abort();
            }
        }

        // copy the data
        for (std::size_t i = 0; i < total_elements(); ++i) {
            data[i] = B.data[i];
        }
    }
    return *this;
}

template <typename T, std::size_t N> ndarray<T, N> & ndarray<T, N>::operator=(ndarray && B) noexcept {
    if (this != &B) {
        free(data);

        for (std::size_t i = 0; i < N; ++i) {
            dims[i]    = B.dims[i];
            strides[i] = B.strides[i];
        }

        // transfer ownership of data
        data   = B.data;
        B.data = nullptr;
    }
    return *this;
}

template <typename T, std::size_t N> ndarray<T, N>::ndarray(const ndarray & grid_b) {
    for (std::size_t i = 0; i < N; ++i) {
        dims[i]    = grid_b.dims[i];
        strides[i] = grid_b.strides[i];
    }
    data = static_cast<T *>(malloc(total_elements() * sizeof(T)));
    if (!data) {
        abort();
    }
    for (std::size_t i = 0; i < total_elements(); ++i) {
        data[i] = grid_b.data[i];
    }
}

template <typename T, std::size_t N> ndarray<T, N>::ndarray(ndarray && grid_b) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
        dims[i]    = grid_b.dims[i];
        strides[i] = grid_b.strides[i];
    }
    data        = grid_b.data;
    grid_b.data = nullptr;
}

};  // namespace velm_DR
