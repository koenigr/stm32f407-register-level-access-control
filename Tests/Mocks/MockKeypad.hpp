#pragma once

#include "Interfaces/IKeypad.hpp"

class MockKeypad : public IKeypad {
public:
	char nextKey = '\0';

	char Scan() override {
		char key = nextKey;
		nextKey = '\0';

		return key;
	}
};