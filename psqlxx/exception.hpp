#pragma once

#include <exception>


namespace psqlxx {

class Exception : public std::exception {
    std::string m_message;

public:
    explicit constexpr Exception(std::string message): m_message(std::move(message)) {
    }

    [[nodiscard]]
    virtual const char *what() const noexcept override {
        return m_message.c_str();
    }
};

class CommandException : public Exception {
public:
    using Exception::Exception;
};

}//namespace psqlxx
