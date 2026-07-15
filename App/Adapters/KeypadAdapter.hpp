#pragma once

#include "IKeypad.hpp"

class KeypadAdapter : public IKeypad {

public:
	char Scan() override;

};