#pragma once

class ILedOutput {
public:
	virtual void Green() = 0;
	virtual void Red() = 0;
	virtual void BlinkRed() = 0;

protected:
	virtual ~ILedOutput() = default;
}