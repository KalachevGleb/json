#pragma once
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <stdexcept>
#include <concepts>
#include <format>

//! \file json.h
//! \brief Contains the class representing a JSON object.
class JSON {
    std::variant<std::nullptr_t, int64_t, double, bool, std::string, std::vector<JSON>, std::map<JSON, JSON>> data;
public:
    enum Type {
        String,
        Integer,
        Double,
        Boolean,
        Null,
        Array,
        Object
    };

    JSON(Type t) {
        switch (t) {
        case String:
            data = "";
            break;
        case Integer:
            data = int64_t(0);
            break;
        case Double:
            data = double(0);
            break;
        case Boolean:
            data = bool(false);
            break;
        case Null:
            data = nullptr;
            break;
        case Array:
            data = std::vector<JSON>();
            break;
        case Object:
            data = std::map<JSON, JSON>();
            break;
        }
    }

    //! \brief Constructs a JSON object from a string.
    //! \param str The string to construct the JSON object from.
    JSON(const std::string& str) {
        data = str;
    }
    //! \brief Constructs a JSON object from a string.
    //! \param str The string to construct the JSON object from.
    JSON(const char* str) {
        data = std::string(str);
    }
    //! \brief Constructs a JSON object from a string.
    //! \param str The string to construct the JSON object from.
    JSON(std::string&& str) {
        data = std::move(str);
    }
    //! \brief Constructs a JSON object from an integer.
    //! \param i The integer to construct the JSON object from.
    template<std::integral T>
    JSON(T i) {
        if constexpr(std::same_as<T, bool>)
            data = bool(i);
        else
            data = int64_t(i);
    }
    //! \brief Constructs a JSON object from a double.
    //! \param d The double to construct the JSON object from.
    template<std::floating_point T>
    JSON(T d) {
        data = double(d);
    }

    //! \brief Constructs a null JSON object.
    JSON() {
        data = nullptr;
    }
    ~JSON() = default;

    JSON(const JSON&) = default;
    JSON(JSON&&) = default;
    JSON& operator=(const JSON&) = default;
    JSON& operator=(JSON&&) = default;

    //! \brief Constructs a JSON object from a vector.
    //! \param v The vector to construct the JSON object from.
    JSON(const std::vector<JSON>& v) {
        data = v;
    }
    //! \brief Constructs a JSON object from a vector.
    //! \param v The vector to construct the JSON object from.
    JSON(std::vector<JSON>&& v) {
        data = std::move(v);
    }

    //! \brief Constructs a JSON object from a tuple (stored as an array).
    //! \param t The tuple to construct the JSON object from.
    template<class ... Args>
    JSON(const std::tuple<Args...>& t) {
        data = std::apply([](auto&& ... args) {
            return std::vector<JSON>{JSON(args)...};
        }, t);
    }

    //! \brief Constructs a JSON object from a pair (stored as an array).
    //! \param p The pair to construct the JSON object from.
    template<class Arg1, class Arg2>
    JSON(const std::pair<Arg1, Arg2>& p) {
        data = std::vector<JSON>{JSON(p.first), JSON(p.second)};
    }

    //! \brief Constructs a JSON object from a vector.
    //! \param v The vector to construct the JSON object from.
    template<typename T> requires std::convertible_to<T, JSON>
    JSON(const std::vector<T>& v) {
        data.emplace<std::vector<JSON>>(v.begin(), v.end());
    }
    //! \brief Constructs a JSON object from a map
    //! \param v The map to construct the JSON object from.
    JSON(const std::map<JSON, JSON>& v) {
        data = v;
    }
    //! \brief Constructs a JSON object from a map rvalue reference
    //! \param v The rvalue reference to the map to construct the JSON object from.
    JSON(std::map<JSON, JSON>&& v) {
        data = std::move(v);
    }
    //! \brief Constructs a JSON object from a map
    //! \param v The map to construct the JSON object from.
    template<typename K, typename V, typename cmp> requires std::convertible_to<K, JSON> && std::convertible_to<V, JSON>
    JSON(const std::map<K, V, cmp>& v){
        data.emplace<std::map<JSON, JSON>>(v.begin(), v.end());
    }

