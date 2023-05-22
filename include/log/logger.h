//
// Created by wegblad on 4/11/23.
//

#ifndef GYARB_LOGGER_H
#define GYARB_LOGGER_H

#include <string>
#include <cstring>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

static int s_width = 30;

class NamedLogger
{
public:
    explicit NamedLogger(std::string name) : _name(std::move(name)) {}

    void error()
    {
        makeLog(std::cerr, "Err");
        this->clear();
    }

    void log()
    {
        makeLog(std::cout, "Inf");
        this->clear();
    }
private:
    std::string _name;
    std::vector<char> _buffer;
public:

    NamedLogger& operator<<(const std::string&& s)
    {
        _buffer.insert(_buffer.end(), s.begin(), s.end());
        return *this;
    }

    void clear()
    {
        _buffer.clear();
    }

private:
    template<typename T_STREAM>
    void makeLog(T_STREAM& os, const char* context)
    {
        _buffer.push_back('\0');
        os << std::left
           << "Author: " << std::setw(s_width) << _name
           << "Thread: " << std::setw(s_width) << std::this_thread::get_id()
           << "[" << context << "] > " << _buffer.data();
        if (!std::string(_buffer.data()).ends_with("\n")) os << std::endl;
    }
};

template <class C>
class Logger : public NamedLogger {
public:
    explicit Logger() : NamedLogger(demangleName<C>()) {}

    template<class T>
    std::string demangleName()
    {
        std::string tmp_name_holder = typeid(T).name();
        size_t slice_at = 0;
        for (int i = 0; i < tmp_name_holder.size(); i++)
        {
            if (!std::isdigit(tmp_name_holder.at(i)))
            {
                slice_at = i;
                break;
            }
        }
        tmp_name_holder.replace(0, slice_at, "");
        return tmp_name_holder;
    }
};

#endif //GYARB_LOGGER_H
