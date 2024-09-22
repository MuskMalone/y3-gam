#pragma once
//#define IGE_MULTITHREAD
#include <pch.h>
#include <Core/GUID.h>


namespace IGE {
	namespace Assets {
		struct AssetGUIDTag {};
		struct TypeGUIDTag {};
		using GUID = IGE::Core::GUID<AssetGUIDTag>; // templating is just to differentiate the structs dont freak out
		using TypeGUID = IGE::Core::GUID<TypeGUIDTag>;

		namespace Details {
			struct InstanceInfo // info on request
			{
				GUID  guid;
				TypeGUID typeGUID;
				int refCount{ 1 };
				std::string filepath;
			};

			struct UniversalInfo
			{
				using OnLoad = std::function<void* ()>;
				using OnDestroy = std::function<void()>;

				IGE::Assets::TypeGUID typeGUID; // the type of asset
				OnLoad loadFunc;
				OnDestroy destroyFunc;
				std::string name;
			};
		}
		struct PartialRef {
			GUID guid; //64bit mersenne twister rand
			void* pointer; //64bit ptr

			bool IsPointer() const { return (guid & static_cast<uint64_t>(1)) == 0; }
		};
		struct UniversalRef {
			PartialRef partialRef;
			TypeGUID typeGUID;
		};
		class RefCounted
		{
		public:
			virtual ~RefCounted() = default;

			void IncRefCount() const
			{
				++mRefCount;
			}
			void DecRefCount() const
			{
				--mRefCount;
			}

			uint32_t GetRefCount() const { 
#ifdef IGE_MULTITHREAD
				return mRefCount.load();
#else
				return mRefCount;
#endif
			}
		private:
#ifdef IGE_MULTITHREAD
			mutable std::atomic<uint32_t> mRefCount = 0;
#else
			mutable uint32_t mRefCount = 0;
#endif
		};

		namespace RefUtils {
			void AddToLiveReferences(void* instance);
			void RemoveFromLiveReferences(void* instance);
			bool IsLive(void* instance);
		}

		template<typename T>
		class Ref
		{
		public:
			Ref()
				: mInstance(nullptr)
			{
			}

			Ref(std::nullptr_t n)
				: mInstance(nullptr)
			{
			}

			Ref(T* instance)
				: mInstance(instance)
			{
				static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");

				IncRef();
			}

			template<typename T2>
			Ref(const Ref<T2>& other)
			{
				mInstance = (T*)other.mInstance;
				IncRef();
			}

			template<typename T2>
			Ref(Ref<T2>&& other)
			{
				mInstance = (T*)other.mInstance;
				other.mInstance = nullptr;
			}

			static Ref<T> CopyWithoutIncrement(const Ref<T>& other)
			{
				Ref<T> result = nullptr;
				result->mInstance = other.mInstance;
				return result;
			}

			~Ref()
			{
				DecRef();
			}

			Ref(const Ref<T>& other)
				: mInstance(other.mInstance)
			{
				IncRef();
			}

			Ref& operator=(std::nullptr_t)
			{
				DecRef();
				mInstance = nullptr;
				return *this;
			}

			Ref& operator=(const Ref<T>& other)
			{
				if (this == &other)
					return *this;

				other.IncRef();
				DecRef();

				mInstance = other.mInstance;
				return *this;
			}

			template<typename T2>
			Ref& operator=(const Ref<T2>& other)
			{
				other.IncRef();
				DecRef();

				mInstance = other.mInstance;
				return *this;
			}

			template<typename T2>
			Ref& operator=(Ref<T2>&& other)
			{
				DecRef();

				mInstance = other.mInstance;
				other.mInstance = nullptr;
				return *this;
			}

			operator bool() { return mInstance != nullptr; }
			operator bool() const { return mInstance != nullptr; }

			T* operator->() { return mInstance; }
			const T* operator->() const { return mInstance; }

			T& operator*() { return *mInstance; }
			const T& operator*() const { return *mInstance; }

			T* Raw() { return  mInstance; }
			const T* Raw() const { return  mInstance; }

			void Reset(T* instance = nullptr)
			{
				DecRef();
				mInstance = instance;
			}

			template<typename T2>
			Ref<T2> As() const
			{
				return Ref<T2>(*this);
			}

			template<typename... Args>
			static Ref<T> Create(Args&&... args)
			{
				return Ref<T>(new T(std::forward<Args>(args)...));
			}

			bool operator==(const Ref<T>& other) const
			{
				return mInstance == other.mInstance;
			}

			bool operator!=(const Ref<T>& other) const
			{
				return !(*this == other);
			}

			bool EqualsObject(const Ref<T>& other)
			{
				if (!mInstance || !other.mInstance)
					return false;

				return *mInstance == *other.mInstance;
			}
			const UniversalRef& GetUniversalRef() const{
				return mInstance;
			}
		private:
			void IncRef() const
			{
				if (mInstance)
				{
					mInstance->IncRefCount();
					RefUtils::AddToLiveReferences((void*)mInstance);
				}
			}

			void DecRef() const
			{
				if (mInstance)
				{
					mInstance->DecRefCount();

					if (mInstance->GetRefCount() == 0)
					{
						delete mInstance;
						RefUtils::RemoveFromLiveReferences((void*)mInstance);
						mInstance = nullptr;
					}
				}
			}

			template<class T2>
			friend class Ref;
			friend class AssetManager;
			//mutable T* mInstance;
			mutable UniversalRef mInstance;
			Details::UniversalInfo mUInfo;
			Details::InstanceInfo mInfo;
		};

	}
}