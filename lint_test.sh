cpplint --filter=-legal/copyright,-runtime/explicit,-whitespace/line_length,-runtime/references --exclude=build/*  $(find . -type f -regex ".*\.\(c\|h\|hpp\|cpp\)")
