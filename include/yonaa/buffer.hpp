#pragma once

#include <cstring>
#include <string>
#include <vector>

namespace yonaa {

/// @brief A utility used to wrap normal buffer operations for convenience.
struct buffer {
    /// @brief Create an empty buffer.
    buffer() {}

    /// @brief Create a buffer using existing data.
    /// @param data The data to be contained by the buffer.
    /// @param size The number of bytes to be contained by the buffer.
    buffer(const char *data, size_t size) : data_(data, data + size) {}

    /// @brief Create a buffer of a specific size with no meaningful data.
    /// @param size The number of bytes to be contained by the buffer.
    explicit buffer(size_t size) { data_.resize(size); }

    /// @brief Create a buffer from an existing POD array.
    /// @tparam T The type of the elements in the POD array.
    /// @tparam N The number of elements in the POD array.
    /// @param data The POD array to be contained by the buffer.
    template<typename T, size_t N>
    explicit buffer(T (&data)[N]) : buffer(data, sizeof(T) * (N - 1)) {}

   public:
    /// @brief Return the data contained by this buffer.
    /// @return The data contained by this buffer.
    char *data() { return data_.data(); }

    /// @brief Return the data contained by this buffer.
    /// @return The data contained by this buffer.
    const char *data() const { return data_.data(); }

    /// @brief Return the number of bytes contained by this buffer.
    /// @return The number of bytes contained by this buffer.
    size_t size() const { return data_.size(); }

    /// @brief Return true if this buffer doesn't contain any data.
    /// @return True if this buffer doesn't contain any data.
    bool is_empty() const { return size() == 0; }

    /// @brief Zero out the underlying memory contained by this buffer.
    void zero() { std::memset(data(), 0, size()); }

    /// @brief Return true if this buffer contains any data, and false otherwise.
    operator bool() const { return size() > 0; }

    /// @brief Resize the storage for the data contained by this buffer.
    /// @param new_size The new size of the storage for the data contained by this buffer.
    void resize(size_t new_size) {
        data_.resize(new_size);
    }

    /// @brief Return the data contained by this buffer interpreted as the type specified by T.
    /// @tparam T The type that the data contained by this buffer should be interpreted as.
    /// @return The data contained by this buffer interpreted as the type specified by T.
    template<typename T>
    T *as() {
        return (T *)data();
    }

    /// @brief Return the data contained by this buffer interpreted as the type specified by T.
    /// @tparam T The type that the data contained by this buffer should be interpreted as.
    /// @return The data contained by this buffer interpreted as the type specified by T.
    template<typename T>
    const T *as() const {
        return (T *)data();
    }

    /// @brief Return the data contained by this buffer in string form.
    /// @return The data contained by this buffer in string form.
    std::string str() const { return std::string(data(), size()); }

    /// @brief Return true if this buffer contains the same data as the other buffer.
    /// @param other The other buffer in this comparison.
    /// @return True if this buffer contains the same data as the other buffer.
    bool operator==(const buffer &other) const {
        if (size() != other.size()) return false;

        return std::memcmp(data(), other.data(), size());
    }

   private:
    /// @brief The underlying storage for this buffer.
    std::vector<char> data_;
};

}  // namespace yonaa
