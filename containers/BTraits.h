#ifndef BTREE_TRAITS_H
#define BTREE_TRAITS_H

#include <string>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>

template <typename keyType> struct BTreeTraits;
struct FileKey
{
    std::string filename;
    long        offset;   

    FileKey() : offset(0) {}
    FileKey(const std::string& f, long o = 0) : filename(f), offset(o) {}

    bool operator==(const FileKey& o) const { return filename == o.filename && offset == o.offset; }
    bool operator< (const FileKey& o) const { return filename < o.filename || (filename == o.filename && offset < o.offset); }
    bool operator> (const FileKey& o) const { return o < *this; }
    bool operator<=(const FileKey& o) const { return !(o < *this); }
    bool operator>=(const FileKey& o) const { return !(*this < o); }

    operator std::string() const { return filename + "@" + std::to_string(offset); }

    friend std::ostream& operator<<(std::ostream& os, const FileKey& k)
    { return os << k.filename << "@" << k.offset; }
};

template <typename keyType>
struct BTreeTraits
{
    using key_type       = keyType;
    static key_type invalid_key()    { return key_type{}; }
    static std::string to_string(const key_type& k)
    {
        std::ostringstream oss;
        oss << k;
        return oss.str();
    }

    static key_type from_stream(std::istream& is)
    {
        key_type k{};
        is >> k;
        return k;
    }

    static bool less(const key_type& a, const key_type& b) { return a < b; }
    static bool equal(const key_type& a, const key_type& b) { return a == b; }
};

template <>
struct BTreeTraits<int>
{
    using key_type = int;
    static key_type  invalid_key()                        { return -1; }
    static std::string to_string(const key_type& k)      { return std::to_string(k); }
    static key_type  from_stream(std::istream& is)       { key_type k; is >> k; return k; }
    static bool      less (const key_type& a, const key_type& b) { return a < b; }
    static bool      equal(const key_type& a, const key_type& b) { return a == b; }
};

template <>
struct BTreeTraits<long>
{
    using key_type = long;
    static key_type  invalid_key()                        { return -1L; }
    static std::string to_string(const key_type& k)      { return std::to_string(k); }
    static key_type  from_stream(std::istream& is)       { key_type k; is >> k; return k; }
    static bool      less (const key_type& a, const key_type& b) { return a < b; }
    static bool      equal(const key_type& a, const key_type& b) { return a == b; }
};

template <>
struct BTreeTraits<float>
{
    using key_type = float;
    static key_type  invalid_key()                        { return -1.0f; }
    static std::string to_string(const key_type& k)
    {
        std::ostringstream oss; oss << k; return oss.str();
    }
    static key_type  from_stream(std::istream& is)       { key_type k; is >> k; return k; }
    static bool      less (const key_type& a, const key_type& b) { return a < b; }
    static bool      equal(const key_type& a, const key_type& b) { return a == b; }
};
template <>
struct BTreeTraits<double>
{
    using key_type = double;
    static key_type  invalid_key()                        { return -1.0; }
    static std::string to_string(const key_type& k)
    {
        std::ostringstream oss; oss << k; return oss.str();
    }
    static key_type  from_stream(std::istream& is)       { key_type k; is >> k; return k; }
    static bool      less (const key_type& a, const key_type& b) { return a < b; }
    static bool      equal(const key_type& a, const key_type& b) { return a == b; }
};

template <>
struct BTreeTraits<std::string>
{
    using key_type = std::string;
    static key_type  invalid_key()                        { return ""; }
    static std::string to_string(const key_type& k)      { return k; }
    static key_type  from_stream(std::istream& is)       { key_type k; is >> k; return k; }
    static bool      less (const key_type& a, const key_type& b) { return a < b; }
    static bool      equal(const key_type& a, const key_type& b) { return a == b; }
};

template <>
struct BTreeTraits<const char*>
{
    using key_type = const char*;
    static key_type  invalid_key()                        { return nullptr; }
    static std::string to_string(const key_type& k)      { return k ? k : ""; }
    static key_type  from_stream(std::istream& is)
    {
        static std::string buf;
        is >> buf;
        return buf.c_str();
    }
    static bool less (const key_type& a, const key_type& b)
    { return a && b ? std::strcmp(a, b) < 0 : (b != nullptr); }
    static bool equal(const key_type& a, const key_type& b)
    { return a && b ? std::strcmp(a, b) == 0 : (a == b); }
};

template <>
struct BTreeTraits<FileKey>
{
    using key_type = FileKey;
    static key_type  invalid_key()                        { return FileKey("", -1); }
    static std::string to_string(const key_type& k)      { return k.filename + "@" + std::to_string(k.offset); }
    static key_type  from_stream(std::istream& is)
    {
        std::string fname; long off = 0;
        is >> fname >> off;
        return FileKey(fname, off);
    }
    static bool less (const key_type& a, const key_type& b) { return a < b; }
    static bool equal(const key_type& a, const key_type& b) { return a == b; }
};

template <typename T, typename = void>
struct has_btree_traits : std::false_type {};

template <typename T>
struct has_btree_traits<T,
    std::void_t<decltype(BTreeTraits<T>::invalid_key())>
> : std::true_type {};

#endif // BTREE_TRAITS_H




