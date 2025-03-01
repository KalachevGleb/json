#include "json.h"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <set>

using namespace std;

void JSON::toString(std::string &out, bool wrapLines) const {
    std::visit([&out, wrapLines](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            out += '"';
            for (char c : arg) {
                if (c == '"' || c == '\\') {
                    out += '\\';
                }
                out += c;
            }
            out += '"';
        } else if constexpr (std::is_same_v<T, int64_t>) {
            out += std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            out += std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            out += arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            out += "null";
        } else if constexpr (std::is_same_v<T, std::vector<JSON>>) {
            out += '[';
            if (wrapLines) {
                out += '\n';
            }
            for (size_t i = 0; i < arg.size(); ++i) {
                if (wrapLines) {
                    out += "  ";
                }
                arg[i].toString(out, wrapLines);
                if (i != arg.size() - 1) {
                    out += ',';
                }
                if (wrapLines) {
                    out += '\n';
                }
            }
            if (wrapLines) {
                out += "  ";
            }
            out += ']';
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
            out += '{';
            if (wrapLines) {
                out += '\n';
            }
            size_t i = 0;
            for (auto& [key, value] : arg) {
                if (wrapLines) {
                    out += "  ";
                }
                key.toString(out, wrapLines);
                out += ':';
                if (wrapLines) {
                    out += ' ';
                }
                value.toString(out, wrapLines);
                if (i != arg.size() - 1) {
                    out += ',';
                }
                if (wrapLines) {
                    out += '\n';
                }
                ++i;
            }
            if (wrapLines) {
                out += "  ";
            }
            out += '}';
        } else {
            static_assert(std::is_same_v<T, void>, "non-exhaustive visitor!");
        }
    }, data);
}

std::string JSON::toPrettyString(unsigned maxLineLength, unsigned minWrapLen, unsigned indent, unsigned level) const{
    const unsigned preferred_table_sizes[] = {1, 2, 3, 4, 5, 8, 10, 16, 20, 32, 40, 50, 64};
    auto getPreferredSize = [preferred_table_sizes](size_t mx, size_t total) {
        unsigned bestDivisible = 1, best = 0;
        for (unsigned i=1; i < std::size(preferred_table_sizes); i++) {
            if (preferred_table_sizes[i] > mx) {
                break;
            }
            best = preferred_table_sizes[i];
            if (total % preferred_table_sizes[i] == 0) {
                bestDivisible = preferred_table_sizes[i];
            }
        }
        return bestDivisible*2>best ? bestDivisible : best;
    };
    std::string str = std::visit([maxLineLength, minWrapLen, indent, level, &getPreferredSize](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            std::string out = "\"";
            for (char   c: arg) {
                if (c == '"' || c == '\\') {
                    out += '\\';
                }
                out += c;
            }
            return out += '"';
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return "null";
        } else if constexpr (std::is_same_v<T, std::vector<JSON>>) {
            std::string s = "[";
            if (arg.empty()) {
                return s + "]";
            }
            vector<string> elements;
            size_t max_len = 0, total_len = 0;
            bool wrap = false;
            bool all_num = true;
            for (size_t i = 0; i < arg.size(); ++i) {
                if (arg[i].type() != Type::Integer && arg[i].type() != Type::Double)
                    all_num = false;
                auto e = arg[i].toPrettyString(maxLineLength, minWrapLen, indent, level + 1);
                elements.push_back(e);
                total_len += e.size();
                max_len  = std::max(max_len, e.size());
                if (e.find('\n') != std::string::npos)
                    wrap = true;
            }
            total_len += elements.size() * 2; // for "[", ", ", "]"
            wrap = wrap || (maxLineLength && total_len > maxLineLength - indent * level);
            bool wrapEach = wrap || (max_len > minWrapLen);
            if (wrap && !wrapEach && all_num) { // align elements (make them have the same length)
                for (auto &e: elements) {
                    e = std::string(max_len - e.size(), ' ') + e;
                }
            }
            auto maxPerLine= std::max((maxLineLength - indent * level - 2) / (max_len + 2), size_t(1));
            auto preferred = wrapEach ? 1 : wrap ? getPreferredSize(maxPerLine, elements.size()) :
                                                    elements.size() + 1;
            unsigned tab = level * indent + (wrapEach ? indent : 1);
            for (size_t i = 0; i < elements.size(); i++) {
                if (i)
                    s += ", ";
                if ((i || wrapEach) && i % preferred == 0) {
                    (s += "\n") += std::string(tab, ' ');
                }
                s += elements[i];
            }
            if (wrapEach) {
                (s += "\n") += std::string(level * indent, ' ');
            }
            return s += "]";
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
            std::string s = "{";
            if (arg.empty()) {
                return s + "}";
            }
            vector<string> elements;
            size_t max_len = 0, total_len = 0;
            bool wrap = false, hasNonNum = false;
            for (auto& [key, value] : arg) {
                auto k = key.toPrettyString(maxLineLength, minWrapLen, indent, level + 1);
                auto v = value.toPrettyString(maxLineLength, minWrapLen, indent, level + 1);
                auto e = k + ": " + v;
                if (key.type() != Type::Integer && key.type() != Type::Double &&
                    key.type() != Type::Boolean && key.type() != Type::Null)
                    hasNonNum = true;
                elements.push_back(e);
                total_len += e.size();
                max_len  = std::max(max_len, e.size());
                if (e.find('\n') != std::string::npos)
                    wrap = true;
            }
            total_len += elements.size() * 2; // for "{", ", ", "}"
            wrap = wrap || (maxLineLength && total_len > maxLineLength - indent * level) || (max_len > minWrapLen);
            bool wrapEach = wrap && ((max_len > minWrapLen) || hasNonNum);
            if (wrap && !wrapEach) { // align elements (make them have the same length)
                for (auto &e: elements) {
                    e = std::string(max_len - e.size(), ' ') + e;
                }
            }
            auto maxPerLine= std::max((maxLineLength - indent * level - 2) / (max_len + 2), size_t(1));
            auto preferred = wrapEach ? 1 : wrap ? getPreferredSize(maxPerLine, elements.size()) :
                                                    elements.size() + 1;
            unsigned tab = level * indent + (wrapEach ? indent : 1);
            for (size_t i = 0; i < elements.size(); i++) {
                if (i)
                    s += ", ";
                if ((i || wrapEach) && i % preferred == 0) {
                    (s += "\n") += std::string(tab, ' ');
                }
                s += elements[i];
            }
            if (wrapEach) {
                (s += "\n") += std::string(level * indent, ' ');
            }
            return s += "}";
        } else {
            static_assert(std::is_same_v<T, void>, "non-exhaustive visitor!");
        }
    }, data);
    return str;
}

