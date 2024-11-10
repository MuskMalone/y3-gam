#pragma once

namespace Graphics {

    class ShaderStorageBuffer {
    public:
        // Constructor: Allocate buffer with a specified size
        ShaderStorageBuffer(unsigned int size);

        // Constructor: Initialize buffer with data
        ShaderStorageBuffer(const void* data, unsigned int size);

        // Destructor: Delete the buffer
        ~ShaderStorageBuffer();

        // Factory methods for creating SSBOs
        static std::shared_ptr<ShaderStorageBuffer> Create(unsigned int size);

        static std::shared_ptr<ShaderStorageBuffer> Create(const void* data, unsigned int size);

        // Bind the SSBO to a specific binding point
        void Bind(unsigned int bindingPoint) const;

        // Unbind the SSBO from its binding point
        void Unbind() const;

        // Update data in the SSBO
        void SetData(const void* data, unsigned int size, unsigned int offset = 0);

    private:
        unsigned int mSsboHdl;  // Handle for the SSBO
        unsigned int mSize;     // Size of the buffer
    };

}
