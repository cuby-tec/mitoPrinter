#ifndef A4988_H
#define A4988_H

#include <stdint.h>

//MS1	MS2 	MS3
//L		L		L		Full step
//H 	L 		L 		Half Step
//L 	H 		L 		Quarter Step
//H 	H 		L 		Eighth Step
//H 	H 		H 		Sixteenth Step

#define Full_Step       0x00000000
#define Half_Step       0x00000001
#define Quarter_Step    0x00000002
#define Eighth_Step     0x00000003
#define Sixteenth_Step  0x00000007

const uint8_t  microstepTable[5] = {Full_Step,Half_Step,Quarter_Step,Eighth_Step,Sixteenth_Step};


#endif // A4988_H
