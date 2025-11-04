#pragma once
#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @brief A Utility class for creating sequential ID's.
 */
class IDGenerator
{
public:
    explicit IDGenerator(const std::string& prefix) : m_prefix(prefix) { }

    u32 getNext()
    {
        if (m_deletedIds.empty()) { return m_idCount++; }
        const u32 id = m_deletedIds.top();
        m_deletedIds.pop();
        return id;
    }

    std::string getNextStr()
    {
        const std::string id = std::to_string(getNext());
        return m_prefix + id;
    }

    void onDelete(const u32 id)
    {
        if (id > m_idCount) {
            return;
        }
        m_deletedIds.push(id);
    }

    void onDelete(const std::string& id)
    {
        if (id.starts_with(m_prefix)) {
            const std::string_view substr{ id.data() + m_prefix.length(), id.size() - m_prefix.length() };
            if (isNumber(substr)) {
                const i32 num = std::atoi(substr.data());
                onDelete(num);
            }
        }
    }

private:
    std::stack<u32> m_deletedIds{ };
    u32 m_idCount = 0;

    std::string m_prefix;

    bool isNumber(const std::string_view s) const
    {
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }
};

}
