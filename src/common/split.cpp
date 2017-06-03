#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string& input, char delim) {
    std::vector<std::string> result;
    std::istringstream stream(input);
    for (std::string field; std::getline(stream, field, delim); ) {
        result.push_back(field);
    }
    return result;
}
