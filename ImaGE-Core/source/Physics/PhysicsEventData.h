#pragma once
namespace IGE {
	namespace Physics {
		enum EventConstraintBreak {
			CONSTRAINTS,
			COUNT
		};
		enum EventWake {
			ACTORS,
			COUNT
		};
		enum EventSleep {
			ACTORS,
			COUNT
		};
		enum EventContact{
			ENTITY_PAIR,
			CONTACT_POINTS,
			SEPERATION

		};
		enum EventTrigger {

		};
		enum EventAdvance {

		};
	}
}