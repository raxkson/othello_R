#include "msg.h"

std::vector<std::string> split(std::string& str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string temp;

    while (std::getline(ss, temp, delimiter)) {
        internal.push_back(temp);
    }

    return internal;
}

std::string gen_move_msg(std::string& move, std::string& token)
{
    std::ostringstream oss;
    oss << "move\n" << "move:" << move << "\n" << "token:" << token;
    return oss.str();
}

std::pair<std::string, std::map<std::string, std::string> >
parse_msg(std::string& msg)
{
    std::map<std::string, std::string> data;
    auto line = split(msg, '\n');
    auto code = line[0];
    for (int i = 1; i < line.size(); ++i) {
        auto kv = split(line[i], ':');
        data[kv[0]] = kv[1];
    }
    return std::make_pair(code, data);
}
