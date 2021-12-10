#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Value.hh"

using namespace glosso;

Value::Value() : mType(ValueType::Null), mAs({.uintVal = 0ULL})
{
}

Value::Value(int val) : mType(ValueType::Integer), mAs({.intVal = (int64_t)val})
{
}

Value::Value(unsigned int val)
    : mType(ValueType::UInteger), mAs({.uintVal = (uint64_t)val})
{
}

Value::Value(int64_t val) : mType(ValueType::Integer), mAs({.intVal = val})
{
}

Value::Value(uint64_t val) : mType(ValueType::UInteger), mAs({.uintVal = val})
{
}

Value::Value(double val) : mType(ValueType::Float), mAs({.floatVal = val})
{
}

Value::Value(char val) : mType(ValueType::Char), mAs({.charVal = val})
{
}

Value::Value(bool val) : mType(ValueType::Boolean), mAs({.boolVal = val})
{
}

Value::Value(size_t val)
    : mType(ValueType::GlobalPtr), mAs({.globalPtrVal = val})
{
}

Value::Value(void* val) : mType(ValueType::HeapPtr), mAs({.heapPtrVal = val})
{
}

/* Operator Overloading */
constexpr int match(ValueType v1, ValueType v2)
{
    return ((int)v1 << 4) | (int)v2;
}

std::partial_ordering Value::operator<=>(const Value& rhs) const
{
    switch (match(mType, rhs.mType))
    {
    case match(ValueType::Null, ValueType::Null):
        return std::partial_ordering::equivalent;
    case match(ValueType::Null, ValueType::Boolean):
        return false <=> rhs.mAs.boolVal;

    case match(ValueType::Integer, ValueType::Integer):
        return mAs.intVal <=> rhs.mAs.intVal;
    case match(ValueType::Integer, ValueType::UInteger):
        return mAs.intVal <=> ((int64_t)rhs.mAs.uintVal);
    case match(ValueType::Integer, ValueType::Float):
        return (double)mAs.intVal <=> rhs.mAs.floatVal;
    case match(ValueType::Integer, ValueType::Char):
        return mAs.intVal <=> ((int64_t)rhs.mAs.charVal);

    case match(ValueType::UInteger, ValueType::Integer):
        return (int64_t)mAs.uintVal <=> rhs.mAs.intVal;
    case match(ValueType::UInteger, ValueType::UInteger):
        return mAs.uintVal <=> rhs.mAs.uintVal;
    case match(ValueType::UInteger, ValueType::Float):
        return (double)mAs.uintVal <=> rhs.mAs.floatVal;
    case match(ValueType::UInteger, ValueType::Char):
        return mAs.uintVal <=> ((uint64_t)rhs.mAs.charVal);

    case match(ValueType::Float, ValueType::Integer):
        return mAs.floatVal <=> ((double)rhs.mAs.intVal);
    case match(ValueType::Float, ValueType::UInteger):
        return mAs.floatVal <=> ((double)rhs.mAs.uintVal);
    case match(ValueType::Float, ValueType::Float):
        return mAs.floatVal <=> rhs.mAs.floatVal;
    case match(ValueType::Float, ValueType::Char):
        return mAs.floatVal <=> ((double)rhs.mAs.charVal);

    case match(ValueType::Char, ValueType::Integer):
        return (int64_t)mAs.charVal <=> rhs.mAs.intVal;
    case match(ValueType::Char, ValueType::UInteger):
        return (uint64_t)mAs.charVal <=> rhs.mAs.uintVal;
    case match(ValueType::Char, ValueType::Float):
        return (double)mAs.charVal <=> rhs.mAs.floatVal;
    case match(ValueType::Char, ValueType::Char):
        return mAs.charVal <=> rhs.mAs.charVal;

    case match(ValueType::Boolean, ValueType::Null):
        return mAs.boolVal <=> false;
    case match(ValueType::Boolean, ValueType::Boolean):
        return mAs.boolVal <=> rhs.mAs.boolVal;

    case match(ValueType::GlobalPtr, ValueType::GlobalPtr):
        return mAs.globalPtrVal <=> rhs.mAs.globalPtrVal;

    case match(ValueType::HeapPtr, ValueType::HeapPtr):
        return mAs.heapPtrVal <=> rhs.mAs.heapPtrVal;

    default:
        return std::partial_ordering::unordered;
    }
}

bool Value::operator==(const Value& rhs) const
{
    return (*this <=> rhs) == 0;
}
bool Value::operator!=(const Value& rhs) const
{
    return (*this <=> rhs) != 0;
}

