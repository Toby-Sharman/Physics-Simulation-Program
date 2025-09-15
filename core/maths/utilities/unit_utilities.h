//
// Created by Tobias Sharman on 11/09/2025.
//

#ifndef SIMULATION_PROGRAM_UNIT_UTILITIES_H
#define SIMULATION_PROGRAM_UNIT_UTILITIES_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

struct Dimension {
    std::map<std::string,int> exponents; // e.g. {{"M",1},{"L",2},{"T",-2}}
    Dimension(const std::initializer_list<std::pair<const std::string,int>> init)
    : exponents(init) {}

    static Dimension dimensionless() { return {}; }

    bool operator==(const Dimension& other) const { return exponents == other.exponents; }
    bool operator!=(const Dimension& other) const { return !(*this == other); }

    Dimension operator+(const Dimension& other) const {
        Dimension res = *this;
        for (auto& [k,v] : other.exponents) res.exponents[k] += v;
        return res.removeEmpty();
    }

    Dimension operator-(const Dimension& other) const {
        Dimension res = *this;
        for (auto& [k,v] : other.exponents) res.exponents[k] -= v;
        return res.removeEmpty();
    }

    Dimension& removeEmpty() {
        for (auto it = exponents.begin(); it != exponents.end(); ) {
            it = (it->second == 0) ? exponents.erase(it) : std::next(it);
        }
        return *this;
    }
};

struct UnitInfo {
    double scale{};       // Factor relative to SI (For use in units like eV)
    Dimension dimension;  // Dimension type based on SI
};

inline std::string UTF8_CHAR(const std::string& s, size_t& i) {
    const unsigned char c = s[i];
    size_t len = 1;
    if ((c & 0xE0) == 0xC0) len = 2;
    else if ((c & 0xF0) == 0xE0) len = 3;
    else if ((c & 0xF8) == 0xF0) len = 4;
    std::string res = s.substr(i, len);
    i += len;
    return res;
}

inline int UNICODE_DIGIT_VALUE(const std::string& s) {
    static const std::unordered_map<std::string,int> superscripts = {
        {"⁰",0},{"¹",1},{"²",2},{"³",3},{"⁴",4},
        {"⁵",5},{"⁶",6},{"⁷",7},{"⁸",8},{"⁹",9},
        {"⁻",-1} // negative sign
    };
    const auto it = superscripts.find(s);
    return (it != superscripts.end()) ? it->second : -2; // -2 = not a superscript
}

// Return numerical exponent - Limited to integers
inline int parseExponent(const std::string& token) {
    // 1. ^n or ^-n
    if (const size_t pos = token.find('^'); pos != std::string::npos)
        return std::stoi(token.substr(pos + 1));

    // 2. Unicode superscripts
    int exp = 0;
    bool negative = false;
    size_t i = 0;
    while (i < token.size() && static_cast<unsigned char>(token[i]) < 128) i++; // skip ASCII base
    bool found = false;
    while (i < token.size()) {
        std::string ch = UTF8_CHAR(token,i);
        if (const int val = UNICODE_DIGIT_VALUE(ch); val == -1) negative = true; // superscript minus
        else if (val >= 0) { exp = exp*10 + val; found = true; } // *10 to shift each digit (e.g. to get 12 10*1+2 etc.)
        else break;
    }
    if (found) return negative ? -exp : exp;

    // 3. Trailing digits like s2 or s-12
    i = token.size();
    while (i > 0 && std::isdigit(token[i-1])) i--;
    if (i < token.size()) {
        const size_t start = (i > 0 && token[i-1]=='-') ? i-1 : i;
        return std::stoi(token.substr(start));
    }

    return 1; // default
}

inline std::string stripExponent(const std::string& token) {
    if (const size_t pos = token.find('^'); pos != std::string::npos) return token.substr(0,pos); // ^ powers

    size_t i = token.size();
    while (i>0) {
        if (const unsigned char c = token[i-1]; std::isdigit(c) || c=='-') { i--; continue; }             // Trailing ASCII
        size_t j=i-1; while(j>0 && ((token[j]&0xC0)==0x80)) j--;                                          // UTF-8 start
        if (std::string ch = token.substr(j,i-j); UNICODE_DIGIT_VALUE(ch) != -2) i=j;
        else break;
    }
    return token.substr(0,i);
}

inline std::vector<std::string> splitTerms(const std::string& s) {
    std::vector<std::string> tokens;
    size_t start = 0;
    while (start < s.size()) {
        while (start < s.size() && std::isspace(s[start])) start++;
        if (start >= s.size()) break;

        size_t end = start;
        while (end < s.size() && !std::isspace(s[end]) && s[end] != '*') end++;

        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    return tokens;
}

inline void multiplyUnit(UnitInfo& result, const UnitInfo& u, const int exp=1) {
    result.scale *= std::pow(u.scale,exp);
    Dimension d = u.dimension;
    for (int i=1;i<std::abs(exp);++i) d = d + u.dimension;
    result.dimension = (exp>0) ? result.dimension + d : result.dimension - d;
}

#endif //SIMULATION_PROGRAM_UNIT_UTILITIES_H