class IKeypad {

public:
	virtual ~IKeypad() = default;
	virtual char Scan() = 0;
	
};