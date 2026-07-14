#include <iostream>

void RunPinValidatorTests();
void RunAttemptCounterTests();
void RunAccessControllerTests();
void RunAccessControllerLockoutTest();

int main()
{
    RunPinValidatorTests();
    RunAttemptCounterTests();
    RunAccessControllerTests();
    RunAccessControllerLockoutTest();

    std::cout << "All tests passed\n";

    return 0;
}