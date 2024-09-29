#pragma once

namespace Graphics{
	class ElementBuffer {
	public:
		ElementBuffer(unsigned int size);
		ElementBuffer(unsigned int const* indices, unsigned int count);
		~ElementBuffer();

		static std::shared_ptr<ElementBuffer> Create(unsigned int size);
		static std::shared_ptr<ElementBuffer> Create(unsigned int const* indices, unsigned int count);

		void SetData(void* const data, unsigned int size);

		void Bind() const;
		void Unbind() const;
		unsigned int GetCount() const;
	private:
		unsigned int mEboHdl;
		unsigned int mIdxCount;
	};
}

