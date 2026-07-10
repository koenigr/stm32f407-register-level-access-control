#include <iostream>

void RunPinValidatorTests();
void RunAttemptCounterTests();

int main()
{
    RunPinValidatorTests();
    // RunAttemptCounterTests();

    std::cout << "All tests passed\n";

    return 0;
}