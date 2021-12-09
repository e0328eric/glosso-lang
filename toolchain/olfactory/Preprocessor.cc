#include <cstring>
#include <iostream>
#include <string>

#include "FileIO.hh"
#include "Preprocessor.hh"

#define PADDING_STRING_I "__GLOSSO_OLFACTORY_IWUHNDXAXS_"
#define PADDING_STRING_II "_XSDASDLWBKAS_GLOSSO_"

using namespace glosso::olfactory;
using Err = glosso::olfactory::OlfactoryErr;

constexpr char FIRST_INCLUDE_MACRO_FOUND = 1 << 0;
constexpr char NON_MACRO_CHAR_FOUND = 1 << 1;
constexpr char APPEND_MAIN_LOCATION_FINISHED = 1 << 2;

static constexpr uint64_t fnv1Hash(const char* str)
{
    size_t len = strlen(str);
    uint64_t output = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < len; ++i)
    {
        output *= 0x100000001b3ULL;
        output ^= (uint64_t)str[i];
    }

    return output;
}

static bool isLetter(const char& chr)
{
    return ('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z') ||
           chr == '_';
}

static std::string makeHashString(const char* filename)
{
    std::string output;

    output.append(PADDING_STRING_I);
    output.append(std::to_string(fnv1Hash(filename)));
    output.append(PADDING_STRING_II);

    return output;
}

Preprocessor::Preprocessor(const char* mainFilePath, const char* source)
    : mMainPath(), mSource(source), mSaveLocation(source), mStart(source),
      mCurrent(source), mIsPreprocessed(false), mIdentPairs()
{
    memset(static_cast<void*>(mDefinedFstLetter), 0, sizeof(mDefinedFstLetter));

    // Initialize mMainPath
    mMainPath = std::filesystem::absolute(mainFilePath);
    mMainPath = mMainPath.remove_filename();
}

Preprocessor::Preprocessor(const std::filesystem::path& mainFilePath,
                           const char* source)
    : mMainPath(), mSource(source), mSaveLocation(source), mStart(source),
      mCurrent(source), mIsPreprocessed(false), mIdentPairs()
{
    memset(static_cast<void*>(mDefinedFstLetter), 0, sizeof(mDefinedFstLetter));

    // Initialize mMainPath
    mMainPath = std::filesystem::absolute(mainFilePath);
    mMainPath = mMainPath.remove_filename();
}

Preprocessor::~Preprocessor()
{
    delete[] mSource;
}

// %include "stdio.h"
// %define THE_FOO 3
//
// includeHandle is a bitmask with three bit used
// 3 2 1 (nth bit from right to left)
// 0 0 0
// (1st) 1 if the first include macro was found
// (2nd) 1 if the first non-macro character was found
// (3rd) 1 if appending the main locaion label is finished
Err Preprocessor::preprocess(char** output)
{
    Err err = Err::Ok;
    std::string result;
    char includeHandle = NON_MACRO_CHAR_FOUND;

    result.reserve(strlen(mSource));

    while (!mIsPreprocessed)
    {
        switch (*mCurrent)
        {
        case '%':
            ++mCurrent;
            if (strncmp(mCurrent, "include", 7) == 0)
            {
                if ((includeHandle & (FIRST_INCLUDE_MACRO_FOUND |
                                      APPEND_MAIN_LOCATION_FINISHED)) == 0)
                {
                    result.append("jmp ");
                    result.append(makeHashString(mMainPath.c_str()));
                    result.append("\n");
                }
                includeHandle |= FIRST_INCLUDE_MACRO_FOUND;
                if ((err = parseIncludes(result)) != Err::Ok)
                {
                    *output = nullptr;
                    return err;
                }
            }
            else if (strncmp(mCurrent, "define", 6) == 0)
            {
                if ((err = parseDefine()) != Err::Ok)
                {
                    *output = nullptr;
                    return err;
                }
            }
            break;

        case ';':
            while (*mCurrent != '\n' && *mCurrent != '\0')
            {
                ++mCurrent;
            }
            break;

        case '\0':
            mIsPreprocessed = true;
            break;

        default:
            if (mDefinedFstLetter[static_cast<size_t>(*mCurrent)])
            {
                if ((err = pluginDefine(result, includeHandle)) != Err::Ok)
                {
                    *output = nullptr;
                    return err;
                }
            }
            else
            {
                if (includeHandle ==
                    (FIRST_INCLUDE_MACRO_FOUND | NON_MACRO_CHAR_FOUND))
                {
                    result.append("\n");
                    result.append(makeHashString(mMainPath.c_str()));
                    result.append(":\n");
                    includeHandle |= APPEND_MAIN_LOCATION_FINISHED;
                }
                includeHandle &= ~NON_MACRO_CHAR_FOUND;
                result.push_back(*mCurrent);
                ++mCurrent;
            }
            break;
        }
    }

    *output = new char[result.size() + 1];
    strlcpy(*output, result.c_str(), result.size() + 1);
    return Err::Ok;
}

