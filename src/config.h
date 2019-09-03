#ifndef CONFIG_H
#define CONFIG_H

//补充缺少的函数
/**
 * #if ((__cplusplus >= 201103L) && defined(_GLIBCXX_USE_C99)
 * namespace std{
 *  stoa
 *  to_string
 * }
 * #endif
 */
#ifndef _GLIBCXX_USE_C99

#include <string>
std::string to_string(int32_t val);
std::string to_string(int64_t val);
std::string to_string(uint32_t val);
std::string to_string(uint64_t val);

int stoi(const std::string& s, std::size_t* pos = 0, int base = 10);

#endif

#endif