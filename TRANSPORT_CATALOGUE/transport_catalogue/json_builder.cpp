// Вставьте сюда решение из предыдущего спринта
#include "json_builder.h"

using namespace std::literals;

namespace json{

Builder::Builder() : root_(), nodes_stack_{&root_} {}

Node Builder::Build() {
    
    return std::move(root_);
}

KeyContext Builder::Key(std::string key) {
    
    Node* back_ptr = nodes_stack_.back();
    if (!back_ptr->IsDict()) {
        throw std::logic_error("Not related type to Key() method");
    }
    Dict& dict_ref = std::get<Dict>(back_ptr->GetRawValue());
    nodes_stack_.emplace_back(&dict_ref[key]);
    return Context{*this};
}

Context Builder::Value(Node::Value value) {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Class Builder hasn't been created");
    }
    InputContainerOrElement(std::move(value));
    return *this;
     
}

DictContext Builder::StartDict() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Class Builder hasn't been created");
    }
    InputContainerOrElement(Dict{});
    return Context{*this};
}

ArrayContext Builder::StartArray() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Class Builder hasn't been created");
    }
    InputContainerOrElement(Array{});
    return Context{*this};
}

Context Builder::EndDict() {
    if (nodes_stack_.empty() || !(nodes_stack_.back()->IsDict())) {
        throw std::logic_error("Not dictionary related function");
    }
    nodes_stack_.pop_back();
    return *this;
}

Context Builder::EndArray() {
    if (nodes_stack_.empty() || !(nodes_stack_.back()->IsArray())) {
        throw std::logic_error("Not array related function");
    }
    nodes_stack_.pop_back();
    return *this;
}

void Builder::InputContainerOrElement(Node::Value elem) {
    bool is_container = (std::holds_alternative<Array>(elem) || std::holds_alternative<Dict>(elem));
    Node* back_ptr = nodes_stack_.back();
    if (back_ptr->IsArray()) {
        Node& n_ptr = const_cast<Array&>(back_ptr->AsArray()).emplace_back(elem);
        if (is_container) {
            nodes_stack_.push_back(&n_ptr);}
    } else {
        if (!std::holds_alternative<std::nullptr_t>(back_ptr->GetValue())) {
            throw std::logic_error("Trial to give a value to non-null item");
        }
        back_ptr->GetRawValue() = elem;
        if (!is_container) {
            nodes_stack_.pop_back();
        }
    }
}

Node Context::Build() {
    return builder_.Build();
}

KeyContext Context::Key(std::string key) {
    return builder_.Key(std::move(key));
}

Context Context::Value(Node::Value value) {
    return builder_.Value(std::move(value));
}

DictContext Context::StartDict() {
    return builder_.StartDict();
}

ArrayContext Context::StartArray() {
    return builder_.StartArray();
}

Context Context::EndDict() {
            return builder_.EndDict();
}

Context Context::EndArray() {
    return builder_.EndArray();
}

DictContext KeyContext::Value(Node::Value value) {
    return Context{Context::Value(std::move(value))};
}

ArrayContext ArrayContext::Value(Node::Value value) {
    return Context{Context::Value(std::move(value))};
}
} // end json namespace