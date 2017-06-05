#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <EEPROM.h>
#include <avr/pgmspace.h>

#define EEPROM_STEP_ADDRESS 0    // Where to store effect step

#define PIN 6

#define PIXEL_COUNT 30

#define BRIGHTNESS 32

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT , PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(BRIGHTNESS);  
}



void loop() {
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  /*
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue
*/
//while (1) sinewave(20);


  
  byte effect_step = EEPROM.read(EEPROM_STEP_ADDRESS);

  effect_step++;

  if (effect_step >=4) effect_step = 0;

  EEPROM.write(EEPROM_STEP_ADDRESS, effect_step);

  while (1) {

    switch (effect_step) {
  
      case 0:
        rainbow(20);
        break;

       case 1:
          rainbowCycle(20);
          break;

       case 2: 
          breath(20);
          break;
       
       case 3: 
          sinewave(2);
          break;
      
    }
  
  }


  //theaterChaseRainbow(50);
}


void sinewave(uint8_t wait) {


  uint8_t pixels = 12;// PIXEL_COUNT / 2; // only care about length

  for( uint8_t step=0; step<100; step++ ) {

    int next_p = min( 12,  ( ( cos( ((step/100.0) * 2 * PI) + PI  ) + 1.0 ) / 2.0 ) * pixels);

     for(int8_t p=0; p<pixels; p+=1) {

        if (p==next_p || p==next_p+1 || p==next_p+2) {

           strip.setPixelColor(p*2, strip.Color( 0, 255 , 0 ) );
           strip.setPixelColor(p*2 +1 , strip.Color( 0, 255 , 0 ) );
           
          
        } else {

           strip.setPixelColor(p*2, strip.Color( 0, 0, 0 ) );
           strip.setPixelColor(p*2 +1 , strip.Color( 0, 0 , 0 ) );
          
        }

        
     }


     strip.show();

     delay(2);

    
  }

  
}

void shoot(void) {

  for(uint8_t wait = 20; wait>0; wait--) {

    for(float location=0; location< 100 ; location++ ) {      // 100 steps up the shaft

      for(int8_t p=0; p<PIXEL_COUNT; p+=2) {

        uint8_t brightness = 10 - min( 10 , 10 - 
          abs( 
            location - 
            ((p * 100.0) /PIXEL_COUNT)
          )   
        ) ;

        strip.setPixelColor(p, strip.Color( brightness * 25 , 0 , 0 ) );

      }

      strip.show();
      delay(100);
    }
  
  }


}


const PROGMEM uint8_t breath_data[] =  {
  64, 68, 72, 76, 81, 85, 90, 95, 100, 105, 110, 116, 121, 127, 132,  138, 
  144, 150, 156, 163, 169, 176, 182, 189, 196, 203, 211, 218, 225, 233, 241, 249,
  255, 249, 241, 233, 225, 218, 211, 203, 196, 189, 182, 176, 169, 163, 156, 150,
  144, 138, 132, 127, 121, 116, 110, 105, 100, 95, 90, 85, 81, 76, 72, 68
};

void breath(uint8_t wait) {


  int8_t len = sizeof( breath_data) / sizeof( *breath_data );

  int8_t i=0;

  for( int8_t direction = 1; direction == 1 || direction == -1; direction-=2 ) {

    do {
      
     uint8_t color = pgm_read_byte_near( breath_data + i);

      for(uint8_t p=0; p<strip.numPixels(); p++) {
          strip.setPixelColor(p, strip.Color( 0 , 0 , color  ) );
      }      

      strip.show();
      delay(wait);

      i+= direction;
    
    } while (i>0 && i<(len-1));
  
  }
  
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
