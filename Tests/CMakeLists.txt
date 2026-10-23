add_executable(unit_tests
	main.cpp
	PinValidatorTest.cpp
	AttemptCounterTest.cpp
	AccessControllerTest.cpp
	KeypadAdapterTest.cpp
	Stubs/keypad_stub.c

	../App/PinValidator.cpp
	../App/sha256.c
	../App/AttemptCounter.cpp
	../App/AccessController.cpp
	../App/Adapters/KeypadAdapter.cpp
)

target_include_directories(unit_tests
	PRIVATE
		../App
		../App/Adapters
		../App/Interfaces
		../Drivers
)