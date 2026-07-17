#include <iostream>

void RunPinValidatorTests();

void RunAttemptCounterTests();

void RunAccessControllerTests();
void RunAccessControllerLockoutTest();
void RunAccessControllerShortPinTest();
void RunAccessControllerLongPinTest();
void RunAccessControllerClearPinTest();

int main()
{
	RunPinValidatorTests();
	
	RunAttemptCounterTests();

	RunAccessControllerTests();
	RunAccessControllerLockoutTest();
	RunAccessControllerShortPinTest();
	RunAccessControllerLongPinTest();
	RunAccessControllerClearPinTest();

	std::cout << "All tests passed\n";

	return 0;
}