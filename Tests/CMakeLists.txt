add_executable(unit_tests
	main.cpp
	PinValidatorTest.cpp
	AttemptCounterTest.cpp
	../App/PinValidator.cpp
	../App/AttemptCounter.cpp
)

target_include_directories(unit_tests
	PRIVATE
		../App
)