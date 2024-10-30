#pragma once
#include <any>
#include <unordered_map>
#include <type_traits>
#include <DebugTools/DebugLogger/DebugLogger.h>
#include <DebugTools/Exception/Exception.h>

namespace IGE {
	namespace Physics {
		enum class PhysicsEventID {
			CONSTRAINT_BREAK,
			WAKE,
			SLEEP,
			CONTACT,
			TRIGGER,
			ADVANCE
		};
		template <typename _event_enum>
		int IntCast(_event_enum e) { return static_cast<int>(e); }

		class PhysicsEvent
		{
		public:
			PhysicsEvent() = delete;

			explicit PhysicsEvent(PhysicsEventID type)
				: mType(type)
			{}
			/*  _________________________________________________________________________ */
		/*! SetParam

		@param id The ID of the parameter to be set.
		@param value The value to be set for the parameter.

		@return none.

		Sets a parameter with the specified ID and value for the event.
		*/
			template<typename _event_enum, typename T>
			void SetParam(_event_enum e, T value)
			{
				mData[IntCast(e)] = value;
			}
			/*  _________________________________________________________________________ */
		/*! GetParam

		@param id The ID of the parameter to be retrieved.

		@return T The value of the parameter with the specified ID.

		Retrieves the value of a parameter with the specified ID. If the parameter
		does not exist, returns a default-constructed value of type T and sets the
		failure flag.
		*/

			template<typename T, typename _event_enum>
			T const& GetParam(_event_enum enumid) const
			{
				int id{ IntCast(enumid) };
				static_assert(std::is_default_constructible<T>::value);
				if (mData.find(id) == mData.end()) {
					mGetFail = true;
					if (mGetFail) Debug::DebugLogger::GetInstance().LogError("no such data was sent in Physics Event");
				}
				mGetFail = false;
				return std::any_cast<T const&>(mData.at(id));
			}
			/*  _________________________________________________________________________ */
		/*! GetType

		@return EventId The type ID of the event.

		Provides the type ID of the event.
		*/
			PhysicsEventID GetType() const
			{
				return mType;
			}
			/*  _________________________________________________________________________ */
		/*! GetFail

		@return bool Whether the last GetParam call failed.

		Provides the status of the last GetParam call. Returns true if the call failed,
		otherwise false.
		*/
			bool GetFail() const {
				return mGetFail;
			}

		private:
			mutable bool mGetFail{false}; //failbit
			PhysicsEventID mType{};
			std::unordered_map<int, std::any> mData{};
		};

	}
}
