#pragma once
#include <utility>
#include <sstream>
#include <vector>
#include "string_processing.h"

template <typename Iterator>
class IteratorRange {
public:

    IteratorRange(Iterator begin, Iterator end, int size)
        : begin_(begin), end_(end), size_(size)
    {
    }

    std::string PrintIteratorRange() const {
        std::ostringstream out;
        for (Iterator first = begin_; first != end_; ++first) {
            out << *first;
        }
        return out.str();
    }

private:
    Iterator begin_;
    Iterator end_;
    int size_;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& output, const IteratorRange<Iterator>& iterator_range) {
    return output << iterator_range.PrintIteratorRange();
}

template <typename Iterator>
class Paginator {
public:
    Paginator(const Iterator& c_begin, const Iterator& c_end, int size)
    {

        for (Iterator first = c_begin; first != c_end; advance(first, size)) {
            if (distance(first, c_end) < size) {
                size = distance(first, c_end);
            }
            pages_.push_back(IteratorRange<Iterator>(first, first + size, size));
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    auto size() const {
        return pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, int page_size) {
    return Paginator<typename Container::const_iterator>(begin(c), end(c), page_size);
}