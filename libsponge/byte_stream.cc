#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in
// `byte_stream.hh`

template <typename... Targs> void DUMMY_CODE(Targs&&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : container(capacity) {}

size_t ByteStream::write(const string& data) {
    int cnt = 0;
    for (auto ch : data) {
        if (container.is_full()) {
            break;
        }
        container.push(ch);
        cnt++;
    }
    bytes_written_ += cnt;
    return cnt;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t can_peek = min(len, container.curr_stored());
    vector<char> tmp_ans = container.peek_output(can_peek);
    return string(tmp_ans.begin(), tmp_ans.end());
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > container.curr_stored()) {
        set_error();
        return;
    }

    size_t tmp = len;
    while (tmp > 0) {
        container.pop();
        tmp--;
    }
    bytes_read_ += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string ans;
    size_t tmp_len = len;
    int cnt = 0;
    while (tmp_len > 0 and !container.is_empty()) {
        ans += container.pop();
        ++cnt, --tmp_len;
    }
    bytes_read_ += cnt;
    return ans;
}

void ByteStream::end_input() { is_end_input = true; }

bool ByteStream::input_ended() const { return is_end_input; }

size_t ByteStream::buffer_size() const { return container.curr_stored(); }

bool ByteStream::buffer_empty() const { return container.is_empty(); }

bool ByteStream::eof() const { return is_end_input and container.is_empty(); }

size_t ByteStream::bytes_written() const { return bytes_written_; }

size_t ByteStream::bytes_read() const { return bytes_read_; }

size_t ByteStream::remaining_capacity() const {
    return container.remain_size();
}