    //////// getters /////////

    //! \brief Returns the type of the JSON object.
    //! \return The type of the JSON object.
    Type type() const {
        return std::visit([](auto&& arg) -> Type {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return Type::String;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return Type::Integer;
            } else if constexpr (std::is_same_v<T, double>) {
                return Type::Double;
            } else if constexpr (std::is_same_v<T, bool>) {
                return Type::Boolean;
            } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return Type::Null;
            } else if constexpr (std::is_same_v<T, std::vector<JSON>>) {
                return Type::Array;
            } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
                return Type::Object;
            } else {
                static_assert(std::is_same_v<T, void>, "non-exhaustive visitor!");
            }
        }, data);
    }

    //! \brief Returns the type of the JSON object as a string.
    //! \return The type of the JSON object as a string.
    const char* typeStr() const {
        switch (type()) {
            case Type::String:
                return "string";
            case Type::Integer:
                return "integer";
            case Type::Double:
                return "double";
            case Type::Boolean:
                return "boolean";
            case Type::Null:
                return "null";
            case Type::Array:
                return "array";
            case Type::Object:
                return "object";
        }
    }

    /**
     * @brief Returns a reference to the underlying string value of the JSON object.
     *
     * If the JSON object is of type String, this function returns a reference to the string value.
     * If the JSON object is not of type String, the std::bad_variant_access exception will be thrown.
     *
     * @return A reference to the underlying string value.
     */
    std::string& str() {
        return std::get<std::string>(data);
    }

    /**
     * @brief Get the string value of the JSON object.
     *
     * This function returns the string value of the JSON object if its type is String.
     * If the JSON object's type is not String, an exception will be thrown.
     *
     * @return The string value of the JSON object.
     */
    const std::string& str() const {
        return std::get<std::string>(data);
    }

    /**
     * @brief Gets a reference to the integer value stored in the JSON object.
     *
     * This function returns a reference to the integer value stored in the JSON object.
     * If the JSON object is not of type JSON::Integer, the std::bad_variant_access exception will be thrown.
     *
     * @return A reference to the integer value.
     */
    int64_t& i() {
        return std::get<int64_t>(data);
    }

    /**
     * @brief Returns the integer value stored in the JSON object.
     *
     * This function returns the integer value stored in the JSON object.
     * If the JSON object is not of type JSON::Integer,
     * the std::bad_variant_access exception will be thrown.
     *
     * @return The integer value.
     */
    int64_t i() const {
        return std::get<int64_t>(data);
    }

    /**
     * @brief Returns a reference to the floating-point value stored in the JSON object.
     *
     * This function returns a reference to the double value stored in the JSON object.
     * If the JSON object is not of type JSON::Double, the std::bad_variant_access exception will be thrown.
     *
     * @return double& A reference to the floating-point number stored in the JSON object.
     */
    double& d() {
        return std::get<double>(data);
    }

    /**
     * @brief Returns a value of the floating-point value stored in the JSON object.
     *
     * This function returns the value of the floating-point number stored in the JSON object.
     * If the JSON object is not of type JSON::Double, the std::bad_variant_access exception will be thrown.
     *
     * @return double The value of the floating-point number stored in the JSON object.
     */
    double d() const {
        return std::get<double>(data);
    }

    /**
     * @brief Returns a reference to the Boolean value stored in the JSON object.
     *
     * This function retrieves a reference to the Boolean value stored in the JSON object.
     * It assumes that the JSON object is of type JSON::Boolean. If the JSON object
     * has other type, it will throw a std::bad_variant_access exception.
     *
     * @return A reference to the boolean value stored in the JSON object.
     */
    bool& b() {
        return std::get<bool>(data);
    }
    /**
     * @brief Returns the value of the JSON object as a boolean.
     *
     * This function retrieves the value of the JSON object as a boolean.
     * It assumes that the JSON object is of type JSON::Boolean. If the JSON object
     * has other type, it will throw a std::bad_variant_access exception.
     *
     * @return bool - The value of the JSON object as a boolean.
     */
    bool b() const {
        return std::get<bool>(data);
    }

    /** \brief Returns true if the JSON object holds a null value.
     *
     * \return bool - True if the JSON object holds a null value.
     */
    bool isNull() const {
        return std::holds_alternative<std::nullptr_t>(data);
    }

    /** \brief Returns a reference to the array stored in the JSON object.
     *
     * This function returns a reference to the underlying array of the JSON object.
     * It can only be called when the JSON object has a type of JSON::Array. If the JSON object
     * is not of type JSON::Array, the std::bad_variant_access exception will be thrown.
     *
     * \return std::vector<JSON>& - The array stored in the JSON object.
     */
    std::vector<JSON>& arr() {
        return std::get<std::vector<JSON>>(data);
    }
    /**
     * @brief Get the array stored in the JSON object.
     *
     * @return const std::vector<JSON>& The array stored in the JSON object.
     */
    const std::vector<JSON>& arr() const {
        return std::get<std::vector<JSON>>(data);
    }

    /** \brief Returns a reference to the map stored in the JSON object.
     *
     * This function returns a reference to the underlying map of the JSON object.
     * It can only be called when the JSON object has a type of JSON::Object. If the JSON object
     * is not of type JSON::Object, the std::bad_variant_access exception will be thrown.
     *
     * \return std::map<JSON, JSON>& - The map stored in the JSON object.
     */
    std::map<JSON, JSON>& obj() {
        return std::get<std::map<JSON, JSON>>(data);
    }
    /**
     * @brief Returns a constant reference to the map stored in the JSON object.
     *
     * This function returns a constant reference to the underlying map of the JSON object.
     * It can only be called when the JSON object has a type of JSON::Object. If the JSON object
     * is not of type JSON::Object, the std::bad_variant_access exception will be thrown.
     *
     * @return const std::map<JSON, JSON>& The map stored in the JSON object.
     */
    const std::map<JSON, JSON>& obj() const {
        return std::get<std::map<JSON, JSON>>(data);
    }

    // comparison operator
    std::strong_ordering operator <=>(const JSON& other) const;
    bool operator == (const JSON& other) const = default;
    template<typename T>
    bool operator == (const T& other) const {
        if constexpr(std::is_convertible_v<T, JSON>) {
            return *this == JSON(other);
        } else {
            return false;
        }
    }

    //// set type (without argument and conversion) ////

    //! \brief Sets the type of the JSON object to a string.
    std::string& setS() {
        data.emplace<std::string>();
        return std::get<std::string>(data);
    }
    //! \brief Sets the type of the JSON object to an integer.
    int64_t& setI() {
        data.emplace<int64_t>();
        return std::get<int64_t>(data);
    }
    //! \brief Sets the type of the JSON object to a double.
    double& setD() {
        data.emplace<double>();
        return std::get<double>(data);
    }
    //! \brief Sets the type of the JSON object to a boolean.
    bool& setB() {
        data.emplace<bool>();
        return std::get<bool>(data);
    }
    //! \brief Sets the type of the JSON object to null.
    void setNull() {
        data.emplace<std::nullptr_t>();
    }
    //! \brief Sets the type of the JSON object to an array.
    std::vector<JSON>& setArr() {
        data.emplace<std::vector<JSON>>();
        return std::get<std::vector<JSON>>(data);
    }
    //! \brief Sets the type of the JSON object to an object.
    std::map<JSON, JSON>& setObj() {
        data.emplace<std::map<JSON, JSON>>();
        return std::get<std::map<JSON, JSON>>(data);
    }

    /**
     * @brief Converts the object to a string representation with optional line wrapping.
     *
     * @param[out] out The output string where the converted representation will be stored.
     * @param[in] wrapLines Set to true to wrap lines after a certain number of characters.
     *
     * @remarks The function converts the object to a string representation, which will be stored in the
     *          `out` parameter. If `wrapLines` is set to true, the resulting string will have line breaks
     *          inserted at a certain number of characters to ensure proper line wrapping. By default,
     *          line wrapping is disabled.
     *
     * @note The `out` parameter must be an empty string before calling this function. The function will
     *       append the converted representation to the existing content of `out`.
     */
    ///////// converters /////////
    void toString(std::string &out, bool wrapLines = false) const;
    /**
     * @brief Converts the object to a string representation.
     *
     * This method converts the object to a string representation. By default, the string
     * representation does not wrap lines. However, if `wrapLines` parameter is set to true, the
     * string representation will wrap lines.
     *
     * @param wrapLines Flag indicating whether to wrap lines in the string representation.
     *
     * @return The string representation of the object.
     */
    std::string toString(bool wrapLines = false) const {
        std::string out;
        toString(out, wrapLines);
        return out;
    }

    /**
       * @brief Tries to extract a value from the variant and assign it to the output parameter.
       *
       * This function is templated on an integral type `I`. It attempts to extract a value from the data variant
       * and assigns it to the output parameter `out`. If the variant holds an `int64_t` or a `bool` value,
       * the value is casted to type `I` and assigned to `out`.
       *
       * @param out Reference to the output parameter where the extracted value will be assigned.
       * @return `true` if a value is successfully extracted and assigned to `out`, `false` otherwise.
       */
    template<std::integral I>
    bool tryGet(I& out) const {
        return std::visit([&out](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, bool>) {
                out = static_cast<I>(arg);
                return true;
            } else {
                return false;
            }
        }, data);
    }
    /**
     * @brief Tries to extract a value from the variant and assigns it to the provided output variable.
     *
     * This function attempts to extract a value from a variant and assigns it to the provided output variable.
     * The function supports extracting values of type `double`, `int64_t`, and `bool` and will convert the extracted value to the type specified by the template parameter `F`.
     *
     * @tparam F The type of the output variable, which should be a floating-point type.
     *
     * @param out A reference to the output variable where the extracted value will be assigned.
     * @return True if a value was successfully extracted and assigned to the output variable, otherwise false.
     *
     * @note This function uses `std::decay_t` to remove any reference or cv-qualifiers from the extracted value type before type checking.
     */
    template<std::floating_point F>
    bool tryGet(F& out) const {
        return std::visit([&out](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int64_t> || std::is_same_v<T, bool>) {
                out = static_cast<F>(arg);
                return true;
            } else {
                return false;
            }
        }, data);
    }
    /**
     * @brief Tries to get the string representation of the object.
     *
     * This function converts the object to a string using the toString() function
     * and stores the result in the given out parameter. It returns true to indicate
     * that the conversion was successful.
     *
     * @param out The output string where the result will be stored.
     * @return true if the conversion was successful.
     */
    bool tryGet(std::string& out) const {
        toString(out);
        return true;
    }
    /**
     * @brief Tries to get the values from the array variant.
     *
     * This function attempts to retrieve the values from the array variant.
     * It checks if the current variant type is an array, and if so,
     * it resizes the output vector to match the size of the array.
     * Then, it iterates through each element of the array and tries to get the value
     * by calling the `tryGet()` function recursively on each element.
     * If at any point, the `tryGet()` function returns false, it means that the value
     * couldn't be retrieved, so the function returns false.
     * If all values are successfully retrieved, the function returns true.
     *
     * @tparam T The type of the elements in the output vector.
     * @param[out] out The output vector to store the values.
     * @return True if all values are successfully retrieved, false otherwise.
     */
    template<class T>
    bool tryGet(std::vector<T>& out) const {
        if (type() != Type::Array) {
            return false;
        }
        out.resize(arr().size());
        for (size_t i = 0; i < arr().size(); ++i) {
            if (!arr()[i].tryGet(out[i])) {
                return false;
            }
        }
        return true;
    }
    template<class T1, class T2>
    bool tryGet(std::pair<T1, T2>& out) const {
        if (type() != Type::Array || arr().size() != 2) {
            return false;
        }
        return arr()[0].tryGet(out.first) && arr()[1].tryGet(out.second);
    }
    template<class ... T>
    bool tryGet(std::tuple<T...>& out) const {
        if (type() != Type::Array || arr().size() != sizeof...(T)) {
            return false;
        }
        return _tryGetTuple(out, std::index_sequence_for<T...>());
    }
    template<class K, class V, class cmp>
    bool tryGet(std::map<K, V, cmp>& out) const {
        if (type() != Type::Object) {
            return false;
        }
        for (const auto& [k, v] : obj()) {
            K key;
            if (!k.tryGet(key)) {
                return false;
            }
            if (!v.tryGet(out[key])) {
                return false;
            }
        }
        return true;
    }
    template<class T>
    void get(T& out) const {
        if (!tryGet(out)) {
            throw std::runtime_error("JSON::get(): type mismatch");
        }
    }

    template<class T>
    T as() const {
        T out;
        get(out);
        return out;
    }
    bool is(Type t) const {
        return type() == t;
    }

    //// container operators ////
    template<class I> requires std::is_convertible_v<I, JSON>
    JSON& operator[](const I& index) {
        return std::visit([&index](auto&& arg) -> JSON& {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_integral_v<I>) {
                if constexpr(std::is_integral_v<I>) {
                    auto idx = index;  // Create a local copy
                    if (idx < 0) {
                        idx += arg.size();
                    }
                    if (idx < 0 || idx >= arg.size()) {
                        throw std::out_of_range(
                                "JSON index out of range (index = " + std::to_string(index) + ", size = " +
                                std::to_string(arg.size()) + ")");
                    }
                    return arg[idx];
                }
            } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
                return arg[index];
            } else {
                throw std::runtime_error("JSON::operator[]: type mismatch");
            }
        }, data);
    }
    template<class I> requires std::is_convertible_v<I, JSON>
    const JSON& at(const I& index) const {
        return std::visit([&index](auto&& arg) -> const JSON& {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<JSON>> && std::is_integral_v<I>) {
                if constexpr (std::is_integral_v<I>) {
                    auto idx = index;  // Create a local copy
                    if (idx < 0) {
                        idx += arg.size();
                    }
                    if (idx<0 || idx >= arg.size()) {
                        throw std::out_of_range("JSON::at(" + std::to_string(index) + "): index out of range"
                                                " (size = " + std::to_string(arg.size()) + ")");
                    }
                    return arg[idx];
                }
            } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
                auto it = arg.find(index);
                if (it == arg.end()) {
                    throw std::out_of_range("JSON::at(" + JSON(index).toString() + "): key not found");
                }
                return it->second;
            } else {
                throw std::runtime_error("JSON::at: type mismatch");
            }
        }, data);
    }
    size_t size() const {
        return std::visit([](auto&& arg) -> size_t {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<JSON>>) {
                return arg.size();
            } else if constexpr (std::is_same_v<T, std::map<JSON, JSON>>) {
                return arg.size();
            } else {
                throw std::runtime_error("JSON::size(): type mismatch");
            }
        }, data);
    }
    template<class T>
    void push_back(const T& val) {
        std::visit([&val](auto&& arg) {
            using I = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<I, std::vector<JSON>>) {
                arg.push_back(val);
            } else {
                throw std::runtime_error("JSON::push_back(): type mismatch");
            }
        }, data);
    }
    template<class K, class V> requires std::is_convertible_v<K, JSON> && std::is_convertible_v<V, JSON>
    void emplace(const K& key, const V& val) {
        std::visit([&key, &val](auto&& arg) {
            using I = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<I, std::map<JSON, JSON>>) {
                arg.emplace(key, val);
            } else {
                throw std::runtime_error("JSON::emplace(): type mismatch");
            }
        }, data);
    }
    template<class K> requires std::is_convertible_v<K, JSON>
    bool contains(const K& key) const {  // check if key exists in object or index exists in array
        return std::visit([&key](auto&& arg) -> bool {
            using I = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<I, std::map<JSON, JSON>>) {
                return arg.contains(key);
            } else if constexpr (std::is_same_v<I, std::vector<JSON>> && std::is_integral_v<K>) {
                if constexpr(std::is_integral_v<K>) {
                    return (int)key >= 0 && key < arg.size();
                }
            } else {
                throw std::runtime_error("JSON::contains(): type mismatch");
            }
        }, data);
    }
    //// arithmetic operators ////
    explicit operator bool() const;
    JSON operator-() const;
    JSON& operator+=(const JSON& rhs);
    JSON& operator-=(const JSON& rhs);
    JSON& operator*=(const JSON& rhs);
    JSON& operator/=(const JSON& rhs);
    JSON& operator%=(const JSON& rhs);
    JSON& operator&=(const JSON& rhs);
    JSON& operator|=(const JSON& rhs);
    JSON& operator^=(const JSON& rhs);
    JSON& operator<<=(const JSON& rhs);
    JSON& operator>>=(const JSON& rhs);
    JSON& operator++(){
        return *this += 1;
    }
    JSON& operator--() {
        return *this -= 1;
    }
    JSON operator+(const JSON& rhs) const{
        return JSON(*this) += rhs;
    }
    JSON operator-(const JSON& rhs) const{
        return JSON(*this) -= rhs;
    }
    JSON operator*(const JSON& rhs) const{
        return JSON(*this) *= rhs;
    }
    JSON operator/(const JSON& rhs) const{
        return JSON(*this) /= rhs;
    }
    JSON operator%(const JSON& rhs) const{
        return JSON(*this) %= rhs;
    }
    JSON operator&(const JSON& rhs) const{
        return JSON(*this) &= rhs;
    }
    JSON operator|(const JSON& rhs) const{
        return JSON(*this) |= rhs;
    }
    JSON operator^(const JSON& rhs) const{
        return JSON(*this) ^= rhs;
    }
    JSON operator<<(const JSON& rhs) const{
        return JSON(*this) <<= rhs;
    }
    JSON operator>>(const JSON& rhs) const{
        return JSON(*this) >>= rhs;
    }

    // input/output
    static JSON parse(const std::string& str, bool defaultToString = false);
    static JSON load(const std::string& filename);
    void write(std::ostream& out, bool wrapLines = false) const;
    void save(const std::string& filename, bool wrapLines = false) const;
    std::string toPrettyString(unsigned maxLineLength = 120, unsigned minWrapLen=20, unsigned indent = 4, unsigned level = 0) const;
    void savePretty(const std::string& filename, unsigned maxLineLength = 120, unsigned minWrapLen=20, unsigned indent = 4) const;
