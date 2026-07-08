#ifndef KEYPAD_H
#define KEYPAD_H

#ifdef __cplusplus
extern "C" {
#endif

void Keypad_Init(void);
char Keypad_Scan(void);

#ifdef __cplusplus
}
#endif

#endif