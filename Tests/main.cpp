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

void RunKeypadAdapterInitOrderTest();
void RunKeypadAdapterScanTest();

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

	RunKeypadAdapterInitOrderTest();
	RunKeypadAdapterScanTest();

	std::cout << "All tests passed\n";

	return 0;
}
