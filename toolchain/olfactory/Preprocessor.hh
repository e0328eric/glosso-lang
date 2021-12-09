#ifndef GLOSSO_LANG_TOOLCHAIN_OLFACTORY_PREPROCESSOR_HH_
#define GLOSSO_LANG_TOOLCHAIN_OLFACTORY_PREPROCESSOR_HH_

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <vector>

#include "Error.hh"

namespace glosso::olfactory
{
struct IdentPair
{
    std::string_view identifier;
    std::string_view value;
};

class Preprocessor
{
  public:
    Preprocessor(const char* mainFilePath, const char* source);
    Preprocessor(const std::filesystem::path& mainFilePath, const char* source);
    ~Preprocessor();

    Preprocessor(const Preprocessor&) = delete;
    Preprocessor(Preprocessor&&) = delete;
    Preprocessor& operator=(const Preprocessor&) = delete;
    Preprocessor& operator=(Preprocessor&&) = delete;

    OlfactoryErr preprocess(char** output);

  private:
    OlfactoryErr parseIncludes(std::string& string);
    OlfactoryErr parseDefine();
    OlfactoryErr pluginDefine(std::string& string, char& includeHandle);

  private:
    std::filesystem::path mMainPath;
    const char* mSource;
    const char* mSaveLocation;
    const char* mStart;
    const char* mCurrent;
    bool mIsPreprocessed;

    std::vector<IdentPair> mIdentPairs;
    bool mDefinedFstLetter[128];
};
} // namespace glosso::olfactory

#endif // GLOSSO_LANG_TOOLCHAIN_OLFACTORY_PREPROCESSOR_HH_
