#pragma once

class ILedOutput {
public:
	ILedOutput(const ILedOutput&) = delete;
	ILedOutput& operator=(const ILedOutput&) = delete;
	ILedOutput(ILedOutput&&) = delete;
	ILedOutput& operator=(ILedOutput&&) = delete;

	virtual void Green() = 0;
	virtual void Red() = 0;
	virtual void BlinkRed() = 0;
	virtual void Off() = 0;

protected:
	ILedOutput() = default;
	virtual ~ILedOutput() = default;
};