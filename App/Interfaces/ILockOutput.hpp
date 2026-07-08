class ILockOutput {

public:
	virtual ~ILockOutput() = default;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;

};