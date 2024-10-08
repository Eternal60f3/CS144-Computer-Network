#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream
//! (possibly out of order, possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.

    size_t _capacity;
    ByteStream _output;
    std::deque<char> buffer;
    std::deque<bool> bitmap;

    size_t _first_unass;
    size_t _unass_size;
    bool _eof;

    size_t remain_size() {
        return _capacity - _output.buffer_size() - _unass_size;
    }

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity`
    //! bytes. \note This capacity limits both the bytes that have been
    //! reassembled, and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the
    //! stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the
    //! `capacity`. Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte
    //! in `data` \param eof the last byte of `data` will be the last byte in
    //! the entire stream
    void push_substring(const std::string& data, const uint64_t index,
                        const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream& stream_out() const { return _output; }
    ByteStream& stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once,
    //! it should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;

    // 当前已经assemble等待read的bytes数量
    size_t hold_bytes() const;

    // 第一个unass_bytes的序列号(包括syn和fin)
    size_t first_unass_seq() const;

    // 当前已经assemble的最后一个字节的序列号(包括syn和fin)
    std::optional<uint64_t> last_ass_seq() const;

    bool is_end_input() const;
};

#endif // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