Err Preprocessor::parseIncludes(std::string& string)
{
    char* innerFileSource = nullptr;
    char* filename_c = nullptr;
    Err err = Err::Ok;

    while (*mCurrent != '"' && *mCurrent != '\n' && *mCurrent != '\0')
    {
        ++mCurrent;
    }
    if (*mCurrent != '"')
    {
        return Err::IllFormedInclude;
    }

    mStart = ++mCurrent;

    while (*mCurrent != '"' && *mCurrent != '\n' && *mCurrent != '\0')
    {
        ++mCurrent;
    }
    if (*mCurrent != '"')
    {
        return Err::IllFormedInclude;
    }

    // Change current directory into the one of the input file
    // is located
    std::filesystem::current_path(mMainPath);

    filename_c = new char[mCurrent - mStart + 1];
    strlcpy(filename_c, mStart, mCurrent - mStart + 1);

    auto filename = std::filesystem::absolute(filename_c);

    // Read a file. Note that the related path is the position
    // at which the compiler run in default.
    if ((err = readFile(&innerFileSource, filename.c_str())) != Err::Ok)
    {
        return err;
    }

    // Repreprocess everything inside the innerFileSource
    char* preprocessed = nullptr;
    Preprocessor prep{filename, innerFileSource};
    if ((err = prep.preprocess(&preprocessed)) != Err::Ok)
    {
        return err;
    }

    string.append(preprocessed);

    while (*mCurrent != '\n' && *mCurrent != '\0')
    {
        ++mCurrent;
    }
    if (*mCurrent == '\0')
    {
        return Err::IllFormedInclude;
    }

    delete[] preprocessed;
    delete[] filename_c;

    return err;
}

// Syntax of %define grammar
//
// %define <indentifier> <expression>
// TODO(#14): At present, the <expression> is just a bunch words and
// the preprocessor does just copy and paste the words
// That means that it is not smart enough to act like C's one
Err Preprocessor::parseDefine()
{
    // At this point, *current is pointing into 'd', a part of "define" words.
    // move this pointer into the whitespace
    mCurrent += 6;

    // Find the next character until newline or EOF found
    while ((*mCurrent == ' ' || *mCurrent == '\t') && *mCurrent != '\n' &&
           *mCurrent != '\0')
    {
        ++mCurrent;
    }
    if (!isLetter(*mCurrent))
    {
        return Err::IllFormedDefine;
    }

    mStart = mCurrent;
    while (isLetter(*mCurrent))
    {
        ++mCurrent;
    }

    std::string_view identifier{mStart, static_cast<size_t>(mCurrent - mStart)};
    mDefinedFstLetter[static_cast<size_t>(*mStart)] = true;
    mStart = mCurrent;

    while (*mCurrent != '\n' && *mCurrent != '\0')
    {
        ++mCurrent;
    }
    if (*mCurrent == '\0')
    {
        return Err::IllFormedDefine;
    }

    while ((*mStart == ' ' || *mStart == '\t') && mStart < mCurrent)
    {
        ++mStart;
    }

    std::string_view value{mStart, static_cast<size_t>(mCurrent - mStart)};
    mIdentPairs.emplace_back(identifier, value);

    // pass the newline
    ++mCurrent;

    return Err::Ok;
}

Err Preprocessor::pluginDefine(std::string& string, char& includeHandle)
{
    size_t i;
    IdentPair* pair;
    size_t identPairsLen = mIdentPairs.size();

    for (i = 0; i < identPairsLen; ++i)
    {
        pair = &mIdentPairs[i];
        if (strncmp(mCurrent, pair->identifier.data(),
                    pair->identifier.size()) == 0)
        {
            break;
        }
    }

    if (i >= identPairsLen)
    {
        includeHandle &= !NON_MACRO_CHAR_FOUND;
        string.push_back(*mCurrent);
        ++mCurrent;
        return Err::Ok;
    }

    mCurrent += pair->identifier.size();
    string.append(pair->value.data(), pair->value.size());

    return Err::Ok;
}
