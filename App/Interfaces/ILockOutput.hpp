class ILockOutput {

public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;

protected:
	virtual ~ILockOutput() = default;

};