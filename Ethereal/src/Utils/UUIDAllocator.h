#pragma once

#include <unordered_map>

namespace Ethereal
{
    static const size_t k_invalid_UUID = 0;

    template <typename T>
    class UUIDAllocator {
      public:
        static bool isValidUUID(size_t UUID) { return UUID != k_invalid_UUID; }

        size_t allocUUID(const T& t) {
            auto find_it = m_elements_UUID_map.find(t);
            if (find_it != m_elements_UUID_map.end()) {
                return find_it->second;
            }

            for (size_t i = 0; i < m_UUID_elements_map.size() + 1; i++) {
                size_t UUID = i + 1;
                if (m_UUID_elements_map.find(UUID) == m_UUID_elements_map.end()) {
                    m_UUID_elements_map.insert(std::make_pair(UUID, t));
                    m_elements_UUID_map.insert(std::make_pair(t, UUID));
                    return UUID;
                }
            }

            return k_invalid_UUID;
        }

        bool getUUIDRelatedElement(size_t UUID, T& t) {
            auto find_it = m_UUID_elements_map.find(UUID);
            if (find_it != m_UUID_elements_map.end()) {
                t = find_it->second;
                return true;
            }
            return false;
        }

        bool getElementUUID(const T& t, size_t& UUID) {
            auto find_it = m_elements_UUID_map.find(t);
            if (find_it != m_elements_UUID_map.end()) {
                UUID = find_it->second;
                return true;
            }
            return false;
        }

        bool hasElement(const T& t) { return m_elements_UUID_map.find(t) != m_elements_UUID_map.end(); }

        void freeUUID(size_t UUID) {
            auto find_it = m_UUID_elements_map.find(UUID);
            if (find_it != m_UUID_elements_map.end()) {
                const auto& ele = find_it->second;
                m_elements_UUID_map.erase(ele);
                m_UUID_elements_map.erase(UUID);
            }
        }

        void freeElement(const T& t) {
            auto find_it = m_elements_UUID_map.find(t);
            if (find_it != m_elements_UUID_map.end()) {
                const auto& UUID = find_it->second;
                m_elements_UUID_map.erase(t);
                m_UUID_elements_map.erase(UUID);
            }
        }

        std::vector<size_t> getAllocatedUUIDs() const {
            std::vector<size_t> allocated_UUIDs;
            for (const auto& ele : m_UUID_elements_map) {
                allocated_UUIDs.push_back(ele.first);
            }
            return allocated_UUIDs;
        }

        void clear() {
            m_elements_UUID_map.clear();
            m_UUID_elements_map.clear();
        }

      private:
        std::unordered_map<T, size_t> m_elements_UUID_map;
        std::unordered_map<size_t, T> m_UUID_elements_map;
    };
}  // namespace Ethereal