Value Value::operator+(const Value& rhs)
{
    switch (match(mType, rhs.mType))
    {
    case match(ValueType::Integer, ValueType::Integer):
        return Value{mAs.intVal + rhs.mAs.intVal};
    case match(ValueType::Integer, ValueType::UInteger):
        return Value{mAs.intVal + (int64_t)rhs.mAs.uintVal};
    case match(ValueType::Integer, ValueType::Float):
        return Value{(double)mAs.intVal + rhs.mAs.floatVal};
    case match(ValueType::Integer, ValueType::Char):
        return Value{mAs.intVal + (int64_t)rhs.mAs.charVal};
    case match(ValueType::Integer, ValueType::HeapPtr):
        return Value{(void*)((char*)rhs.mAs.heapPtrVal + mAs.intVal)};

    case match(ValueType::UInteger, ValueType::Integer):
        return Value{(int64_t)mAs.uintVal + rhs.mAs.intVal};
    case match(ValueType::UInteger, ValueType::UInteger):
        return Value{mAs.uintVal + rhs.mAs.uintVal};
    case match(ValueType::UInteger, ValueType::Float):
        return Value{(double)mAs.uintVal + rhs.mAs.floatVal};
    case match(ValueType::UInteger, ValueType::Char):
        return Value{mAs.uintVal + (uint64_t)rhs.mAs.charVal};
    case match(ValueType::UInteger, ValueType::HeapPtr):
        return Value{(void*)((char*)rhs.mAs.heapPtrVal + mAs.uintVal)};

    case match(ValueType::Float, ValueType::Integer):
        return Value{mAs.floatVal + (double)rhs.mAs.intVal};
    case match(ValueType::Float, ValueType::UInteger):
        return Value{mAs.floatVal + (double)rhs.mAs.uintVal};
    case match(ValueType::Float, ValueType::Float):
        return Value{mAs.floatVal + rhs.mAs.floatVal};

    case match(ValueType::Char, ValueType::Integer):
        return Value{(int64_t)mAs.charVal + rhs.mAs.intVal};
    case match(ValueType::Char, ValueType::UInteger):
        return Value{(uint64_t)mAs.charVal + rhs.mAs.uintVal};

    case match(ValueType::HeapPtr, ValueType::Integer):
        return Value{(void*)((char*)mAs.heapPtrVal + rhs.mAs.intVal)};
    case match(ValueType::HeapPtr, ValueType::UInteger):
        return Value{(void*)((char*)mAs.heapPtrVal + rhs.mAs.uintVal)};

    default:
        return Value{};
    }
}

Value Value::operator-(const Value& rhs)
{
    switch (match(mType, rhs.mType))
    {
    case match(ValueType::Integer, ValueType::Integer):
        return Value{mAs.intVal - rhs.mAs.intVal};
    case match(ValueType::Integer, ValueType::UInteger):
        return Value{mAs.intVal - (int64_t)rhs.mAs.uintVal};
    case match(ValueType::Integer, ValueType::Float):
        return Value{(double)mAs.intVal - rhs.mAs.floatVal};
    case match(ValueType::Integer, ValueType::Char):
        return Value{mAs.intVal - (int64_t)rhs.mAs.charVal};
    case match(ValueType::Integer, ValueType::HeapPtr):
        return Value{(void*)((char*)rhs.mAs.heapPtrVal - mAs.intVal)};

    case match(ValueType::UInteger, ValueType::Integer):
        return Value{(int64_t)mAs.uintVal - rhs.mAs.intVal};
    case match(ValueType::UInteger, ValueType::UInteger):
        return Value{mAs.uintVal - rhs.mAs.uintVal};
    case match(ValueType::UInteger, ValueType::Float):
        return Value{(double)mAs.uintVal - rhs.mAs.floatVal};
    case match(ValueType::UInteger, ValueType::Char):
        return Value{mAs.uintVal - (uint64_t)rhs.mAs.charVal};
    case match(ValueType::UInteger, ValueType::HeapPtr):
        return Value{(void*)((char*)rhs.mAs.heapPtrVal - mAs.uintVal)};

    case match(ValueType::Float, ValueType::Integer):
        return Value{mAs.floatVal - (double)rhs.mAs.intVal};
    case match(ValueType::Float, ValueType::UInteger):
        return Value{mAs.floatVal - (double)rhs.mAs.uintVal};
    case match(ValueType::Float, ValueType::Float):
        return Value{mAs.floatVal - rhs.mAs.floatVal};

    case match(ValueType::Char, ValueType::Integer):
        return Value{(int64_t)mAs.charVal - rhs.mAs.intVal};
    case match(ValueType::Char, ValueType::UInteger):
        return Value{(int64_t)mAs.charVal - (int64_t)rhs.mAs.uintVal};
    case match(ValueType::Char, ValueType::Char):
        return Value{(int64_t)mAs.charVal - (int64_t)rhs.mAs.charVal};

    case match(ValueType::HeapPtr, ValueType::Integer):
        return Value{(void*)((char*)mAs.heapPtrVal - rhs.mAs.intVal)};
    case match(ValueType::HeapPtr, ValueType::UInteger):
        return Value{(void*)((char*)mAs.heapPtrVal - rhs.mAs.uintVal)};
	case match(ValueType::HeapPtr, ValueType::HeapPtr):
		return Value{(int64_t)((char*)mAs.heapPtrVal - (char*)rhs.mAs.heapPtrVal)};

    default:
        return Value{};
    }
}

