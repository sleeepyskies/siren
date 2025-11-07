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
        if (m_priorityQueue.empty()) { return m_idCount++; }
        const u32 id = m_priorityQueue.top();
        m_priorityQueue.pop();
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
        m_priorityQueue.push(id);
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
    std::priority_queue<u32, std::vector<u32>, std::greater<>> m_priorityQueue{ };
    u32 m_idCount = 0;

    std::string m_prefix;

    bool isNumber(const std::string_view s) const
    {
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }
};

}
