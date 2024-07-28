#ifndef SPONGE_LIBSPONGE_BYTE_STREAM_HH
#define SPONGE_LIBSPONGE_BYTE_STREAM_HH

#include <cassert>
#include <cstddef>
#include <string>
#include <vector>

template <typename T> class Container {
  private:
    size_t arr_size;
    std::vector<T> arr;
    size_t capacity;
    int h, t;

  public:
    Container(int capacity_)
        : arr_size(capacity_ + 1), arr(arr_size), capacity(capacity_), h(0),
          t(0) {}

    size_t curr_stored() const { return (t + arr_size - h) % arr_size; }

    size_t remain_size() const { return capacity - curr_stored(); }

    bool is_full() const { return (t + 1) % static_cast<int>(arr_size) == h; }

    bool is_empty() const { return h == t; }

    void push(T data) {
        assert(!is_full());

        arr[t] = data;
        (++t) %= arr_size;
    }

    T pop() {
        assert(!is_empty());

        T ans = arr[h];
        (++h) %= arr_size;

        return ans;
    }

    std::vector<T> peek_output(size_t len) const {
        assert(len <= curr_stored());
        std::vector<T> ans;

        int th = h;
        size_t cnt = 0;
        while (cnt < len) {
            ++cnt;
            ans.push_back(arr[th]);
            (++th) %= arr_size;
        }
        return ans;
    }
};

//! \brief An in-order byte stream.

//! Bytes are written on the "input" side and read from the "output"
//! side.  The byte stream is finite: the writer can end the input,
//! and then no more bytes can be written.
class ByteStream {
  private:
    // Your code here -- add private members as necessary.
    // Hint: This doesn't need to be a sophisticated data structure at
    // all, but if any of your tests are taking longer than a second,
    // that's a sign that you probably want to keep exploring
    // different approaches.
    Container<char> container;
    bool is_end_input = false;
    bool _error{}; //!< Flag indicating that the stream suffered an error.
    int bytes_written_ = 0, bytes_read_ = 0;

  public:
    //! Construct a stream with room for `capacity` bytes.
    ByteStream(const size_t capacity);

    //! \name "Input" interface for the writer
    //!@{

    //! Write a string of bytes into the stream. Write as many
    //! as will fit, and return how many were written.
    //! \returns the number of bytes accepted into the stream
    size_t write(const std::string& data);

    //! \returns the number of additional bytes that the stream has space for
    size_t remaining_capacity() const;

    //! Signal that the byte stream has reached its ending
    void end_input();

    //! Indicate that the stream suffered an error.
    void set_error() { _error = true; }
    //!@}

    //! \name "Output" interface for the reader
    //!@{

    //! Peek at next "len" bytes of the stream
    //! \returns a string
    std::string peek_output(const size_t len) const;

    //! Remove bytes from the buffer
    void pop_output(const size_t len);

    //! Read (i.e., copy and then pop) the next "len" bytes of the stream
    //! \returns a string
    std::string read(const size_t len);

    //! \returns `true` if the stream input has ended
    bool input_ended() const;

    //! \returns `true` if the stream has suffered an error
    bool error() const { return _error; }

    //! \returns the maximum amount that can currently be read from the stream
    size_t buffer_size() const;

    //! \returns `true` if the buffer is empty
    bool buffer_empty() const;

    //! \returns `true` if the output has reached the ending
    bool eof() const;
    //!@}

    //! \name General accounting
    //!@{

    //! Total number of bytes written
    size_t bytes_written() const;

    //! Total number of bytes popped
    size_t bytes_read() const;
    //!@}
};

#endif // SPONGE_LIBSPONGE_BYTE_STREAM_HH
