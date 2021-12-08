#include <cstring>
#include <iostream>
#include <string>

#include "FileIO.hh"
#include "Preprocessor.hh"

#define HASH_APPEND "__AWSDEFQRC__XSDASDLWBKAS_"

using namespace glosso::olfactory;
using Err = glosso::olfactory::OlfactoryErr;

constexpr char FIRST_INCLUDE_MACRO_FOUND = 1 << 0;
constexpr char NON_MACRO_CHAR_FOUND = 1 << 1;
constexpr char APPEND_MAIN_LOCATION_FINISHED = 1 << 2;

static bool definedFstLetter[128];

static bool isLetter(const char& chr)
{
    return ('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z') ||
           chr == '_';
}

Preprocessor::Preprocessor(const char* mainFilePath, const char* source)
    : mMainPath(), mSource(source), mSaveLocation(source),
      mStart(source), mCurrent(source), mIsPreprocessed(false), mIdentPairs()
{
    memset(static_cast<void*>(definedFstLetter), 0, sizeof(definedFstLetter));

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
    std::string result[2] = {"", ""};
    size_t idx = 0;
    char includeHandle = NON_MACRO_CHAR_FOUND;

    result[0].reserve(strlen(mSource));
    result[1].reserve(strlen(mSource));

    while (true)
    {
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
                        result[idx].append("jmp " HASH_APPEND "\n");
                    }
                    includeHandle |= FIRST_INCLUDE_MACRO_FOUND;
                    if ((err = parseIncludes(result[idx])) != Err::Ok)
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
                if (definedFstLetter[static_cast<size_t>(*mCurrent)])
                {
                    if ((err = pluginDefine(result[idx], includeHandle)) !=
                        Err::Ok)
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
                        result[idx].append("\n" HASH_APPEND ":\n");
                        includeHandle |= APPEND_MAIN_LOCATION_FINISHED;
                    }
                    includeHandle &= ~NON_MACRO_CHAR_FOUND;
                    result[idx].push_back(*mCurrent);
                    ++mCurrent;
                }
                break;
            }
        }

        for (auto& chr : result[idx])
        {
            if (chr == '%')
            {
                goto STILL_PREPROCESS_NEEDED;
            }
        }
        break; // Exit the main largest loop

    STILL_PREPROCESS_NEEDED:
        result[!idx].clear();
        mIsPreprocessed = false;
        includeHandle = (includeHandle & APPEND_MAIN_LOCATION_FINISHED) != 0
                            ? APPEND_MAIN_LOCATION_FINISHED
                            : NON_MACRO_CHAR_FOUND;
        mStart = mCurrent = result[idx].c_str();
        // A trick to switch 0 and 1
        idx = !idx;
        continue;
    }

    *output = new char[result[idx].size() + 1];
    strlcpy(*output, result[idx].c_str(), result[idx].size() + 1);
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

    string.append(innerFileSource);

    while (*mCurrent != '\n' && *mCurrent != '\0')
    {
        ++mCurrent;
    }
    if (*mCurrent == '\0')
    {
        return Err::IllFormedInclude;
    }

    delete[] innerFileSource;
    delete[] filename_c;

    return err;
}

// Syntax of %define grammar
//
// %define <indentifier> <expression>
// TODO: At present, the <expression> is just a bunch words and
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
    definedFstLetter[static_cast<size_t>(*mStart)] = true;
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
