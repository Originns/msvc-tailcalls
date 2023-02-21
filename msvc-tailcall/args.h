#pragma once
#include <cstdint>
#include <cstring>
#include <type_traits>

template <typename T>
struct type_size {
    static constexpr std::size_t value = sizeof(T);
};

template <>
struct type_size<void> { // msvc doesn't like sizeof(void)
    static constexpr std::size_t value = 0;
};

template <typename Ret, std::size_t N>
class Arguments {
public:
    template <typename T>
    __forceinline T* get(std::size_t index) {
        if (index >= N) return nullptr;
        if constexpr (type_size<Ret>::value > sizeof(std::uint64_t))
        {
            // return value of anything larger than 8 bytes 
            // will instead be caller allocated and
			// passed as pointer in rcx, so we need to
			// offset the index by 1
            index++;
        }

        if (std::is_floating_point<T>::value && index <= 3)
        {
            // handle floating point arguments
			// passed in xmm0-3
            return reinterpret_cast<T*>(&xmmdata[index]);
        }

        if constexpr (type_size<T>::value > sizeof(std::uint64_t))
        {
			// handle arguments larger than 8 bytes,
			// passed as pointers by the caller
            return *reinterpret_cast<T**>(&data_[index]);
        }

        return reinterpret_cast<T*>(&data_[index]);
    }
	
    template <typename = std::enable_if_t < (type_size<Ret>::value > sizeof(std::uint64_t)) >>
    __forceinline Ret* get_return_value() {
        return reinterpret_cast<Ret*>(data_[0]);
    }

private:
    std::uint64_t xmmdata[4];
	std::uint64_t reserved; // our return address, we shouldn't fuck with this...
    std::uint64_t data_[N];
};
