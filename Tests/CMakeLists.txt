include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(GoogleTest)

include(GoogleTest)

add_executable(unit_tests
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

target_link_libraries(unit_tests PRIVATE GTest::gtest_main)

gtest_discover_tests(unit_tests)
