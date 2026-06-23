int main(void)
{
    GPIO_Init();
    Keypad_Init();

    while(1)
    {
        AccessControl_Run();
    }
}
