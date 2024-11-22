#pragma once
#include <utility>
#include <deque>
#include "search_server.h"
#include "document.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);   

    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

protected:
    struct QueryResult {
        std::string raw_query;
        long long counter;
    };

    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    long long counter_ = 0;
};

template <typename DocumentPredicate>
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate)  {
        ++counter_;
        std::vector<Document> search_result = search_server_.FindTopDocuments(raw_query, document_predicate);
        if (search_result.empty()) {
           requests_.push_back({ raw_query, counter_ });
        }
        while (requests_[0].counter <= counter_ - 1440) {
            requests_.pop_front();
        }
        return search_result;
    }