#ifndef GLOSSO_LANG_CORE_VALUE_HH_
#define GLOSSO_LANG_CORE_VALUE_HH_

#include <compare>
#include <cstdint>
#include <cstdio>

#include "SizeInt.hh"

namespace glosso
{
enum class ValueType : uint8_t
{
    Null = 0,
    Integer,
    UInteger,
    Float,
    Char,
    Boolean,
    GlobalPtr,
    HeapPtr,
};

class Value
{
  public:
    Value();
    explicit Value(int val);
    explicit Value(unsigned int val);
    explicit Value(int64_t val);
    explicit Value(uint64_t val);
    explicit Value(double val);
    explicit Value(char val);
    explicit Value(bool val);
    explicit Value(SizeInt val);
    explicit Value(void* val);

    std::partial_ordering operator<=>(const Value& rhs) const;
    bool operator==(const Value& rhs) const;
    bool operator!=(const Value& rhs) const;

    Value operator+(const Value& rhs);
    Value operator-(const Value& rhs);
    Value operator*(const Value& rhs);
    Value operator/(const Value& rhs);

	Value operator&(const Value& rhs);
	Value operator|(const Value& rhs);
	Value operator^(const Value& rhs);

    Value operator-() const;
    Value operator!() const;

    Value& operator++();
    Value& operator--();

    bool isType(ValueType valType) const;

    bool getIntVal(int64_t& intVal) const;
    bool getUIntVal(uint64_t& uintVal) const;
    bool getFloatVal(double& floatVal) const;
    bool getCharVal(char& charVal) const;
    bool getBoolVal(bool& boolVal) const;
    bool getGlobalPtrVal(size_t& globalPtrVal) const;
    void* getHeapPtrVal() const;

    bool isFalseVal() const;

    void writeValue(FILE* fs) const;
    const char* readValue(const char* str);

    void printValue(const char* globalData) const;
    friend std::ostream& operator<<(std::ostream& os, const Value& value);

  private:
    ValueType mType;
    union
    {
        int64_t intVal;
        uint64_t uintVal;
        double floatVal;
        char charVal;
        bool boolVal;
        SizeInt globalPtrVal;
        void* heapPtrVal;
    } mAs;
};

std::ostream& operator<<(std::ostream& os, const Value& value);
} // namespace glosso

#endif // GLOSSO_LANG_CORE_VALUE_HH_
