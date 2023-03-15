#include <Arduino.h>
#include <SPI.h>

static uint8_t heartBit_pin;
static uint32_t heartBit_ms;
void heartBit_init(uint8_t pin, uint32_t ms);
void heartBit_loop(void);

#define RGB_ARRAY_LED 12
typedef struct{
  uint32_t start3ones : 3;
  uint32_t brightness : 5;//global
  uint32_t BLUE       : 8;
  uint32_t GREEN      : 8;
  uint32_t RED        : 8;
}rgbFrame_t;
typedef struct{
  uint32_t   startFrame;
  rgbFrame_t LED[RGB_ARRAY_LED];
  uint32_t   stopFrame;
}rgbArray_t;
typedef enum{
  ARJ_0  = 0,
  ARJ_1  = 1,
  ARJ_2  = 2,
  ARJ_3  = 3,
  ARJ_4  = 4,
  ARJ_5  = 5,
  ARJ_6  = 6,
  ARJ_7  = 7,
  ARJ_8  = 8,
  ARJ_9  = 9,
  ARJ_10 = 10,
  ARJ_11 = 11,
  ARJ_12 = 12,
  ARJ_13 = 13,
  ARJ_14 = 14,
  ARJ_15 = 15,
  ARJ_16 = 16,
  ARJ_17 = 17,
  ARJ_18 = 18,
  ARJ_19 = 19,
  ARJ_20 = 20,
  ARJ_21 = 21,
  ARJ_22 = 22,
  ARJ_23 = 23,
  ARJ_24 = 24,
  ARJ_25 = 25,
  ARJ_26 = 26,
  ARJ_27 = 27,
  ARJ_28 = 28,
  ARJ_29 = 29,
  ARJ_30 = 30,
  ARJ_31 = 31
}brightness_t;
typedef enum{
  reserved = 0,
}grayLevel_t;
typedef enum{
  LED_R,
  LED_G,
  LED_B,
}ledColor_t;

static rgbArray_t rgbArray;
static uint32_t rgbArray_ms;
void rgbArray_init(uint32_t ms);
void rgbArray_loop(void);
void rgbFrame_init(rgbFrame_t *Frame, uint32_t r, uint32_t g, uint32_t b);
void rgbArray_feed(rgbArray_t *frame, uint32_t r, uint32_t g, uint32_t b);
void setColor(ledColor_t color, uint32_t value);
void NextColor(ledColor_t *color);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting...");
  rgbArray_init(10);
  // heartBit_init(LED_BUILTIN, 500);
}

void loop() {
  // put your main code here, to run repeatedly:
  // heartBit_loop();
  rgbArray_loop();
}

void heartBit_init(uint8_t pin, uint32_t ms){
  heartBit_pin = pin;
  heartBit_ms  = ms;
  pinMode(heartBit_pin, OUTPUT);
}
void heartBit_loop(void){
  static uint32_t curTick;
  static uint32_t nexTick;
  curTick = millis();
  if(nexTick < curTick){
    nexTick = curTick + heartBit_ms;
    digitalWrite(heartBit_pin, !digitalRead(heartBit_pin));
  }
}

void rgbArray_init(uint32_t ms){
  rgbArray_ms  = ms;
  SPI.begin();
  rgbArray_feed(&rgbArray, 0x00, 0x00, 0x00);
  SPI.transfer(&rgbArray, sizeof(rgbArray));
}
void rgbArray_loop(void){
  static uint32_t curTick;
  static uint32_t nexTick;
  // static brightness_t brigh = ARJ_0;
  // static uint32_t *pBrigh   = (uint32_t*)&brigh;

  curTick = millis();
  if(nexTick < curTick){
    static ledColor_t color = LED_R;
    static bool op = false;
    static uint32_t value = 0;
    nexTick = curTick + rgbArray_ms;
    // rgbArray_feed(&rgbArray, value, value, value);
    setColor(color, value);
    Serial.printf("value: 0x%02x, 0x%02x, 0x%02x\n", rgbArray.LED[0].RED, rgbArray.LED[0].GREEN, rgbArray.LED[0].BLUE);
    switch(op){
      case true:
        value--;
        if(value == 0){
          op = false;
          value++;
          NextColor(&color);
        }
        break;
      case false:
        value++;
        if(value > 255){
          op = true;
          value--;
        }
        break;
    }
    
    
    SPI.transfer(&rgbArray, sizeof(rgbArray));

    /*
    // if(!value){
    //   value = 0xFF;
    // }
    // else{
    //   value = 0x00;
    // }

    // if(++*pBrigh > ARJ_31){
    //   *pBrigh = ARJ_0;
    // }
    */
  }
}
void rgbFrame_init(rgbFrame_t *Frame, uint32_t r, uint32_t g, uint32_t b){
  Frame->start3ones = 0x7;
  Frame->brightness = 0x1F;//global
  Frame->BLUE       = b;
  Frame->GREEN      = g;
  Frame->RED        = r;
}
void rgbArray_feed(rgbArray_t *frame, uint32_t r, uint32_t g, uint32_t b){
  frame->startFrame = 0x00000000;
  for(int i = 0; i < RGB_ARRAY_LED; i++){
    rgbFrame_init(&frame->LED[i], r, g, b);
  }
  frame->stopFrame  = 0xFFFFFFFF;
}
void NextColor(ledColor_t *color){
  switch(*color){
  case LED_R:
    *color = LED_G;
    break;
  case LED_G:
    *color = LED_B;
    break;
  case LED_B:
    *color = LED_R;
    break;
  
  default:
    break;
  }
}
void setColor(ledColor_t color, uint32_t value){
  switch(color){
  case LED_R:
    rgbArray_feed(&rgbArray, value, 0x00, 0x00);
    break;
  case LED_G:
    rgbArray_feed(&rgbArray, 0x00, value, 0x00);
    break;
  case LED_B:
    rgbArray_feed(&rgbArray, 0x00, 0x00, value);
    break;
  
  default:
    break;
  }
}