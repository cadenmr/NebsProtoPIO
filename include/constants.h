#include <Arduino.h>

using namespace std;

// Neopixel Grid Settings
#define NP_PIN 9
#define CENTER_SIZE 256
#define EYE_SIZE 64
#define NUMPIXELS CENTER_SIZE + EYE_SIZE + EYE_SIZE 

// Timings
#define ANIM_BLINK_DURATION_MS 175
#define ANIM_BLINK_DELAY_MS 2200
#define ANIM_BLINK_DELAY_JITTER_MS 1000

#define ANIM_BLEPTRANSITION_DURATION_MS 270

// Speech
#define MICROPHONE_INPUT_PIN D2
#define ANIM_SPEAK_TIMEOUT_PERIOD_MILLIS 400
#define ANIM_SPEAK_DURATION_MS 150

// States
#define ST_NORM 0    // blink cycle + normal speak
#define ST_BLEPTRANSITION 1
#define ST_BLEPTRANSITION_R 2
#define ST_BLEP 3
#define ST_UWU 4    // UwU face, no anim

#define SPEAKST_NORM_IDLE 0
#define SPEAKST_NORM_FORWARD_ANIM 1
#define SPEAKST_NORM_REVERSE_ANIM 2
#define SPEAKST_NORM_HOLD_ON 3