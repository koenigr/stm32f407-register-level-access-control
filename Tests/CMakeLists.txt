add_executable(unit_tests
	main.cpp
	PinValidatorTest.cpp
	AttemptCounterTest.cpp
	AccessControllerTest.cpp

	../App/PinValidator.cpp
	../App/sha256.c
	../App/AttemptCounter.cpp
	../App/AccessController.cpp
)

target_include_directories(unit_tests
	PRIVATE
		../App
)