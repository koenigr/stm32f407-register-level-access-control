#pragma once

class IKeypad {

public:
	virtual char Scan() = 0;

protected:
	virtual ~IKeypad() = default;

};