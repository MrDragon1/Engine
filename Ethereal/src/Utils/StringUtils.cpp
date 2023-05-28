#include "pch.h"
#include "StringUtils.h"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <regex>

namespace Ethereal::Utils {

namespace String {

bool EqualsIgnoreCase(const std::string_view a, const std::string_view b) {
    if (a.size() != b.size()) return false;

    return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](const char a, const char b) {
        return std::tolower(a) == std::tolower(b);
    });
}

std::string& ToLower(std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(),
                   [](const unsigned char c) { return std::tolower(c); });
    return string;
}

std::string ToLowerCopy(const std::string_view string) {
    std::string result(string);
    ToLower(result);
    return result;
}

void Erase(std::string& str, const char* chars) {
    for (size_t i = 0; i < strlen(chars); i++)
        str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
}

void Erase(std::string& str, const std::string& chars) { Erase(str, chars.c_str()); }

std::string SubStr(const std::string& string, size_t offset, size_t count) {
    if (offset == std::string::npos) return string;

    if (offset >= string.length()) return string;

    return string.substr(offset, count);
}
}  // namespace String

std::string_view GetFilename(const std::string_view filepath) {
    const std::vector<std::string> parts = SplitString(filepath, "/\\");

    if (parts.size() > 0) return parts[parts.size() - 1];

    return "";
}

std::string GetExtension(const std::string& filename) {
    std::vector<std::string> parts = SplitString(filename, '.');

    if (parts.size() > 1) return parts[parts.size() - 1];

    return "";
}

std::string RemoveExtension(const std::string& filename) {
    return filename.substr(0, filename.find_last_of('.'));
}

bool StartsWith(const std::string& string, const std::string& start) {
    return string.find(start) == 0;
}

std::vector<std::string> SplitStringAndKeepDelims(std::string str) {
    const static std::regex re(R"((^\W|^\w+)|(\w+)|[:()])", std::regex_constants::optimize);

    std::regex_iterator<std::string::iterator> rit(str.begin(), str.end(), re);
    std::regex_iterator<std::string::iterator> rend;
    std::vector<std::string> result;

    while (rit != rend) {
        result.emplace_back(rit->str());
        ++rit;
    }
    return result;
}

std::vector<std::string> SplitString(const std::string_view string,
                                     const std::string_view& delimiters) {
    size_t first = 0;

    std::vector<std::string> result;

    while (first <= string.size()) {
        const auto second = string.find_first_of(delimiters, first);

        if (first != second) result.emplace_back(string.substr(first, second - first));

        if (second == std::string_view::npos) break;

        first = second + 1;
    }

    return result;
}

std::vector<std::string> SplitString(const std::string_view string, const char delimiter) {
    return SplitString(string, std::string(1, delimiter));
}

std::string ToLower(const std::string_view& string) {
    std::string result;
    for (const auto& character : string) {
        result += std::tolower(character);
    }

    return result;
}

std::string ToUpper(const std::string_view& string) {
    std::string result;
    for (const auto& character : string) {
        result += std::toupper(character);
    }

    return result;
}

std::string BytesToString(uint64_t bytes) {
    constexpr uint64_t GB = 1024 * 1024 * 1024;
    constexpr uint64_t MB = 1024 * 1024;
    constexpr uint64_t KB = 1024;

    char buffer[32];

    if (bytes > GB)
        sprintf_s(buffer, "%.2f GB", (float)bytes / (float)GB);
    else if (bytes > MB)
        sprintf_s(buffer, "%.2f MB", (float)bytes / (float)MB);
    else if (bytes > KB)
        sprintf_s(buffer, "%.2f KB", (float)bytes / (float)KB);
    else
        sprintf_s(buffer, "%.2f bytes", (float)bytes);

    return std::string(buffer);
}

std::string DurationToString(std::chrono::duration<double> duration) {
    const auto durations =
        BreakDownDuration<std::chrono::minutes, std::chrono::seconds, std::chrono::milliseconds>(
            duration);

    std::stringstream durSs;
    durSs << std::setfill('0') << std::setw(1) << std::get<0>(durations).count() << ':'
          << std::setfill('0') << std::setw(2) << std::get<1>(durations).count() << '.'
          << std::setfill('0') << std::setw(3) << std::get<2>(durations).count();
    return durSs.str();
}

int SkipBOM(std::istream& in) {
    char test[4] = {0};
    in.seekg(0, std::ios::beg);
    in.read(test, 3);
    if (strcmp(test, "\xEF\xBB\xBF") == 0) {
        in.seekg(3, std::ios::beg);
        return 3;
    }
    in.seekg(0, std::ios::beg);
    return 0;
}

// Returns an empty string when failing.
std::string ReadFileAndSkipBOM(const std::filesystem::path& filepath) {
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in) {
        in.seekg(0, std::ios::end);
        uint32_t fileSize = in.tellg();
        const int skippedChars = SkipBOM(in);

        fileSize -= skippedChars - 1;
        result.resize(fileSize);
        in.read(result.data(), fileSize);
        // Add a dummy tab to beginning of file.
        // result[0] = '\t';
    } else {
        ET_CORE_WARN("Unable to load file {0}!", filepath.string());
    }
    in.close();
    result.pop_back();
    if (result.empty()) ET_CORE_WARN("Unable to load file {0}!", filepath.string());
    return result;
}

void ReplaceIdentifier(string& source, StringMap map) {
    for (auto [key, value] : map) {
        string surround_key = "{{" + key + "}}";

        for (string::size_type pos(0); pos != string::npos; pos += value.length()) {
            if ((pos = source.find(surround_key, pos)) != string::npos) {
                source.replace(pos, surround_key.length(), value);
            } else
                break;
        }
    }
}

}  // namespace Ethereal::Utils
