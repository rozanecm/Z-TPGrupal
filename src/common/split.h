#ifndef TP3TALLER_COMMON_SPLIT_H
#define TP3TALLER_COMMON_SPLIT_H

#include <vector>
#include <string>

/* Splits the input string in as many strings as possible, using the char
 * 'delim' as the delimiter between the result strings. Returns the smaller
 * strings in a vector. The function is guaranteed to return with at least
 * one string in the vector.
 */
std::vector<std::string> split(const std::string& input, char delim);

#endif //TP3TALLER_COMMON_SPLIT_H

