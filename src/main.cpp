#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <esp_random.h>

#include "constants.h"
#include "sprites.h"

using namespace std;

Adafruit_NeoPixel pixels(NUMPIXELS, NP_PIN, NEO_GRB + NEO_KHZ800);

uint8_t state = ST_NORM;

// color array - TODO: Store me to EEPROM!
// state, screen (eye, center), rgb
// double colors[][2][3] = { 

//   {{ 0.3961, 0.1020, 0.6902 }, { 0.3961, 0.1020, 0.6902 }},   // NORM
//   {{ 0.0157, 0.1922, 0.9765 }, { 0.0157, 0.1922, 0.9765 }},
//   {{ 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0 }},
//   {{ 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0 }} 

// };


// state -> screen {eye, center} -> {r,g,b}
byte colors[][2][3] = {

  {{ 101, 26, 176 }, { 101, 26, 176 }},   // NORM
  {{ 4, 49, 249 }, { 4, 49, 249 }},
  {{ 1, 1, 1 }, { 1, 1, 1 }},
  {{ 1, 1, 1 }, { 1, 1, 1 }} 

};

unsigned long switchMillis = 0;  // TODO: testing - remove me
bool updated = false; // TODO: testing - remove me
unsigned char switchCount = 0;  // TODO: testing - remove me

void setAll_sprite(uint8_t spriteLefthandEye[64], uint8_t spriteCenter[256], uint8_t spriteRighthandEye[64], byte color[][2][3], uint8_t state);
void setCenter_sprite(uint8_t sprite[256], byte color[][2][3], uint8_t state);
void setLefthandEye_sprite(uint8_t sprite[64], byte color[][2][3], uint8_t state);
void setRighthandEye_sprite(uint8_t sprite[64], byte color[][2][3], uint8_t state);

void setup() {

  Serial.begin(115200);

  state = ST_NORM;

  pixels.begin();
  pixels.clear();

  pixels.setBrightness(10);

  setAll_sprite(Sprites::eye_lefthand_norm, Sprites::center_blep3, Sprites::eye_righthand_norm, colors, state);

  switchMillis = millis(); // TODO: testing - remove me

}

// RUNTIME VARS FOR ANIMATIONS:
// blinking
bool anim_BlinkEnabled = true;
unsigned long anim_BlinkEventNextStartingMillis = millis() + ANIM_BLINK_DELAY_MS;
bool anim_BlinkStagesStarted[] = {false, false, false};

// speaking
bool anim_SpeakNormEnabled = true;
bool anim_SpeakBlepEnabled = false;

unsigned long anim_SpeakLastActivationMillis = 0;
unsigned long anim_SpeakAnimStartedMillis = 0;
bool anim_SpeakTriggered = false;
unsigned char anim_SpeakState = 0;
bool anim_SpeakScreenUpdated = false;

// blep transition (forwards)
unsigned long anim_BlepTransitionStartingMillis = 0;
bool anim_BlepTransitionStagesStarted[] = {false, false, false};

