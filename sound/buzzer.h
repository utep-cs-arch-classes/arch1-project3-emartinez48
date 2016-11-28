#ifndef buzzer_included
#define buzzer_included

//12 pre-defined pitches
#define _C 61156
#define _Db 57723
#define _D 54484
#define _Eb 51426
#define _E 48539
#define _F 45815
#define _Gb 43244
#define _G 40816
#define _Ab 38526
#define _A 36364
#define _Bb 34322
#define _B 32396

/**
 * Plays a sound at a ptich and octave
 * @param pitch  the ptich the sound is at
 * @param octave the octave the pitch is at
 */
void sound(long pitch, char octave);

/**
 * setups buzzer use
 */
void buzzer_init();

/**
 * set the period of the
 * @param cycles [description]
 */
void buzzer_set_period(short cycles);

extern char buzzer_on;

#endif // included
