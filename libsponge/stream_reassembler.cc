#include "stream_reassembler.hh"
#include <cstddef>
#include <cstdint>

#include <cassert>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in
// `stream_reassembler.hh`

template <typename... Targs> void DUMMY_CODE(Targs&&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _capacity(capacity), _output(capacity), buffer(capacity, '\0'),
      bitmap(capacity, false), _first_unass(0), _unass_size(0), _eof(false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string& data, const size_t index,
                                       const bool eof) {
    bool t_eof = eof;

    if (index >= _first_unass) {
        size_t offset = index - _first_unass;
        size_t len = data.size(),
               t = _capacity - _output.buffer_size() - offset;
        if (len > t) {
            t_eof = false;
            len = t;
        }

        for (size_t i = 0; i < len; ++i) {
            if (bitmap[i + offset])
                continue;
            buffer[i + offset] = data[i];
            bitmap[i + offset] = true;
            _unass_size++;
        }
    } else if (index + data.size() > _first_unass) {
        size_t offset = _first_unass - index;
        size_t len = data.size() - offset,
               t = _capacity - _output.buffer_size();
        if (len > t) {
            _eof = false;
            len = t;
        }

        for (size_t i = 0; i < len; ++i) {
            if (bitmap[i])
                continue;
            buffer[i] = data[i + offset];
            bitmap[i] = true;
            _unass_size++;
        }
    }

    string tmp = "";
    while (bitmap.front()) {
        tmp += buffer.front();
        buffer.pop_front();
        bitmap.pop_front();
        buffer.push_back('\0');
        bitmap.push_back(false);
    }
    if (tmp.size() > 0) {
        _unass_size -= tmp.size();
        _first_unass += tmp.size();
        _output.write(tmp);
    }

    if (t_eof)
        _eof = true;

    if (_eof and _unass_size == 0) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unass_size; }

bool StreamReassembler::empty() const { return _unass_size == 0; }

size_t StreamReassembler::hold_bytes() const { return _output.buffer_size(); }

size_t StreamReassembler::first_unass_seq() const { return _first_unass + 1; }

optional<uint64_t> StreamReassembler::last_ass_seq() const {
    if (_first_unass != 0) {
        return _first_unass - 1;
    } else {
        return std::nullopt;
    }
}

bool StreamReassembler::is_end_input() const { return _output.input_ended(); }