void loop() {

  // TODO: testing - remove me
  if (millis() > (switchMillis + 10000)) {

    if (state == ST_NORM && switchCount < 5) { state = ST_BLEPTRANSITION; }
    else if (state == ST_NORM && switchCount > 5) { switchCount = 0; state = ST_UWU; }
    else if (state == ST_UWU) { state = ST_NORM; }
    else if (state == ST_BLEP) { state = ST_BLEPTRANSITION_R; }

    switchMillis = millis();
    switchCount += 1;
    updated = false;
  }

  switch(state) {

    default:

    case ST_NORM:

      if (!updated) { 

        setAll_sprite(Sprites::eye_lefthand_norm, Sprites::center_norm, Sprites::eye_righthand_norm, colors, state); 

        anim_BlinkEnabled = true;
        anim_BlinkEventNextStartingMillis = millis() + ANIM_BLINK_DELAY_MS;

        anim_SpeakNormEnabled = true;
        
        updated = true; 

      }

      break;

    case ST_BLEPTRANSITION:

      anim_BlinkEnabled = false;
      anim_SpeakNormEnabled = false;

      if (!anim_BlepTransitionStagesStarted[0]) {

        anim_BlepTransitionStagesStarted[0] = true;
        anim_BlepTransitionStartingMillis = millis();

        setCenter_sprite(Sprites::center_blep0, colors, ST_NORM);
        pixels.show();

      } else if (millis() > (anim_BlepTransitionStartingMillis + (ANIM_BLEPTRANSITION_DURATION_MS / 3)) && !anim_BlepTransitionStagesStarted[1]) {

        anim_BlepTransitionStagesStarted[1] = true;
        setAll_sprite(Sprites::eye_lefthand_blink1, Sprites::center_blep1, Sprites::eye_righthand_blink1, colors, ST_NORM);

      } else if (millis() > (anim_BlepTransitionStartingMillis + ( 2 * (ANIM_BLEPTRANSITION_DURATION_MS / 3))) && !anim_BlepTransitionStagesStarted[2]) {

        anim_BlepTransitionStagesStarted[2] = true;
        setAll_sprite(Sprites::eye_lefthand_blink1, Sprites::center_blep2, Sprites::eye_righthand_blink1, colors, ST_NORM);

      } else if (millis() > (anim_BlepTransitionStartingMillis + ANIM_BLEPTRANSITION_DURATION_MS)) {

        setAll_sprite(Sprites::eye_lefthand_blink2, Sprites::center_blep3, Sprites::eye_righthand_blink2, colors, ST_NORM);

        anim_BlepTransitionStagesStarted[0] = false;
        anim_BlepTransitionStagesStarted[1] = false;
        anim_BlepTransitionStagesStarted[2] = false;

        state = ST_BLEP;

      }
      
      break;

    case ST_BLEPTRANSITION_R:

      anim_BlinkEnabled = false;
      anim_SpeakNormEnabled = false;

      if (!anim_BlepTransitionStagesStarted[0]) {

        anim_BlepTransitionStagesStarted[0] = true;
        anim_BlepTransitionStartingMillis = millis();

        setAll_sprite(Sprites::eye_lefthand_blink2, Sprites::center_blep3, Sprites::eye_righthand_blink2, colors, ST_NORM);
        pixels.show();

      } else if (millis() > (anim_BlepTransitionStartingMillis + (ANIM_BLEPTRANSITION_DURATION_MS / 3)) && !anim_BlepTransitionStagesStarted[1]) {

        anim_BlepTransitionStagesStarted[1] = true;
        setAll_sprite(Sprites::eye_lefthand_blink1, Sprites::center_blep2, Sprites::eye_righthand_blink1, colors, ST_NORM);

      } else if (millis() > (anim_BlepTransitionStartingMillis + ( 2 * (ANIM_BLEPTRANSITION_DURATION_MS / 3))) && !anim_BlepTransitionStagesStarted[2]) {

        anim_BlepTransitionStagesStarted[2] = true;
        setAll_sprite(Sprites::eye_lefthand_blink1, Sprites::center_blep1, Sprites::eye_righthand_blink1, colors, ST_NORM);

      } else if (millis() > (anim_BlepTransitionStartingMillis + ANIM_BLEPTRANSITION_DURATION_MS)) {

        setAll_sprite(Sprites::eye_lefthand_blink1, Sprites::center_blep0, Sprites::eye_righthand_blink1, colors, ST_NORM);

        anim_BlepTransitionStagesStarted[0] = false;
        anim_BlepTransitionStagesStarted[1] = false;
        anim_BlepTransitionStagesStarted[2] = false;

        state = ST_NORM;

      }
      
      break;

    case ST_BLEP:

      anim_BlinkEnabled = false;
      anim_SpeakNormEnabled = false;

      if (!updated) { setAll_sprite(Sprites::eye_lefthand_blink2, Sprites::center_blep3, Sprites::eye_righthand_blink2, colors, ST_NORM); updated = true; }

      break;

    case ST_UWU:

      anim_BlinkEnabled = false;
      anim_SpeakNormEnabled = false;

      if (!updated) { setAll_sprite(Sprites::eye_lefthand_u, Sprites::center_uwu, Sprites::eye_righthand_u, colors, 1); updated = true; }

  }

  // these will override anything manually set
  // disable this when using alternative sprites or manual pixel setting  

  // blinking routune
  if (anim_BlinkEnabled) {

    // frame 1
    if (millis() > anim_BlinkEventNextStartingMillis && !anim_BlinkStagesStarted[0]) {

      anim_BlinkStagesStarted[0] = true;

      setLefthandEye_sprite(Sprites::eye_lefthand_blink1, colors, state);
      setRighthandEye_sprite(Sprites::eye_righthand_blink1, colors, state);

      pixels.show();

    } else if ((millis() > (anim_BlinkEventNextStartingMillis + (ANIM_BLINK_DURATION_MS / 3))) && !anim_BlinkStagesStarted[1]) {  // blink frame 2

      anim_BlinkStagesStarted[1] = true;

      setLefthandEye_sprite(Sprites::eye_lefthand_blink2, colors, state);
      setRighthandEye_sprite(Sprites::eye_righthand_blink2, colors, state);

      pixels.show();

    } else if ((millis() > (anim_BlinkEventNextStartingMillis + (2 * (ANIM_BLINK_DURATION_MS / 3)))) && !anim_BlinkStagesStarted[2]) {  // blink frame 1

      anim_BlinkStagesStarted[2] = true;

      setLefthandEye_sprite(Sprites::eye_lefthand_blink1, colors, state);
      setRighthandEye_sprite(Sprites::eye_righthand_blink1, colors, state);

      pixels.show();

    } else if (millis() > ( anim_BlinkEventNextStartingMillis + ANIM_BLINK_DURATION_MS )) {  // end action

      double jitterAdder = esp_random() * ( (double)ANIM_BLINK_DELAY_JITTER_MS / (double)UINT32_MAX );
      anim_BlinkEventNextStartingMillis = millis() + ANIM_BLINK_DELAY_MS + (unsigned long)jitterAdder;

      anim_BlinkStagesStarted[0] = false;
      anim_BlinkStagesStarted[1] = false;
      anim_BlinkStagesStarted[2] = false;

      setLefthandEye_sprite(Sprites::eye_lefthand_norm, colors, state);
      setRighthandEye_sprite(Sprites::eye_righthand_norm, colors, state);

      pixels.show();

    }

  }

  // speaking routine
  if (anim_SpeakLastActivationMillis + millis() > ANIM_SPEAK_TIMEOUT_PERIOD_MILLIS) { anim_SpeakTriggered = false; }
  if (digitalRead(MICROPHONE_INPUT_PIN)) { anim_SpeakTriggered = true; anim_SpeakLastActivationMillis = millis(); }

  if (anim_SpeakNormEnabled && false) {

    switch (anim_SpeakState) {
      
      default:

      case SPEAKST_NORM_IDLE:

        if (!anim_SpeakScreenUpdated) {
          setCenter_sprite(Sprites::center_norm, colors, ST_NORM);
          pixels.show();
          anim_SpeakScreenUpdated = true;
        }

        if (anim_SpeakTriggered) {
          anim_SpeakState = SPEAKST_NORM_FORWARD_ANIM;
          anim_SpeakScreenUpdated = false;
          anim_SpeakAnimStartedMillis = millis();
        }

        break;

      case SPEAKST_NORM_FORWARD_ANIM:


        if (!anim_SpeakScreenUpdated) {
          setCenter_sprite(Sprites::center_speak1, colors, ST_NORM);
          pixels.show();
          anim_SpeakScreenUpdated = true;
        }

        if (millis() > anim_SpeakAnimStartedMillis + (ANIM_SPEAK_DURATION_MS / 2)) {

          setCenter_sprite(Sprites::center_speak2, colors, ST_NORM);

          anim_SpeakScreenUpdated = false;
          anim_SpeakState = SPEAKST_NORM_HOLD_ON;

        }

        break;

      case SPEAKST_NORM_REVERSE_ANIM:

        break;

      case SPEAKST_NORM_HOLD_ON:

        if (!anim_SpeakTriggered) {
          anim_SpeakState = SPEAKST_NORM_IDLE;
          anim_SpeakScreenUpdated = false;
        }

        break;

    }

  }

}

