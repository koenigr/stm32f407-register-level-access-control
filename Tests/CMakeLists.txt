include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

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

add_executable(driver_tests
	KeypadDriverTest.cpp

	Stubs/hw_fakes.c

	../Drivers/keypad.c
)

target_include_directories(driver_tests
	PRIVATE
		../Drivers
		../Platform
)

target_compile_definitions(driver_tests PRIVATE UNIT_TEST)
target_link_libraries(driver_tests PRIVATE GTest::gtest_main)

gtest_discover_tests(driver_tests)