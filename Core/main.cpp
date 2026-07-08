#include "KeypadAdapter.hpp"
#include "LockOutputAdapter.hpp"

int main()
{
    KeypadAdapter keypad;
    LockOutputAdapter lock;

    lock.Unlock();

    while(true)
    {
        volatile char pressed_key = keypad.Scan();

        if(key == '#') {
            lock.Lock();
        }

    }
}