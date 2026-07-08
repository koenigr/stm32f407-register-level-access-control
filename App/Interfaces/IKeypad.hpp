class IKeypad {

public:
	virtual char Scan() = 0;

protected:
	~IKeypad() = default;

};