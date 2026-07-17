#include <iostream>

void RunPinValidatorTests();

void RunAttemptCounterTests();

void RunAccessControllerTests();
void RunAccessControllerLockoutTest();
void RunAccessControllerShortPinTest();
void RunAccessControllerLongPinTest();
void RunAccessControllerClearPinTest();
void RunAccessControllerLockoutIgnoresInputTest();
void RunAccessControllerInvalidKeysTest();

int main()
{
	RunPinValidatorTests();
	
	RunAttemptCounterTests();

	RunAccessControllerTests();
	RunAccessControllerLockoutTest();
	RunAccessControllerShortPinTest();
	RunAccessControllerLongPinTest();
	RunAccessControllerClearPinTest();
	RunAccessControllerLockoutIgnoresInputTest();
	RunAccessControllerInvalidKeysTest();

	std::cout << "All tests passed\n";

	return 0;
}
