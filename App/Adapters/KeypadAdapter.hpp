#pragma once

#include "IKeypad.hpp"

class KeypadAdapter : public IKeypad {

public:

	KeypadAdapter();

	char Scan() override;

};