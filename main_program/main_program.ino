
#include <SPI.h>
#include <Adafruit_ILI9341esp.h> //include Adafruit TFT libary
#include <Adafruit_GFX.h> //include grafical library
#include <XPT2046.h>
#include <TFT_eSPI.h>      // Hardware-specific library
#include <SoftwareSerial.h>
#include "Base64.h"
#include <base64.h>
/*
#define TFT_CLK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_DC 10
#define TFT_CS 8
#define TFT_RST 9
*/

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

#define TFT_CS 15
#define TFT_DC 2

#define CS_PIN 4
#define TIRQ_PIN 5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TFT_eSPI tft2 = TFT_eSPI(TFT_CS, TFT_DC);
//TFT_ILI9341_ESP tft2 = TFT_ILI9341_ESP(TFT_CS, TFT_DC);

#define BUTTON_SPACING_X 5
#define BUTTON_TEXTSIZE 2
#define BUTTON_H 25


#define NPIXELS 1  // Must be integer division of both TFT width and TFT height
#define BITS_PER_PIXEL 16 

XPT2046 touch(CS_PIN, TIRQ_PIN);

//alphabet buttons
char alphaButtons[26][3] = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
                              "A", "S", "D", "F", "G", "H", "J", "K", "L", 
                                  "Z", "X", "C", "V", "B", "N", "M"};

//define buttons for ui
Adafruit_GFX_Button buttons[30];


//initialization
char page;
int inkColour, backgroundColour;
uint16_t noteScreen = 0, x, y, new_x, new_y;
// the data we store in the save textfield
#define TEXT_LEN 7
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;
#define NOO = 200;
uint8_t r, g, b;


void setup() {
  Serial.begin(115200);
  

  SPI.setFrequency(ESP_SPI_FREQ);
  // put your setup code here, to run once:

  tft.begin();
  tft2.begin();
  touch.begin(tft.width(), tft.height());

  // Replace these calibrations for your screen module
  touch.setCalibration(1814, 220, 272, 1736);
  tft.setRotation(3);
  tft2.setRotation(3);
  Serial.println("Lets begin");
  //resign button
  buttons[0].initButton(&tft, 60, 220, 100, 20, ILI9341_LIGHTGREY, ILI9341_BLACK, ILI9341_WHITE, "RE-SIGN", BUTTON_TEXTSIZE);

  //save button
  buttons[1].initButton(&tft, 280, 220, 60, 20, ILI9341_GREEN, ILI9341_GREEN, ILI9341_WHITE, "SAVE", BUTTON_TEXTSIZE);
  welcomePage();
  delay(5000);
  page = 'n';
  //Serial.begin(921600);
  //delay(10000);
  //savePage();
  //delay(10000);
  
}

void loop() {
 
  // put your main code here, to run repeatedly:

  if (page == 'n'){
    notePage();
    notePageTouch();
    //getClick();
  }
  else if (page == 's'){
    savePage();
    savePageAction();
    //delay(10000);
  }

  
}


void getClick(){
  
  while(!touch.isTouching()){
    delay(1);
  }
  touch.getPosition(x, y);
  new_x = 310-y;
  new_y = x;
}

void welcomePage(){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(5);
  tft.setCursor(30, 60);  tft.println("SIGNATURE");
  tft.setCursor(30, 130);  tft.println("PAD");
  delay(1000);
}


void notePage(){
  tft.fillScreen(ILI9341_WHITE);
  inkColour =  ILI9341_BLACK;
  backgroundColour = ILI9341_WHITE;
  buttons[0].drawButton();
  buttons[1].drawButton();
}



void notePageTouch(){
  while (page == 'n'){
    getClick();
    if(new_y > 211 ){
      if (buttons[0].contains(new_x, new_y)){
        buttons[0].drawButton(true);
        notePage();
        noteScreen = 0;
      }
  
      else if (buttons[1].contains(new_x, new_y)){
        buttons[1].drawButton(true);
        //screenServer();
        ScreenShot2();
        //String imagee = captureTFTScreen();
        //Serial.println(imagee);
        delay(2000);
        page = 's';
      }
    
    }
    
    else{
      tft.fillCircle(new_x, new_y, 1, ILI9341_BLACK);
      noteScreen = 1;
    }
  }
}


