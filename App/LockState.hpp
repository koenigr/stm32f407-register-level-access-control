#include <cstdint>

enum class LockState : std::uint8_t {
	Locked,
	Unlocked,
	LockedOut
};