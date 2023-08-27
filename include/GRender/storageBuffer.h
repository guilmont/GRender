#pragma once

#include "core.h"

namespace GRender {

class  StorageBuffer {
public:
    StorageBuffer(size_t numBytes, const void* data = nullptr);
    StorageBuffer(void) = default;
    ~StorageBuffer(void);

    // We should not copy GPU data
    StorageBuffer(const StorageBuffer&) = delete;
    StorageBuffer& operator=(const StorageBuffer&) = delete;
    // But we can move IDs around
    StorageBuffer(StorageBuffer&&) noexcept;
    StorageBuffer& operator=(StorageBuffer&&) noexcept;
    
    uint32_t id(void) const { return m_BufferID; }
    size_t numBytes(void) const { return m_NumBytes; }

    void bind(uint32_t location = 0) const;
    void update(const void* data, size_t offset = 0, size_t numBytes = 0);

    template <typename TP>
    std::vector<TP> getBuffer(size_t offset = 0ul, size_t numElements = 0ul) const {
        // Default size return all the available data 
        if (numElements == 0ul) {
            numElements = m_NumBytes / sizeof(TP);
        }

        std::vector<TP> vec(numElements);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, numElements * sizeof(TP), vec.data());
        return vec;
    }

    operator bool() const { return m_BufferID > 0; }
private:
    uint32_t m_BufferID = 0;
    size_t m_NumBytes = 0;
};

} // namespace GRender