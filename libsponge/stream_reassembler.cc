#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in
// `stream_reassembler.hh`

template <typename... Targs> void DUMMY_CODE(Targs&&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _eof_idx(-1), buffer_stored(0),
      first_unreassembler(0), buffer() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.

void StreamReassembler::push_substring(const string& data, const size_t index,
                                       const bool eof) {
    if (data.size() > remain_size()) {
        return;
    }

    Datagram curr = {index, index + data.size(), data};
    if (data.size() != 0) {
        curr.last--;
    }
    auto it = buffer.lower_bound(curr);
    if (curr.last < first_unreassembler) {
        return;
    }

    if (it == buffer.end() and curr.start >= first_unreassembler) {
        buffer.insert(curr);
        buffer_stored += data.size();
    } else {
        if (curr.last >= first_unreassembler) {
            curr.start = first_unreassembler;
            curr.data = data.substr(first_unreassembler - curr.start);
            it = buffer.lower_bound(curr);
        }

        auto it_l = (it == buffer.begin()) ? buffer.end() : --it;
        if (it != buffer.begin())
            ++it;

        if (it_l != buffer.end() and it_l->last > curr.start) {
            auto t = it_l->data.substr(0, curr.start - it_l->start);
            curr.data = t + curr.data;
            curr.start = it_l->start;
            buffer_stored -= it_l->data.size();
            buffer.erase(it_l);
        }

        while (it != buffer.end() and it->last < curr.last) {
            auto tmp = ++it;
            --it;
            buffer_stored -= it->data.size();
            buffer.erase(it);
            it = tmp;
        }

        if (it != buffer.end()) {
            auto t = it->data.substr(curr.last - it->start + 1);
            curr.data = curr.data + t;
            curr.last = it->last;
            buffer_stored -= it->data.size();
            buffer.erase(it);
        }

        buffer_stored += curr.data.size();
        buffer.insert(curr);
    }

    while (!buffer.empty() and first_unreassembler == buffer.begin()->start) {
        _output.write(buffer.begin()->data);
        first_unreassembler =
            buffer.begin()->start + buffer.begin()->data.size();
        buffer.erase(buffer.begin());
    }

    if (eof) {
        _eof_idx = index + data.size() - 1;
    }

    if (_eof_idx <= first_unreassembler) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return buffer_stored; }

bool StreamReassembler::empty() const { return buffer_stored == 0; }
