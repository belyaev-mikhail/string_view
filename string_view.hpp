#include <iostream>
#include <string>
#include <limits>

namespace essentials {

template<class Char, class Traits = std::char_traits<Char>>
class basic_string_view {
    const Char* data_ = nullptr;
    size_t size_ = 0;

    static size_t strlen(const Char* ch) noexcept(noexcept(Traits::length(ch))) {
        return Traits::length(ch);
    }

    template<class T>
    static constexpr void tswap(T& lhv, T& rhv) noexcept {
        auto&& tmp = lhv;
        lhv = rhv;
        rhv = tmp;
    }

    template<class T>
    static constexpr T min(T lhv, T rhv) noexcept {
        return lhv < rhv? lhv : rhv;
    }

public:
    using traits_type = Traits;
    using value_type = Char;
    using pointer = Char*;
    using const_pointer = const Char*;
    using reference = Char&;
    using const_reference = const Char&;

    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static constexpr size_type npos = ~size_type(0);

    constexpr basic_string_view() noexcept = default;
    constexpr basic_string_view(const basic_string_view&) noexcept = default;
    constexpr basic_string_view(const Char* str, size_t size) noexcept: data_(str), size_(size) {}
    basic_string_view(const Char* str) noexcept(noexcept(strlen(str))): data_(str), size_(strlen(str)) {}

    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

    constexpr iterator begin() const noexcept { return data_; }
    constexpr iterator end() const noexcept { return data_ + size_; }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr const_iterator cend() const noexcept { return end(); }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    constexpr const_reference operator[](size_type ix) const noexcept { return data_[ix]; }
    constexpr const_reference at(size_type ix) const {
        if(ix >= size_) throw std::out_of_range(__PRETTY_FUNCTION__);
        return data_[ix];
    }
    constexpr const_reference front() const noexcept { return *data_; }
    constexpr const_reference back() const noexcept { return *(data_ + (size_ - 1)); }
    constexpr const_pointer data() const noexcept { return data_; }

    constexpr size_type size() const noexcept { return size_; }
    constexpr size_type length() const noexcept { return size_; }
    constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr void remove_prefix(size_type n) {
        if(n > size_) throw std::out_of_range(__PRETTY_FUNCTION__);
        size_ -= n;
        data_ += n;
    }
    constexpr void remove_suffix(size_type n) {
        if(n > size_) throw std::out_of_range(__PRETTY_FUNCTION__);
        size_ -= n;
    }
    constexpr void swap(basic_string_view& other) noexcept {
        tswap(size_, other.size_);
        tswap(data_, other.data_);
    }

    size_type copy(Char* dest, size_type count, size_type pos = 0) const {
        if(pos > size_) throw std::out_of_range(__PRETTY_FUNCTION__);
        auto rcount = min(count, size_ - pos);
        auto rdata = data_ + pos;
        Traits::copy(dest, rdata, rcount);
        return rcount;
    }
    constexpr basic_string_view substr(size_type pos = 0, size_type count = npos ) const {
        if(pos > size_) throw std::out_of_range(__PRETTY_FUNCTION__);
        auto rcount = min(count, size_ - pos);
        auto rdata = data_ + pos;
        return basic_string_view(rdata, rcount);
    }

