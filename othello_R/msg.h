//#ifndef MSG_H
//#define MSG_H
#pragma once
#include <string>
#include <sstream>
#include <tuple>
#include <map>
#include <vector>
#include <utility>

std::vector<std::string> split(std::string& str, char delimiter);

std::string gen_move_msg(std::string& move, std::string& token);

std::pair<std::string, std::map<std::string, std::string> >
parse_msg(std::string& msg);

//#endif // MSG_H