template<typename C1, typename C2>
std::strong_ordering compare3way(const C1& c1, const C2& c2) {
    auto sz1 = std::size(c1), sz2 = std::size(c2);
    if (sz1 != sz2) {
        return sz1 <=> sz2;
    }
    auto it1 = std::begin(c1), it2 = std::begin(c2);
    for (; it1 != std::end(c1); ++it1, ++it2) {
        auto c = *it1 <=> *it2;
        if (c != std::strong_ordering::equal) {
            return c;
        }
    }
    return std::strong_ordering::equal;
}

std::strong_ordering JSON::operator<=>(const JSON &other) const
{
    auto c1 = data.index() <=> other.data.index();
    if (c1 != std::strong_ordering::equal) {
        return c1;
    }
    // now we know that the types are the same
    return std::visit([&other](auto&& arg1) -> std::strong_ordering {
        using T = std::decay_t<decltype(arg1)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg1 <=> other.str();
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg1 <=> other.i();
        } else if constexpr (std::is_same_v<T, double>) {
            return arg1 < other.d() ? std::strong_ordering::less : (arg1 > other.d() ? std::strong_ordering::greater
                                                                                     : std::strong_ordering::equal);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg1 <=> other.b();
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return std::strong_ordering::equal;
        } else if constexpr (std::is_same_v<T, std::vector<JSON>>) {
            return compare3way(arg1, other.arr());
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
            return compare3way(arg1, other.obj());
        } else {
            static_assert(std::is_same_v<T, void>, "non-exhaustive visitor!");
        }
    }, data);
}

inline const char* ws(const char*& str) {
    if (!str)return nullptr;
    for (; *str; ++str) {
        if (!isspace(*str)) {
            if (*str != '/')break;
            if (str[1] == '/') {
                while (*str&&*str != '\n')
                    str++;
            } else if (str[1] == '*') {
                for (str+=2; *str; str++)
                    if (str[-1] == '*' && *str == '/')
                        break;
            } else return str;
        }
    }
    return str;
}

inline const char* wait(const char*& str, const char *w) {
    if (!str)return nullptr;
    ws(str);
    for (int i = 0; ; i++) {
        if (!w[i]) {
            return str += i;
        } else if (str[i] != w[i]) {
            return nullptr;
        }
    }
    return nullptr;
}

const char* parseElement(const char*& str, JSON &res);

