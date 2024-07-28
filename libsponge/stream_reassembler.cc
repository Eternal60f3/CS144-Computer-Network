#include "stream_reassembler.hh"

#include <cassert>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in
// `stream_reassembler.hh`

template <typename... Targs> void DUMMY_CODE(Targs&&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _unassemble_strs(), _next_assembled_idx(0), _unassembled_bytes_num(0),
      _eof_idx(-1), _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string& data, const size_t index,
                                       const bool eof) {
    // 获取前一个子串
    auto pos_iter = _unassemble_strs.upper_bound(index);
    if (pos_iter != _unassemble_strs.begin())
        pos_iter--;

    // 处理当前子串和前面子串重叠的部分 注: 可能不存在前一个串
    size_t new_idx = index;
    if (pos_iter != _unassemble_strs.end() and pos_iter->first <= index) {
        const size_t up_idx = pos_iter->first;
        if (index < up_idx + pos_iter->second.size()) {
            new_idx = up_idx + pos_iter->second.size();
        }
    } else if (index < _next_assembled_idx) {
        new_idx = _next_assembled_idx;
    }

    // 子串新起始位置对应到的 data 索引
    const size_t data_start_pos = new_idx - index;
    // 当前子串将保存的 data 的长度
    ssize_t data_size = data.size() - data_start_pos;

    // 获取后一个子串
    if (pos_iter != _unassemble_strs.end() and pos_iter->first <= new_idx)
        ++pos_iter;

    // 处理_unassemble_strs后面的串与当前重叠的情况
    while (pos_iter != _unassemble_strs.end() and new_idx <= pos_iter->first) {
        const size_t data_end_pos = new_idx + data_size;
        if (pos_iter->first >= data_end_pos)
            break;

        if (data_end_pos < pos_iter->first + pos_iter->second.size()) {
            data_size = pos_iter->first - new_idx;
            break;
        } else {
            _unassembled_bytes_num -= pos_iter->second.size();
            pos_iter = _unassemble_strs.erase(pos_iter);
            continue;
        }
    }

    // 检测是否存在数据超出了容量。注意这里的容量并不是指可保存的字节数量，而是指可保存的窗口大小
    // NOTE: 注意这里我们仍然接收了 index 小于 first_unacceptable_idx  但
    //        index + data.size >= first_unacceptable_idx 的那部分数据
    //        这是因为处于安全考虑，最好减少算术运算操作以避免上下溢出
    //        同时多余的那部分数据最多也只会多占用 1kb 左右，属于可承受范围之内
    size_t first_unacceptable_idx =
        _next_assembled_idx + _capacity - _output.buffer_size();
    if (new_idx >= first_unacceptable_idx)
        return;

    // 判断是否还有数据是独立的，顺便检测当前子串是否被上一个子串完全包含
    if (data_size > 0) {
        const string new_data = data.substr(data_start_pos, data_size);
        // 如果新子串可以直接写入
        if (new_idx == _next_assembled_idx) {
            const size_t write_byte = _output.write(new_data);
            _next_assembled_idx += write_byte;
            if (write_byte < new_data.size()) {
                // _output 写不下了，插入进 _unassemble_strs 中
                const string data_to_store =
                    new_data.substr(write_byte, new_data.size() - write_byte);
                _unassembled_bytes_num += data_to_store.size();
                _unassemble_strs.insert(
                    make_pair(_next_assembled_idx, std::move(data_to_store)));
            }
        } else {
            const string data_to_store = new_data.substr(0, new_data.size());
            _unassembled_bytes_num += data_to_store.size();
            _unassemble_strs.insert(
                make_pair(new_idx, std::move(data_to_store)));
        }
    }

    // 处理之前存储的内容
    for (auto iter = _unassemble_strs.begin();
         iter != _unassemble_strs.end();) {
        assert(_next_assembled_idx <= iter->first);

        if (iter->first != _next_assembled_idx)
            break;

        const size_t write_num = _output.write(iter->second);
        _next_assembled_idx += write_num;
        // 没有完全写入
        if (write_num < iter->second.size()) {
            _unassembled_bytes_num += iter->second.size() - write_num;
            _unassemble_strs.insert(
                make_pair(_next_assembled_idx, iter->second.substr(write_num)));
            _unassembled_bytes_num -= iter->second.size();
            _unassemble_strs.erase(iter);
            break;
        } else { // 完全写入了
            _unassembled_bytes_num -= iter->second.size();
            iter = _unassemble_strs.erase(iter);
        }
    }

    if (eof)
        _eof_idx = index + data.size();

    if (_eof_idx <= _next_assembled_idx)
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const {
    return _unassembled_bytes_num;
}

bool StreamReassembler::empty() const { return _unassembled_bytes_num == 0; }
