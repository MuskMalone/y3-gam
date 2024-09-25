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
				using OnLoad = std::function<void* (GUID)>;
				using OnDestroy = std::function<void(void*, GUID)>;

				IGE::Assets::TypeGUID typeGUID; // the type of asset
				OnLoad loadFunc;
				OnDestroy destroyFunc;
				std::string name;
			};
		}
		struct PartialRef {
			GUID guid; //64bit mersenne twister rand
			void* pointer; //64bit ptr

			bool IsPointer() const { return pointer; }
			operator bool() const { return IsPointer(); }
		};
		struct UniversalRef {
			PartialRef partialRef;
			TypeGUID typeGUID;
			operator bool() const { return static_cast<bool>(partialRef); }
			template <typename T>
			void DecRefCount() const { reinterpret_cast<T*>(partialRef.pointer)->DecRefCount(); }
			template <typename T>
			void IncRefCount() const { reinterpret_cast<T*>(partialRef.pointer)->IncRefCount(); }
			
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
			void AddToLiveReferences(GUID instance);
			void RemoveFromLiveReferences(GUID instance);
			bool IsLive(GUID instance);
		}

		template<typename T>
		class Ref
		{
		public:
			Ref(UniversalRef const& instance, Details::UniversalInfo const& uinfo, Details::InstanceInfo const& info)
				: mInstance{ instance }, mUInfo{ uinfo }, mInfo{ info }
			{
			}

			//Ref(std::nullptr_t n)
			//	: mInstance(nullptr)
			//{
			//}

			//Ref(T* instance)
			//	: mInstance(instance)
			//{
			//	static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");

			//	IncRef();
			//}

			template<typename T2>
			Ref(const Ref<T2>& other)
			{
				mInstance = other.mInstance;
				mUInfo = other.mUInfo;
				mInfo = other.mInfo;
				IncRef();
			}

			//template<typename T2>
			//Ref(Ref<T2>&& other)
			//{
			//	mInstance = other.mInstance;
			//	mUInfo = other.mUInfo;
			//	mInfo = other.mInfo;
			//	//other.mInstance = nullptr;
			//}

			//static Ref<T> CopyWithoutIncrement(const Ref<T>& other)
			//{
			//	Ref<T> result = nullptr;
			//	result->mInstance = other.mInstance;
			//	return result;
			//}

			~Ref()
			{
				DecRef();
			}

			Ref(const Ref<T>& other)
				: mInstance(other.mInstance), mUInfo{ other.mUInfo }, mInfo{ other.mInfo }
			{
				IncRef();
			}

			//Ref& operator=(std::nullptr_t)
			//{
			//	//DecRef();
			//	mInstance = nullptr;
			//	return *this;
			//}

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

			T* operator->() const { return reinterpret_cast<T*>(mInstance.partialRef.pointer); }
			//const T* operator->() const { return mInstance.partialRef.pointer; }

			T& operator*() const { return *reinterpret_cast<T*>(mInstance.partialRef.pointer); }
			//const T& operator*() const { return *mInstance.partialRef.pointer; }

			T* Raw() { return  mInstance; }
			const T* Raw() const { return  mInstance; }

			void Reset(T* instance = nullptr)
			{
				DecRef();
				mInstance.partialRef.pointer = reinterpret_cast<void*>(instance);
			}

			template<typename T2>
			Ref<T2> As() const
			{
				return Ref<T2>(*this);
			}

			//template<typename... Args>
			//static Ref<T> Create(Args&&... args)
			//{
			//	return Ref<T>(new T(std::forward<Args>(args)...));
			//}

			bool operator==(const Ref<T>& other) const
			{
				return mInstance == other.mInstance;
			}

			bool operator!=(const Ref<T>& other) const
			{
				return !(*this == other);
			}

			//bool EqualsObject(const Ref<T>& other)
			//{
			//	if (!mInstance || !other.mInstance)
			//		return false;

			//	return *mInstance == *other.mInstance;
			//}
			const UniversalRef& GetUniversalRef() const{
				return mInstance;
			}
		private:
			void Load() {
				mInstance.partialRef.pointer = mUInfo.loadFunc(mInstance.partialRef.guid);
				RefUtils::AddToLiveReferences(mInstance.partialRef.guid);
			}
			void Unload() {
				mUInfo.destroyFunc(mInstance.partialRef.pointer, mInstance.partialRef.guid);
				mInstance.partialRef.pointer = nullptr;
				RefUtils::RemoveFromLiveReferences(mInstance.partialRef.guid);
			}
			void IncRef() //const
			{
				if (mInstance)
				{
					mInstance.IncRefCount<T>();
					if (mInstance) RefUtils::AddToLiveReferences(mInstance.partialRef.guid);
				}
			}

			void DecRef() //const
			{
				if (mInstance)
				{
					mInstance.DecRefCount<T>();

					if (reinterpret_cast<T*>(mInstance.partialRef.pointer)->GetRefCount() == 0)
					{
						Unload();
						RefUtils::RemoveFromLiveReferences(mInstance.partialRef.guid);
					}
				}
			}

			template<class T2>
			friend class Ref;
			friend class AssetManager;
			//mutable T* mInstance;
			UniversalRef mInstance;
			Details::UniversalInfo mUInfo;
			Details::InstanceInfo mInfo;
		};

	}
}