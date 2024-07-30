#include "wrapping_integers.hh"
#include <cstdint>

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs> void DUMMY_CODE(Targs&&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a
//! WrappingInt32 \param n The input absolute 64-bit sequence number \param isn
//! The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    uint32_t tmp =
        static_cast<uint32_t>(static_cast<uint64_t>(isn.raw_value()) + n);
    return WrappingInt32{tmp};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number
//! (zero-indexed) \param n The relative sequence number \param isn The initial
//! sequence number \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to
//! `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One
//! stream runs from the local TCPSender to the remote TCPReceiver and has one
//! ISN, and the other stream runs from the remote TCPSender to the local
//! TCPReceiver and has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t U32_mod = static_cast<uint64_t>(UINT32_MAX) + 1;
    uint64_t tmp = (n - isn);

    if (tmp > checkpoint) {
        if (tmp > checkpoint and tmp - checkpoint >= U32_mod) {
            tmp -= (tmp - checkpoint) / U32_mod * U32_mod;
        }
        if (tmp >= U32_mod and
            checkpoint - (tmp - U32_mod) < tmp - checkpoint) {
            tmp -= U32_mod;
        }
    } else if (tmp < checkpoint) {
        if (tmp < checkpoint and checkpoint - tmp >= U32_mod) {
            tmp += (checkpoint - tmp) / U32_mod * U32_mod;
        }
        if (tmp + U32_mod <= UINT64_MAX and
            tmp + U32_mod - checkpoint < checkpoint - tmp) {
            tmp += U32_mod;
        }
    }

    return tmp;
}