void savePage(){
  //text field
  tft.fillScreen(ILI9341_WHITE);
  tft.drawRect(80, 20, 150, 50, ILI9341_BLACK);
  tft.setCursor(90, 30);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setTextSize(3);
  tft.print(textfield);

  //the buttons
  for (uint8_t num=0; num<26; num++){
    if (num < 10){
      buttons[2 + num].initButton(&tft, 25+num*(25+BUTTON_SPACING_X), 100,
                  25, 25, ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE,
                  alphaButtons[num], BUTTON_TEXTSIZE); 
    }
    else if(num < 19){
      int pos = num - 10;
      buttons[2 + num].initButton(&tft, 45+pos*(25+BUTTON_SPACING_X), 135,
                  25, 25, ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE,
                  alphaButtons[num], BUTTON_TEXTSIZE); 
    }
    else{
      int pos = num - 19;
      buttons[2 + num].initButton(&tft, 65+pos*(25+BUTTON_SPACING_X), 170,
                  25, 25, ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE,
                  alphaButtons[num], BUTTON_TEXTSIZE); 
    }
    buttons[2+num].drawButton();
 
  }

  //clear button
  buttons[28].initButton(&tft, 295, 170, 40, BUTTON_H, ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "CLR", BUTTON_TEXTSIZE);
  buttons[28].drawButton();


  //cancel button
  buttons[29].initButton(&tft, 60, 220, 80, BUTTON_H, ILI9341_WHITE, ILI9341_RED, ILI9341_WHITE, "CANCEL", BUTTON_TEXTSIZE);
  buttons[29].drawButton();

  
  buttons[1].drawButton();
}


void savePageAction(){
  while (page == 's'){
    getClick();

    if(buttons[1].contains(new_x, new_y)){
      buttons[1].drawButton(true);
      savedSuccessfully();
      //screenServer();
      //clear screen
    }
    else if(buttons[29].contains(new_x, new_y)){
      buttons[29].drawButton(true);
      page = 'n';
    }

    // check if the buttons were pressed and tick them
    for (uint8_t a=2; a<29; a++) {
      if (buttons[a].contains(new_x, new_y)) {
        //button was pressed
        buttons[a].press(true);  
      } else {
        //button wasnt pressed
        buttons[a].press(false);
      }
    }
    

    for(uint8_t a=2; a<29; a++){
      if (buttons[a].justReleased()){
        //draw normal for released button
        buttons[a].drawButton(); 
      }

      if(buttons[a].justPressed()){
        buttons[a].drawButton(true);
        //for the alphabets
        if(a < 28){
          if(textfield_i < TEXT_LEN){
            textfield[textfield_i] = alphaButtons[a-2][0];
            textfield_i++;
            textfield[textfield_i] = 0; // zero terminate
          }
        }

        //the delete button
        else if(a == 28){
          textfield[textfield_i] = 0;
          if(strlen(textfield) != 0){
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
        }

        tft.setCursor(90, 30);
        tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
        tft.setTextSize(3);
        tft.print(textfield); 
      } 
    }
  }
}

void savedSuccessfully(){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(2);
  tft.setCursor(45, 100);  tft.println("SAVED SUCCESSFULLY!");
  delay(1000);
  page = 'n';
}

void saveUnsuccessful(){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_RED);  tft.setTextSize(2);
  tft.setCursor(70, 110);  tft.println("NOT SUCCESSFUL!");
  delay(1000);
  savePage();
}

void ScreenShot() {
  uint8_t color[2*212]; // Line Buffer
  
   for (uint16_t y = 0; y < 320; y++) {
      tft2.readRect(0, y, 212, 1, (uint16_t*)color);
      Serial.write(color, 2 * 212);
      delay(2);
  }
}