Value Value::operator*(const Value& rhs)
{
    switch (match(mType, rhs.mType))
    {
    case match(ValueType::Integer, ValueType::Integer):
        return Value{mAs.intVal * rhs.mAs.intVal};
    case match(ValueType::Integer, ValueType::UInteger):
        return Value{mAs.intVal * (int64_t)rhs.mAs.uintVal};
    case match(ValueType::Integer, ValueType::Float):
        return Value{(double)mAs.intVal * rhs.mAs.floatVal};

    case match(ValueType::UInteger, ValueType::Integer):
        return Value{(int64_t)mAs.uintVal * rhs.mAs.intVal};
    case match(ValueType::UInteger, ValueType::UInteger):
        return Value{mAs.uintVal * rhs.mAs.uintVal};
    case match(ValueType::UInteger, ValueType::Float):
        return Value{(double)mAs.uintVal * rhs.mAs.floatVal};

    case match(ValueType::Float, ValueType::Integer):
        return Value{mAs.floatVal * (double)rhs.mAs.intVal};
    case match(ValueType::Float, ValueType::UInteger):
        return Value{mAs.floatVal * (double)rhs.mAs.uintVal};
    case match(ValueType::Float, ValueType::Float):
        return Value{mAs.floatVal * rhs.mAs.floatVal};

    default:
        return Value{};
    }
}

Value Value::operator/(const Value& rhs)
{
    switch (match(mType, rhs.mType))
    {
    case match(ValueType::Integer, ValueType::Integer):
        return Value{mAs.intVal / rhs.mAs.intVal};
    case match(ValueType::Integer, ValueType::UInteger):
        return Value{mAs.intVal / (int64_t)rhs.mAs.uintVal};
    case match(ValueType::Integer, ValueType::Float):
        return Value{(double)mAs.intVal / rhs.mAs.floatVal};

    case match(ValueType::UInteger, ValueType::Integer):
        return Value{(int64_t)mAs.uintVal / rhs.mAs.intVal};
    case match(ValueType::UInteger, ValueType::UInteger):
        return Value{mAs.uintVal / rhs.mAs.uintVal};
    case match(ValueType::UInteger, ValueType::Float):
        return Value{(double)mAs.uintVal / rhs.mAs.floatVal};

    case match(ValueType::Float, ValueType::Integer):
        return Value{mAs.floatVal / (double)rhs.mAs.intVal};
    case match(ValueType::Float, ValueType::UInteger):
        return Value{mAs.floatVal / (double)rhs.mAs.uintVal};
    case match(ValueType::Float, ValueType::Float):
        return Value{mAs.floatVal / rhs.mAs.floatVal};

    default:
        return Value{};
    }
}

Value Value::operator-() const
{
    switch (mType)
    {
    case ValueType::Integer:
        return Value{-mAs.intVal};
    case ValueType::UInteger:
        return Value{-mAs.uintVal};
    case ValueType::Float:
        return Value{-mAs.floatVal};
    default:
        return Value{};
    }
}

Value Value::operator!() const
{
    if (mType != ValueType::Boolean)
        return Value{};

    return Value{!mAs.boolVal};
}

Value& Value::operator++()
{
    switch (mType)
    {
    case ValueType::Integer:
        ++mAs.intVal;
        break;
    case ValueType::UInteger:
        ++mAs.uintVal;
        break;
    case ValueType::Float:
        ++mAs.floatVal;
        break;
    case ValueType::Char:
        ++mAs.charVal;
        break;
    case ValueType::HeapPtr:
        mAs.heapPtrVal = (void*)((char*)mAs.heapPtrVal + 1);
        break;
    default:
        break;
    }
    return *this;
}

Value& Value::operator--()
{
    switch (mType)
    {
    case ValueType::Integer:
        --mAs.intVal;
        break;
    case ValueType::UInteger:
        --mAs.uintVal;
        break;
    case ValueType::Float:
        --mAs.floatVal;
        break;
    case ValueType::Char:
        --mAs.charVal;
        break;
    case ValueType::HeapPtr:
        mAs.heapPtrVal = (void*)((char*)mAs.heapPtrVal - 1);
        break;
    default:
        break;
    }
    return *this;
}

/* Utilities */
bool Value::isType(ValueType valType) const
{
    return mType == valType;
}

