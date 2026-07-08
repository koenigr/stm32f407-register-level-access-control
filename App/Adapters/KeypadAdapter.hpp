#pragma once

#include "../Interfaces/IKeypad.hpp"

class KeypadAdapter : public IKeypad {

public:
	char Scan() override;

};