void ScreenShot2() {
  uint8_t color[3 * 210]; // Line Buffer
  //String encodedImage = "";
  //char encodedImage[3 * 240];
  tft2.setRotation(0);

  for (uint16_t y = 0; y < 320; y++) {
    tft2.readRectRGB(0, y, 210, 1, color);
    //encodedImage += base64::encode(color, 3 * 240);
    //strncat(encodedImage, base64::encode(color, 3 * 240));
    //Serial.write(color, 2 * 212);
    delay(0);
    //size_t encoded_len = Base64.encodedLength(212*320*2);
    //char encoded[encoded_len];
    //Base64.encode(encoded, (char*)color, 212*320*2);
    //Serial.println(encoded);
    Serial.print(base64::encode(color, 3 * 210));
    
  }
  tft2.setRotation(3);
  //Serial.print(encodedImage);
  
}

void ScreenShot1(){
  uint8_t color[3 * NPIXELS];
  String encodedImage = "";
  tft2.setRotation(0);

  // Send all the pixels on the whole screen
  for ( uint32_t y = 0; y < tft2.height(); y++) {
    // Increment x by NPIXELS as we send NPIXELS for every byte received
    for ( uint32_t x = 0; x < tft2.width(); x += NPIXELS) {
      delay(0); // Equivalent to yield() for ESP8266;
      // Fetch N RGB pixels from x,y and put in buffer
      tft2.readRectRGB(x, y, NPIXELS, 1, color);
      encodedImage += base64::encode(color, 3 * NPIXELS);
      delay(2);

    }
  }
  Serial.println(tft2.height());
  Serial.println(tft2.width());
  tft2.setRotation(3);
  Serial.print(encodedImage);
}




String captureTFTScreen() {
 tft.setRotation(0);
 tft2.setRotation(0);
 uint16_t width = tft2.width();
 uint16_t height = tft2.height();
 uint8_t buffer[3 * width];
 uint8_t * ptr;
 String encodedImage = "";

 for (uint16_t y = 0; y < height; y++) {
    ptr = buffer;
    for (uint16_t x = 0; x < width; x++) {
      uint16_t color = tft2.readPixel(x, y);
      *ptr++ = color >> 8; // R
      *ptr++ = color;     // G
      *ptr++ = color >> 16; // B
    }
    encodedImage += base64::encode(buffer, 3 * width);
 }
 tft.setRotation(3);
 tft2.setRotation(3);

 return encodedImage;
}

bool ScreenShot3(){
  uint8_t color[3 * NPIXELS]; // RGB and 565 format color buffer for N pixels

  // Send all the pixels on the whole screen
  for ( uint32_t y = 0; y < tft.height(); y++)
  {
    // Increment x by NPIXELS as we send NPIXELS for every byte received
    for ( uint32_t x = 0; x < tft.width(); x += NPIXELS)
    {
      delay(0); // Equivalent to yield() for ESP8266;

#if defined BITS_PER_PIXEL && BITS_PER_PIXEL >= 24 && NPIXELS > 1
      // Fetch N RGB pixels from x,y and put in buffer
      tft2.readRectRGB(x, y, NPIXELS, 1, color);
      // Send buffer to client
      Serial.write(color, 3 * NPIXELS); // Write all pixels in the buffer
    
#else
      // Fetch N 565 format pixels from x,y and put in buffer
      if (NPIXELS > 1) tft2.readRect(x, y, NPIXELS, 1, (uint16_t *)color);
      else
      {
        uint16_t c = tft2.readPixel(x, y);
        color[0] = c>>8;
        color[1] = c & 0xFF;  // Swap bytes
      }
      // Send buffer to client
      Serial.write(color, 2 * NPIXELS); // Write all pixels in the buffer
#endif
    }
  }
  return true;
}

/*USB to TTL Serial Converter (CP2102, CH340, or PL2303 are compatible), and a breadboard. 
Connect the USB port of the NodeMCU to the RX pin of the converter,
 and the TX port of the NodeMCU to the TX pin of the converter. Connect the GND pins of both devices.
 */
 