void setRighthandEye_sprite(uint8_t sprite[64], byte color[][2][3], uint8_t state) {
  
  for (int i = 0; i < EYE_SIZE; i++) {

    uint8_t r = sprite[i] * (color[state][1][0] / 255.0);
    uint8_t g = sprite[i] * (color[state][1][1] / 255.0);
    uint8_t b = sprite[i] * (color[state][1][2] / 255.0);

    pixels.setPixelColor(i, pixels.Color(r, g, b));

  }

}

void setLefthandEye_sprite(uint8_t sprite[64], byte color[][2][3], uint8_t state) {

    for (int i = 0; i < EYE_SIZE; i++) {

    uint8_t r = sprite[i] * (color[state][1][0] / 255.0);
    uint8_t g = sprite[i] * (color[state][1][1] / 255.0);
    uint8_t b = sprite[i] * (color[state][1][2] / 255.0);

      pixels.setPixelColor(i + (EYE_SIZE + CENTER_SIZE), pixels.Color(r, g, b));

  }
  
}

void setCenter_sprite(uint8_t sprite[256], byte color[][2][3], uint8_t state) {

  for (int i = 0; i < CENTER_SIZE; i++) {

    uint8_t r = sprite[i] * (color[state][1][0] / 255.0);
    uint8_t g = sprite[i] * (color[state][1][1] / 255.0);
    uint8_t b = sprite[i] * (color[state][1][2] / 255.0);

    pixels.setPixelColor(i + EYE_SIZE, pixels.Color(r, g, b));

  }
  
}

void setAll_sprite(uint8_t spriteLefthandEye[64], uint8_t spriteCenter[256], uint8_t spriteRighthandEye[64], byte color[][2][3], uint8_t state) {
  setRighthandEye_sprite(spriteRighthandEye, color, state);
  setCenter_sprite(spriteCenter, color, state);
  setLefthandEye_sprite(spriteLefthandEye, color, state);
  pixels.show();
}
