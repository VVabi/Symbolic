#include <exception>
#include <string>
#include <iostream>

class ParsingException: public std::exception {
private:
    std::string message;
    int position;
 public:
    ParsingException(const std::string &message, int position) {
        this->message = message;
        this->position = position;
    }

    const char * what () {
        return message.c_str();
    }

    int get_position() const {
        return position;
    }
};