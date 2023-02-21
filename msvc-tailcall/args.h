#pragma once
#include <cstddef>
#include <type_traits>
#include <array>

template <typename T>
struct type_size
{
    static constexpr std::size_t value = sizeof(T);
};

template <>
struct type_size<void> // msvc doesn't like sizeof(void)
{
    static constexpr std::size_t value = 0;
};

template <typename Ret, std::size_t N>
class Arguments
{
public:
    template <typename T, std::size_t index>
    inline constexpr T *get()
    {
        // https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170
        static_assert(index < N, "Index out of bounds");

        // return value of anything larger than 8 bytes
        // will instead be allocated by the caller and
        // passed as pointer in rcx, so we need to
        // shift the arguments by one
        constexpr std::size_t real_index = (type_size<Ret>::value > sizeof(std::uint64_t)) ? (index + 1) : index;

        if constexpr (std::is_floating_point<T>::value && real_index < 4)
        {
            // handle floating point arguments
            // passed in xmm0-3
            return reinterpret_cast<T *>(&xmmdata.at(real_index));
        }

        if constexpr (type_size<T>::value > sizeof(std::uint64_t))
        {
            // handle arguments larger than 8 bytes,
            // passed as pointers by the caller
            return *reinterpret_cast<T **>(&data_.at(real_index));
        }

        return reinterpret_cast<T *>(&data_.at(real_index));
    }

    template <typename = std::enable_if_t<(type_size<Ret>::value > sizeof(std::uint64_t))>>
    inline Ret *get_return_value()
    {
        return reinterpret_cast<Ret *>(data_.at(0));
    }

private:
    std::array<std::uint64_t, 4> xmmdata;
    std::uint64_t reserved; // our return address
    std::array<std::uint64_t, (type_size<Ret>::value > sizeof(std::uint64_t)) ? (N + 1) : N> data_;
};
