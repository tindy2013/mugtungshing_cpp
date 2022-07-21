#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <string>
#include <vector>

inline std::string trimOf(const std::string& str, char target, bool before, bool after)
{
    if (!before && !after)
        return str;
    std::string::size_type pos = 0;
    if (before)
        pos = str.find_first_not_of(target);
    if (pos == std::string::npos)
    {
        return str;
    }
    std::string::size_type pos2 = str.size() - 1;
    if (after)
        pos2 = str.find_last_not_of(target);
    if (pos2 != std::string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

inline std::vector<std::string> split(const std::string &s, const std::string &seperator)
{
    std::vector<std::string> result;
    size_t i = 0;

    while(i != s.size())
    {
        int flag = 0;
        while(i != s.size() && flag == 0)
        {
            flag = 1;
            for(size_t x = 0; x < seperator.size(); ++x)
                if(s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        flag = 0;
        size_t j = i;
        while(j != s.size() && flag == 0)
        {
            for(size_t x = 0; x < seperator.size(); ++x)
                if(s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
            if(flag == 0)
                ++j;
        }
        if(i != j)
        {
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}

inline std::string dumpVector(const std::vector<std::string> &vec)
{
    std::string result;
    size_t idx = 0;
    for(auto &x : vec)
    {
        result += "[" + std::to_string(idx++) + "]: \"" + x + "\", ";
    }
    return result.erase(result.size() - 2);
}

inline std::string replaceAllDistinct(std::string str, const std::string &old_value, const std::string &new_value)
{
    for(std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
    {
        if((pos = str.find(old_value, pos)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

inline std::string escapeHTMLCharacters(const std::string &str)
{
    return replaceAllDistinct(replaceAllDistinct(replaceAllDistinct(str, "&", "&amp;"), "<", "&lt;"), ">", "&gt;");
}

#endif
