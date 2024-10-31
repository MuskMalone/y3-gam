#pragma once
namespace IGE {
	namespace Physics {
		namespace EventKey {
			enum class EventConstraintBreak {
				CONSTRAINTS,
				COUNT
			};
			enum class EventWake {
				ACTORS,
				COUNT
			};
			enum class EventSleep {
				ACTORS,
				COUNT
			};
			enum class EventContact {
				ENTITY_PAIR,
				CONTACT_POINTS
			};
			enum class EventTrigger {
				TRIGGER_ENTITY,
				OTHER_ENTITY
			};
			enum class EventAdvance {

			};
		}
	}
}