const char* readList(const char*& str, std::vector<JSON> &res, char end) {
    if (!str)return nullptr;
    const char *ss = str;
    res.resize(1);
    while (*ws(str) != end && parseElement(str, res.back())) {
        res.emplace_back();
        if(!wait(str, ","))
            break;
    }
    res.pop_back();
    if (*str != end) {
        str = ss;
        return nullptr;
    }
    return ++str;
}

const char* readMembers(const char*& str, std::map<JSON,JSON> &res, char end) {
    if (!str)return nullptr;
    res.clear();
    const char *ss = str;
    JSON k, v;
    while (*ws(str) != end && parseElement(str, k) && wait(str,":") && parseElement(str, v)) {
        res[k] = v;
        if (!wait(str, ","))
            break;
    }
    if (*str != end) {
        str = ss;
        return nullptr;
    }
    return ++str;
}

const char *readString(const char *&str, std::string &res) {
    if (!str)return nullptr;
    res.clear();
    if (*ws(str) == '"') {
        while (*++str) {
            switch(*str) {
                case '\\':
                    switch (*++str) {
                        case 'n': res += '\n'; break;
                        case 'b': res += '\b'; break;
                        case 'f': res += '\f'; break;
                        case 'r': res += '\r'; break;
                        case 't': res += '\t'; break;
                        case 0: return str;
                        default: res += *str;
                    }
                    break;
                case '"': return ++str;
                case '\n':
                case 0: return str;
                default:
                    res += *str;
            }
        }
        return str;
    }
    if (!isalnum(*str))return nullptr;
    while (isalnum(*str))
        res += *str++;
    return str;
}

inline const char *readInt(const char*&str, int64_t &res) {
    if (!str)return nullptr;
    res = 0;
    const char *ss = str;
    bool sgn = *ss == '-';
    if (*ss == '-' || *ss == '+')
        ss++;

    for (; *ss&&isdigit(*ss); ss++) {
        if (res > (1ULL << 59))return nullptr;
        res = res * 10 + (*ss - '0');
    }
    if (sgn)res = -res;
    return str = ss;
}

inline const char *readIntF(const char*&str, int64_t &res) {
    const char *ss = str;
    ss = readInt(ss, res);
    if(ss && (*ss == '.' || *ss == 'e'))return nullptr;
    return str=ss;
}

inline const char *readFloat(const char*&str, double &res) {
    if (!str)return nullptr;
    res = 0;
    const char *ss = str;
    bool sgn = *ss == '-';
    if (*ss == '-' || *ss == '+')
        ss++;
    for (; isdigit(*ss); ss++) {
        res = res * 10 + (*ss - '0');
    }
    if (*ss == '.') {
        double f = 0.1;
        for (ss++; *ss && isdigit(*ss); ss++) {
            res += f*(*ss - '0');
            f *= 0.1;
        }
    }
    if (sgn)res = -res;
    if (tolower(*ss) == 'e') {
        int64_t r;
        if (!readInt(++ss, r))return str = ss;
        auto ar = std::abs(r);
        double x = r > 0 ? 10 : 0.1;
        for (; ar; ar >>= 1) { // multiply res by 10^r
            if (ar & 1)res *= x;
            x *= x;
        }
        //res*=pow(10., double(r));
    }
    return str = ss;
}

const char* parseElement(const char*& str, JSON &res) {
    if (!str)return nullptr;
    switch (*ws(str)) {
        case 't': return wait(str, "true");
        case 'f': return wait(str, "false");
        case 'n': return wait(str, "null");
        case '[':
            return readList(++str, res.setArr(), ']');
        case '{':
            return readMembers(++str, res.setObj(), '}');
        case '"':
            return readString(str, res.setS());
        default: break;
    }
    int64_t ii;
    double dd;
    if (isdigit(*str)||(*str=='-'&&isdigit(str[1]))) {
        if (readIntF(str, ii)) { res = ii; return str; }
        if (readFloat(str, dd)) { res = dd; return str; }
    }
    if (isalnum(*str)) {
        if (!readString(str, res.setS()))
            return nullptr;
    }
    return nullptr;
}

std::string position(const char *ptr, const char *pos) {
    int ln=1, col=1;
    for (; ptr < pos; ptr++) {
        if (*ptr == '\n') {
            ln++;
            col = 1;
        } else if (*ptr != '\r')col++;
    }
    return "(" + std::to_string(ln) + ":" + std::to_string(col) + ")";
}

JSON JSON::parse(const std::string& str, bool defaultToString) {
    const char *b = str.c_str(), *curr = b;
    JSON res;
    auto *e = parseElement(curr, res); e = ws(e);
    if (!e) {
        if (defaultToString) {
            return str;
        }
        throw std::runtime_error("JSON error at position " + position(b, curr));
    }
    if (*e) {
        if (defaultToString) {
            return str;
        }
        throw std::runtime_error("EOF expected at position " + position(b, e));
    }
    return res;
}

