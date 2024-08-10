#include "tcp_receiver.hh"
#include "wrapping_integers.hh"
#include <cstddef>
#include <cstdint>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs> void DUMMY_CODE(Targs&&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment& seg) {
    // 如果没有收到syn，则返回零，否则返回第一位未接受的数据序列号
    uint64_t self_start = _had_receive_syn ? _reassembler.first_unass_seq() : 0;
    uint64_t self_last = self_start + window_size();

    if (!_isn.has_value()) {
        if (seg.header().syn) {
            _isn = seg.header().seqno;
            _had_receive_syn = true;
        } else {
            return;
        }
    }

    uint64_t seqno =
        unwrap(seg.header().seqno, _isn.value(), _last_reass_abs.value_or(0));
    uint64_t last_seqno =
        unwrap(seg.header().seqno + seg.length_in_sequence_space(),
               _isn.value(), _last_reass_abs.value_or(0));

    if ((self_start <= seqno and seqno < last_seqno) or
        (self_start < last_seqno and last_seqno <= self_last)) {
        // 传入的index是数据的index, 不包含syn和fin
        // 如果传入的是空字符串，不会做任何处理，只会将fin传入其中
        /*
        不能用seg.payload().str().data(),
           因为data()返回的是原始字符串指针，失去了长度等信息，导致如果数据中存在\0会忽视\0后面的内容
        */
        _reassembler.push_substring(string(seg.payload().str()),
                                    seqno != 0 ? seqno - 1 : 0,
                                    seg.header().fin);

        _last_reass_abs = _reassembler.last_ass_seq();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_had_receive_syn) {
        return nullopt;
    } else {
        // 如果是结束输出了，需要加上fin所占的序列号
        return wrap(_reassembler.first_unass_seq() +
                        _reassembler.is_end_input(),
                    _isn.value());
    }
}

size_t TCPReceiver::window_size() const {
    return _capacity - _reassembler.hold_bytes();
}
