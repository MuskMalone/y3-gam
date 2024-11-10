#include "pch.h"
#include "ShaderStorageBuffer.h"
#include "Utils.h"

namespace Graphics {

    // Constructor: Allocate buffer with a specified size

    ShaderStorageBuffer::ShaderStorageBuffer(unsigned int size) : mSize(size) {
        glCreateBuffers(1, &mSsboHdl);
        glNamedBufferStorage(mSsboHdl, mSize, nullptr, GL_DYNAMIC_STORAGE_BIT); // Allow dynamic updates
    }

    // Constructor: Initialize buffer with data

    ShaderStorageBuffer::ShaderStorageBuffer(const void* data, unsigned int size) : mSize(size) {
        glCreateBuffers(1, &mSsboHdl);
        glNamedBufferStorage(mSsboHdl, mSize, data, GL_DYNAMIC_STORAGE_BIT); // Initialize with data
    }

    // Destructor: Delete the buffer

    inline ShaderStorageBuffer::~ShaderStorageBuffer() {
        glDeleteBuffers(1, &mSsboHdl);
    }

    // Factory methods for creating SSBOs

    std::shared_ptr<ShaderStorageBuffer> ShaderStorageBuffer::Create(unsigned int size) {
        return std::make_shared<ShaderStorageBuffer>(size);
    }

    std::shared_ptr<ShaderStorageBuffer> ShaderStorageBuffer::Create(const void* data, unsigned int size) {
        return std::make_shared<ShaderStorageBuffer>(data, size);
    }

    // Bind the SSBO to a specific binding point

    void ShaderStorageBuffer::Bind(unsigned int bindingPoint) const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, mSsboHdl);
    }

    // Unbind the SSBO from its binding point

     void ShaderStorageBuffer::Unbind() const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    }

    // Update data in the SSBO

    void ShaderStorageBuffer::SetData(const void* data, unsigned int size, unsigned int offset) {
        glNamedBufferSubData(mSsboHdl, offset, size, data);
    }

}