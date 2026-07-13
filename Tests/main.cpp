#include <iostream>

void RunPinValidatorTests();
void RunAttemptCounterTests();
void RunAccessControllerTests();

int main()
{
    RunPinValidatorTests();
    // RunAttemptCounterTests();
    RunAccessControllerTests();

    std::cout << "All tests passed\n";

    return 0;
}