JSON JSON::load(const std::string& filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        throw runtime_error("Cannot not open input file `" + filename + "`");
    }
    std::string str;
    in.seekg(0, std::ios::end);
    str.reserve(in.tellg());
    in.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(in)),
               std::istreambuf_iterator<char>());
    return parse(str);
}

void JSON::write(std::ostream& out, bool wrapLines) const {
    string s;
    toString(s, wrapLines);
    out << s;
}

void JSON::save(const std::string& filename, bool wrapLines) const {
    ofstream out(filename);
    if (!out.is_open()) {
        throw runtime_error("Cannot not open file " + filename);
    }
    write(out, wrapLines);
}

void JSON::savePretty(const std::string& filename, unsigned maxLineLength, unsigned minWrapLen, unsigned indent) const {
    ofstream out(filename);
    if (!out.is_open()) {
        throw runtime_error("Cannot not open file " + filename);
    }
    out << toPrettyString(maxLineLength, minWrapLen, indent);
}

/////////////////
JSON::operator bool() const {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg != 0;
        } else if constexpr (std::is_same_v<T, double>) {
            return std::abs(arg) > 0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return !arg.empty();
        } else if constexpr (std::is_same_v<T, std::vector<JSON>>) {
            return !arg.empty();
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
            return !arg.empty();
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return false;
        } else {
            static_assert(std::is_same_v<T, void>, "non-exhaustive visitor!");
        }
    }, data);
}
JSON JSON::operator-() const {
    return std::visit([](auto&& arg) -> JSON {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            return -arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return -arg;
        } else {
            throw std::runtime_error("JSON::operator-: type mismatch");
        }
    }, data);
}
JSON& JSON::operator+=(const JSON& rhs) {
    std::visit([this](auto&& arg, auto&& rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
        if constexpr (std::is_arithmetic_v<T> && std::is_arithmetic_v<R>) {
            *this = arg + rhs;
        } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<R, std::string>) {
            arg += rhs;
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            arr().reserve(arr().size() + rhs.size());
            arr().insert(arr().end(), rhs.begin(), rhs.end());
        } else {
            throw std::runtime_error("JSON::operator+: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

JSON& JSON::operator-=(const JSON& rhs) { //arithmetic subtract or key set difference
    std::visit([this](auto&& arg, auto &&rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
        if constexpr (std::is_arithmetic_v<T> && std::is_arithmetic_v<R>) {
            *this = arg - rhs;
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            for (auto &key: rhs)
                arg.erase(key.first);
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            for (auto &key: rhs)
                arg.erase(key);
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            vector<JSON> s(rhs);
            std::sort(s.begin(), s.end());
            vector<JSON> new_arr;
            for (auto &key: arg)
                if(!std::binary_search(s.begin(), s.end(), key))
                    new_arr.push_back(key);
            arg = std::move(new_arr);
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            vector<JSON> new_arr;
            for (auto &key: arg)
                if(!rhs.contains(key))
                    new_arr.push_back(key);
            arg = std::move(new_arr);
        } else {
            throw std::runtime_error("JSON::operator-: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

JSON& JSON::operator*=(const JSON& rhs) {
    std::visit([&rhs, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            if (rhs.is(Type::Double))
                *this = arg * rhs.as<double>();
            else
                arg *= rhs.as<int64_t>();
        } else if constexpr (std::is_same_v<T, double>) {
            arg *= rhs.as<double>();
        } else {
            throw std::runtime_error("JSON::operator*: type mismatch");
        }
    }, data);
    return *this;
}
JSON& JSON::operator/=(const JSON& rhs) {
    std::visit([&rhs, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            if (rhs.is(Type::Double))
                *this = arg / rhs.as<double>();
            else {
                int64_t r = rhs.as<int64_t>();
                if (r == 0)
                    throw std::runtime_error("JSON::operator/: integer division by zero");
                arg /= rhs.as<int64_t>();
            }
        } else if constexpr (std::is_same_v<T, double>) {
            arg /= rhs.as<double>();
        } else {
            throw std::runtime_error("JSON::operator/: type mismatch");
        }
    }, data);
    return *this;
}
JSON& JSON::operator%=(const JSON& rhs) {
    std::visit([&rhs, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            if (rhs.is(Type::Double))
                *this = std::fmod(arg, rhs.as<double>());
            else {
                int64_t r = rhs.as<int64_t>();
                if (r == 0)
                    throw std::runtime_error("JSON::operator%: integer modulo by zero");
                arg %= rhs.as<int64_t>();
            }
        } else if constexpr (std::is_same_v<T, double>) {
            arg = std::fmod(arg, rhs.as<double>());
        } else {
            throw std::runtime_error("JSON::operator%: type mismatch");
        }
    }, data);
    return *this;
}

JSON& JSON::operator&=(const JSON& rhs) { //bitwise and or key set intersection
    std::visit([this](auto&& arg, auto &&rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
        if constexpr (std::is_integral_v<T> && std::is_integral_v<R> && std::is_same_v<T, R>) {
            arg &= rhs;
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            std::map<JSON, JSON> new_map, &curr_map = obj();
            for (auto& key : rhs) {
                auto it = curr_map.find(key);
                if (it != curr_map.end())
                    new_map.emplace(key, it->second);
            }
            arg = std::move(new_map);
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            std::vector<JSON> new_arr, &curr_arr = arr();
            std::sort(curr_arr.begin(), curr_arr.end());
            for (auto &key: rhs) {
                if (std::binary_search(curr_arr.begin(), curr_arr.end(), key))
                    new_arr.push_back(key);
            }
            arg = std::move(new_arr);
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            std::vector<JSON> new_arr, &curr_arr = arr();
            for (auto &key: curr_arr) {
                if (rhs.find(key) != rhs.end())
                    new_arr.push_back(key);
            }
            arg = std::move(new_arr);
        } else {
            throw std::runtime_error("JSON::operator&: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

JSON& JSON::operator|=(const JSON& rhs) { //bitwise OR or key set union
    std::visit([this](auto&& arg, auto &&rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
        if constexpr (std::is_integral_v<T> && std::is_integral_v<R> && std::is_same_v<T, R>) {
            arg |= rhs;
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            arg.insert(rhs.begin(), rhs.end());
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            set<JSON> s(arg.begin(), arg.end());
            for (auto &key: rhs)
                if(s.insert(key).second)
                    arg.push_back(key);
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            set<JSON> s(arg.begin(), arg.end());
            for (auto &key: rhs)
                if(s.insert(key.first).second)
                    arg.push_back(key.first);
        } else {
            throw std::runtime_error("JSON::operator|: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

JSON& JSON::operator^=(const JSON& rhs) { //bitwise XOR or key set symmetric difference
    std::visit([this](auto&& arg, auto &&rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
		if constexpr (std::is_integral_v<T> && std::is_integral_v<R> && std::is_same_v<T, R>) {
            arg ^= rhs;
        } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            for (auto &key: rhs)
                arg.erase(key.first);
            arg.insert(rhs.begin(), rhs.end());
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::vector<JSON>>) {
            vector<JSON> s1(arg), s2(arg);
            std::sort(s1.begin(), s1.end());
            std::sort(s2.begin(), s2.end());
            vector<JSON> new_arr;
            for (auto &key: arg)
                if(!std::binary_search(s2.begin(), s2.end(), key))
                    new_arr.push_back(key);
            for (auto &key: rhs)
                if(!std::binary_search(s1.begin(), s1.end(), key))
                    new_arr.push_back(key);
            arg = std::move(new_arr);
        } else if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_same_v<R, std::map<JSON, JSON>>) {
            vector<JSON> s(arg);
            std::sort(s.begin(), s.end());
            vector<JSON> new_arr;
            for (auto &key: arg)
                if(!rhs.contains(key))
                    new_arr.push_back(key);
            for (auto &key: rhs)
                if(!std::binary_search(s.begin(), s.end(), key.first))
                    new_arr.push_back(key.first);
        } else {
            throw std::runtime_error("JSON::operator^: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

JSON& JSON::operator<<=(const JSON& rhs) { //left shift or append
    std::visit([this](auto&& arg, auto &&rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
        if constexpr (std::is_same_v<T, int64_t> && std::is_same_v<R, int64_t>) {
            arg <<= rhs;
        } else {
            throw std::runtime_error("JSON::operator<<: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

JSON& JSON::operator>>=(const JSON& rhs) { //right shift or pop
    std::visit([this](auto&& arg, auto &&rhs) {
        using T = std::decay_t<decltype(arg)>;
        using R = std::decay_t<decltype(rhs)>;
        if constexpr (std::is_same_v<T, int64_t> && std::is_same_v<R, int64_t>) {
            arg >>= rhs;
        } else {
            throw std::runtime_error("JSON::operator>>: type mismatch");
        }
    }, data, rhs.data);
    return *this;
}

std::ostream &operator<<(std::ostream &out, const JSON &json) {
    json.write(out, false);
    return out;
}
