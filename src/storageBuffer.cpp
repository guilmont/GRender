#include "storageBuffer.h"

namespace GRender {

StorageBuffer::StorageBuffer(size_t numBytes, const void* data) : m_NumBytes(numBytes) {
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_NumBytes, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

StorageBuffer::~StorageBuffer(void) {
    glDeleteBuffers(1, &m_BufferID);
}

StorageBuffer::StorageBuffer(StorageBuffer&& buf) noexcept {
    std::swap(m_BufferID, buf.m_BufferID);
    std::swap(m_NumBytes, buf.m_NumBytes);
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& buf) noexcept {
    if (this != &buf) {
        this->~StorageBuffer();
        new(this) StorageBuffer(std::move(buf));
    }
    return *this;
}

void StorageBuffer::bind(uint32_t location) const {
    ASSERT(*this, "StorageBuffer not initialized!!");
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, m_BufferID);
}

void StorageBuffer::update(const void* data, size_t offset, size_t numBytes) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, numBytes == 0 ? m_NumBytes : numBytes, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

} // namespace GRender