bool Value::getIntVal(int64_t& intVal) const
{
    if (mType != ValueType::Integer)
        return false;

    intVal = mAs.intVal;
    return true;
}

bool Value::getUIntVal(uint64_t& uintVal) const
{
    if (mType != ValueType::UInteger)
        return false;

    uintVal = mAs.uintVal;
    return true;
}

bool Value::getFloatVal(double& floatVal) const
{
    if (mType != ValueType::Float)
        return false;

    floatVal = mAs.floatVal;
    return true;
}

bool Value::getCharVal(char& charVal) const
{
    if (mType != ValueType::Char)
        return false;

    charVal = mAs.charVal;
    return true;
}

bool Value::getBoolVal(bool& boolVal) const
{
    if (mType != ValueType::Boolean)
        return false;

    boolVal = mAs.boolVal;
    return true;
}

bool Value::getGlobalPtrVal(size_t& globalPtrVal) const
{
    if (mType != ValueType::GlobalPtr)
        return false;

    globalPtrVal = mAs.globalPtrVal;
    return true;
}

void* Value::getHeapPtrVal() const
{
    if (mType != ValueType::HeapPtr)
        return nullptr;

    return mAs.heapPtrVal;
}

bool Value::isFalseVal() const
{
    switch (mType)
    {
    case ValueType::Null:
        return true;
    case ValueType::Boolean:
        return !mAs.boolVal;
    default:
        return false;
    }
}

void Value::writeValue(FILE* fs) const
{
    fwrite((void*)&mType, 1, 1, fs);

    switch (mType)
    {
    case ValueType::Null:
        fprintf(fs, "%c", '\x00');
        break;

    case ValueType::Boolean:
        fprintf(fs, "%c", mAs.boolVal ? '\x01' : '\x00');
        break;

    case ValueType::Char:
        fprintf(fs, "%c", mAs.charVal);
        break;

    default:
        fwrite((void*)&mAs, 1, sizeof(mAs), fs);
        break;
    }
}

const char* Value::readValue(const char* str)
{
    mType = (ValueType)str[0];

    switch (mType)
    {
    case ValueType::Null:
        if (str[1] != '\0')
            return nullptr;
        mAs.uintVal = 0;
        return str + 2;

    case ValueType::Boolean:
        if (str[1] > '\x01')
            return nullptr;
        mAs.boolVal = str[1] == '\0' ? false : true;
        return str + 2;

    case ValueType::Char:
        mAs.charVal = str[1];
        return str + 2;

    case ValueType::Integer:
    case ValueType::UInteger:
    case ValueType::Float:
    case ValueType::GlobalPtr:
    case ValueType::HeapPtr:
        memcpy((void*)&mAs, str + 1, sizeof(mAs));
        return str + 1 + sizeof(mAs);

    // Invalid value types
    default:
        return nullptr;
    }
}

void Value::printValue(const char* globalData) const
{
    switch (mType)
    {
    case ValueType::Null:
        std::cout << "null";
        break;
    case ValueType::Integer:
        std::cout << mAs.intVal;
        break;
    case ValueType::UInteger:
        std::cout << mAs.uintVal;
        break;
    case ValueType::Float:
        std::cout << mAs.floatVal;
        break;
    case ValueType::Char:
        std::cout << mAs.charVal;
        break;
    case ValueType::Boolean:
        std::cout << (mAs.boolVal ? "true" : "false");
        break;
    case ValueType::GlobalPtr:
        std::cout << &globalData[mAs.globalPtrVal];
        break;
    // TODO(#16): For now on, it is assumed to print raw string literals
    // but it can be an array of bytes
    // later, if the string type is necessary, implement it
    case ValueType::HeapPtr:
        std::cout << (char*)mAs.heapPtrVal;
        break;
    }
}

std::ostream& glosso::operator<<(std::ostream& os, const Value& value)
{
    switch (value.mType)
    {
    case ValueType::Null:
        os << "Null";
        break;
    case ValueType::Integer:
        os << "Integer(" << value.mAs.intVal << ")";
        break;
    case ValueType::UInteger:
        os << "UInteger(" << value.mAs.uintVal << ")";
        break;
    case ValueType::Float:
        os << "Float(" << value.mAs.floatVal << ")";
        break;
    case ValueType::Char:
        if (value.mAs.charVal >= '!')
            os << "Char('" << value.mAs.charVal << "')";
        else
            os << "Char(" << (int)value.mAs.charVal << ")";
        break;
    case ValueType::Boolean:
        os << "Bool(" << (value.mAs.boolVal ? "true" : "false") << ")";
        break;
    case ValueType::GlobalPtr:
        os << "GlobalPtr(" << value.mAs.globalPtrVal << ")";
        break;
    case ValueType::HeapPtr:
        os << "HeapPtr(" << value.mAs.heapPtrVal << ")";
        break;
    }

    return os;
}
