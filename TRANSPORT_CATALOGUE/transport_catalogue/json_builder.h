// Вставьте сюда решение из предыдущего спринта
#pragma once
#include <string>
#include <vector>
#include "json.h"

namespace json {

    class Context;
    class KeyContext;
    class DictContext;
    class ArrayContext;
    
    class Builder {
            
        public:
            Builder();
            Node Build();
            KeyContext Key(std::string key);
            Context Value(Node::Value value);
            DictContext StartDict();
            ArrayContext StartArray();
            Context EndDict();
            Context EndArray();


        private:
            Node root_;
            std::vector<json::Node*> nodes_stack_;

            void InputContainerOrElement(Node::Value elem);

    };

    class Context {
    public:
        Context(Builder& builder) :builder_(builder) {};
        Node Build(); 
        KeyContext Key(std::string key);
        Context Value(Node::Value value) ;
        DictContext StartDict() ;
        ArrayContext StartArray() ;
        Context EndDict() ;
        Context EndArray() ;
    private:
        Builder& builder_;
    };

    class KeyContext :public Context {
    public:
        KeyContext(Context item) :Context(item) {};
        DictContext Value(Node::Value value) ;
        Node Build() = delete;
        KeyContext Key(std::string key) = delete;
        Context EndDict() = delete;
        Context EndArray() = delete;
    };

    class DictContext :public Context {
    public:
        DictContext(Context item) :Context(item) {};
        Node Build() = delete;
        Context Value(Node value) = delete;
        Context EndArray() = delete;
        DictContext StartDict() = delete;
        ArrayContext StartArray() = delete;
    };

    class ArrayContext :public Context {
    public:
        ArrayContext(Context item) :Context(item) {};
        ArrayContext Value(Node::Value value);
        Node Build() = delete;
        KeyContext Key(std::string key) = delete;
        Context EndDict() = delete;
    };

} // end json