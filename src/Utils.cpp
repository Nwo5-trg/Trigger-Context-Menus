#include "Utils.hpp"

using namespace geode::prelude;

std::string floatToFormattedString(float num, int round) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(round) << num;
    std::string string = ss.str();
    string.erase(string.find_last_not_of('0') + 1, std::string::npos);
    if (string.back() == '.') string.pop_back();
    return string;
}