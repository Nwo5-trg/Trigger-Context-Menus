#include "Utils.hpp"

using namespace geode::prelude;

std::string ftofstr(float num, int decimal) {
    if (decimal == 0) return std::to_string((int)round(num));
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(decimal) << num;
    std::string string = ss.str();
    string.erase(string.find_last_not_of('0') + 1, std::string::npos);
    if (string.back() == '.') string.pop_back();
    return string;
}