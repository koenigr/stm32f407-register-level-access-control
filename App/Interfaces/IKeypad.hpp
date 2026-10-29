#pragma once

class IKeypad {

public:
	IKeypad(const IKeypad&) = delete;
	IKeypad& operator=(const IKeypad&) = delete;
	IKeypad(IKeypad&&) = delete;
	IKeypad& operator = (IKeypad&&) = delete;

	virtual char Scan() = 0;

protected:
	IKeypad() = default;
	~IKeypad() = default;

};