    constexpr int compare(basic_string_view v) const noexcept {
        auto rlen = min(size_, v.size_);
        auto chcomp = Traits::compare(data_, v.data_, rlen);
        return (chcomp != 0)? chcomp : int(size_ - v.size_);
    }
    constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const {
        return substr(pos1, count1).compare(v);
    }
    constexpr int compare(size_type pos1, size_type count1, basic_string_view v,
                      size_type pos2, size_type count2) const {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
    }
    constexpr int compare(const Char* s) const {
        return compare(basic_string_view(s));
    }
    constexpr int compare(size_type pos1, size_type count1, const Char* s) const {
        return substr(pos1, count1).compare(s);
    }
    constexpr int compare(size_type pos1, size_type count1,
                        const Char* s, size_type count2) const {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    friend constexpr bool operator==(basic_string_view lhv, basic_string_view rhv) noexcept {
        return lhv.size_ == rhv.size_ && lhv.compare(rhv) == 0;
    }

    constexpr size_type find(Char needle, size_type pos = 0) const {
        auto haystack = substr(pos);
        auto found = Traits::find(haystack.data_, haystack.size_, needle);
        return (found == nullptr)? npos : size_type(found - data_);
    }
    constexpr size_type find(basic_string_view needle, size_type pos = 0) const {
        if(pos >= size_) return npos;
        if(needle.empty()) return pos;
        auto chars = needle.size();
        while(true) {
            if(pos >= size_) return npos;
            auto pick = find(needle.front(), pos);
            if(pick == npos || pick + chars > size_) return npos;
            if(substr(pick, chars) == needle) return pick;
            pos = pick + 1;
        }
    }
    constexpr size_type find(const Char* s, size_type pos, size_type count) const {
        return find(basic_string_view(s, count), pos);
    }
    constexpr size_type find(const Char* s, size_type pos = 0) const {
        return find(basic_string_view(s), pos);
    }

    constexpr size_type rfind(Char needle, size_type pos = npos) const {
        pos = min(pos, size_);
        for(auto ix = size_type(0); ix < pos; ++ix)
            if(Traits::eq(at(pos - ix - 1), needle))
                return pos - ix - 1;
        return npos;
    }
    constexpr size_type rfind(basic_string_view needle, size_type pos = npos) const {
        pos = min(pos, size_);
        if(needle.empty()) return pos;
        auto chars = needle.size();
        while(true) {
            auto pick = rfind(needle.front(), pos);
            if(pick == npos) return npos;
            if(substr(pick, chars) == needle) return pick;
            if(pick == 0) return npos;
            pos = pick - 1;
        }
    }
    constexpr size_type rfind(const Char* s, size_type pos, size_type count) const {
        return rfind(basic_string_view(s, count), pos);
    }
    constexpr size_type rfind(const Char* s, size_type pos = npos) const {
        return rfind(basic_string_view(s), pos);
    }

    constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept {
        if(pos > size_) return npos;
        for(auto ix = pos; ix < size_; ++ix)
            if(v.find(operator[](ix)) != npos) return ix;
        return npos;
    }
    constexpr size_type find_first_of(Char c, size_type pos = 0) const noexcept {
        return find(c, pos);
    }
    constexpr size_type find_first_of(const Char* s, size_type pos, size_type count) const {
        return find_first_of(basic_string_view(s, count), pos);
    }
    constexpr size_type find_first_of(const Char* s, size_type pos = 0) const {
        return find_first_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept {
        pos = min(size_ - 1, pos);
        // we cannot use "pos > 0" here, cos pos is unsigned,
        // so we bet on underflow
        for(;pos < size_;--pos)
            if(v.find(operator[](pos)) != npos) return pos;
        return npos;
    }
    constexpr size_type find_last_of(Char c, size_type pos = npos) const noexcept {
        return rfind(c, pos);
    }
    constexpr size_type find_last_of(const Char* s, size_type pos, size_type count) const {
        return find_last_of(basic_string_view(s, count), pos);
    }
    constexpr size_type find_last_of(const Char* s, size_type pos = npos) const {
        return find_last_of(basic_string_view(s), pos);
    }

    constexpr size_type find_first_not_of(basic_string_view v, size_type pos = 0) const noexcept {
        if(pos > size_) return npos;
        for(auto ix = pos; ix < size_; ++ix)
            if(v.find(operator[](ix)) == npos) return ix;
        return npos;
    }
    constexpr size_type find_first_not_of(Char c, size_type pos = 0) const noexcept {
        return find_first_not_of(basic_string_view(&c, 1), pos);
    }
    constexpr size_type find_first_not_of(const Char* s, size_type pos, size_type count) const {
        return find_first_not_of(basic_string_view(s, count), pos);
    }
    constexpr size_type find_first_not_of(const Char* s, size_type pos = 0) const {
        return find_first_not_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept {
        pos = min(size_ - 1, pos);
        // we cannot use "pos > 0" here, cos pos is unsigned,
        // so we bet on underflow
        for(;pos < size_;--pos)
            if(v.find(operator[](pos)) == npos) return pos;
        return npos;
    }
    constexpr size_type find_last_not_of(Char c, size_type pos = npos) const noexcept {
        return find_last_not_of(basic_string_view(&c, 1), pos);
    }
        constexpr size_type find_last_not_of(const Char* s, size_type pos, size_type count) const {
            return find_last_not_of(basic_string_view(s, count), pos);
        }
    constexpr size_type find_last_not_of(const Char* s, size_type pos = npos) const {
        return find_last_not_of(basic_string_view(s), pos);
    }

    friend constexpr bool operator !=(basic_string_view lhv, basic_string_view rhv) noexcept {
        return not (lhv == rhv);
    }
# define COMPARE_TO_OP(OPC) \
    friend constexpr bool operator OPC(basic_string_view lhv, basic_string_view rhv) noexcept { \
        return lhv.compare(rhv) OPC 0; \
    }

    COMPARE_TO_OP(<)
    COMPARE_TO_OP(<=)
    COMPARE_TO_OP(>)
    COMPARE_TO_OP(>=)

# undef COMPARE_TO_OP

    // NON-STANDARD
    basic_string_view(const std::basic_string<Char, Traits>& str):
        data_(str.data()), size_(str.size()) {}
    explicit operator std::basic_string<Char, Traits>() const {
        return std::basic_string<Char, Traits>(data_, size_);
    }
};

template<class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, basic_string_view<Char, Traits> v) {
    auto width = os.width();
    auto size = v.size();
    auto left = (os.flags() & os.adjustfield) == os.left;
    auto putfill = [](std::basic_ostream<Char, Traits>& os, size_t amount) {
        os.width(amount); os << "";
    };
    if(size < size_t(width)) {
        auto diff = width - size;
        if(not left) putfill(os, diff);
        os.write(v.data(), v.size());
        if(left) putfill(os, diff);
    } else {
        os.write(v.data(), v.size());
    }
    os.width(0);
    return os;
}

constexpr basic_string_view<char> operator"" _sv(const char* s, size_t len) { return basic_string_view<char>(s, len); }

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;

} /* namespace essentials */

namespace fnv {
    template<size_t hashSize = sizeof(std::size_t)> struct fnv;
    template<>
    struct fnv<8> {
        enum: uint64_t {
            base = 1099511628211ULL,
            offset = 14695981039346656037ULL
        };
    };
    template<>
    struct fnv<4> {
        enum: uint32_t {
            base = 16777619UL,
            offset = 2166136261UL
        };
    };
} /* namespace fnv */

namespace std {
    template<>
    struct hash<essentials::string_view> {
        size_t operator()(essentials::string_view sv) const noexcept {
            /* fnv hash */
            size_t hash = fnv::fnv<>::offset;
            for(char c : sv) {
                hash *= fnv::fnv<>::base;
                hash ^= size_t(c);
            }
            return hash;
        }
    };

    template<>
    struct hash<essentials::wstring_view> {
        constexpr size_t operator()(essentials::wstring_view sv) const noexcept {
            /* djb2 hash */
            size_t hash = 5381;
            for(auto&& c: sv)
                hash = hash * 33 + (size_t)c;
            return hash;
        }
    };
} /* namespace std */
