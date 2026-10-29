#pragma once

#include "IKeypad.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor) -- final, never deleted via base pointer (no heap)
class KeypadAdapter final : public IKeypad {

public:

	KeypadAdapter();

	char Scan() override;

};