private:
    template<class ... T, size_t ... I>
    bool _tryGetTuple(std::tuple<T...>& out, std::index_sequence<I...>) const {
        return (arr()[I].tryGet(std::get<I>(out)) && ...);
    }
};

std::ostream &operator<<(std::ostream &out, const JSON &json);

// support for std::format
template <>
struct std::formatter<JSON> : std::formatter<std::string> {
    // Possible format specifiers are "", "w", or "Np" where N is the line width for pretty printing
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') {
            if (*it == 'w') {
                spec = 'w';
                ++it;
            } else if (*it == 'p') {
                spec = 'p';
                lineWidth = 120;
                ++it;
            } else if (std::isdigit(*it)) {
                spec = 'p';
                lineWidth = 0;
                while (it != end && std::isdigit(*it)) {
                    lineWidth = lineWidth * 10 + (*it - '0');
                    ++it;
                }
            } else {
                throw std::format_error("invalid format specifier `" + std::string(it, ctx.end()) + "`for JSON object");
            }
        }

        return it;
    }

    auto format(const JSON& json, auto& ctx) const {
         switch (spec) {
            case 'w':
                return std::formatter<std::string>::format(json.toString(true), ctx);
            case 'p':
                return std::formatter<std::string>::format(json.toPrettyString(lineWidth), ctx);
            default:
                return std::formatter<std::string>::format(json.toString(false), ctx);
        }
    }

private:
    char spec = 0; // Default spec is empty, meaning single-line
    int lineWidth = 120; // Default width for pretty printing
};
