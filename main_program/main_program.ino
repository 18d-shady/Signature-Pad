
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



XPT2046 touch(CS_PIN, TIRQ_PIN);



//define buttons for ui
Adafruit_GFX_Button buttons[5];


//initialization
char page;
int inkColour, backgroundColour;
uint16_t noteScreen = 0, x, y, new_x, new_y;
int confirmm = 0;


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

  //pc button
  buttons[2].initButton(&tft, 170, 220, 50, 20, ILI9341_LIGHTGREY, ILI9341_BLACK, ILI9341_WHITE, "PC", BUTTON_TEXTSIZE);

  buttons[3].initButton(&tft, 170, 220, 50, 20, ILI9341_LIGHTGREY, ILI9341_BLACK, ILI9341_WHITE, "PAD", BUTTON_TEXTSIZE);
  welcomePage();
  //delay(5000);
  

  while (confirmm == 0) {
    Serial.write("SPad connecting to PC..");
    if (Serial.available() > 0){
      String termtext = Serial.readString();
      termtext.trim();
      if (termtext.substring(0) == "PC connected to SPad"){
        confirmm = 1;
      }
      else{
        confirmm = 1;
      }
    }
    
    delay(1500);
    
  }

  page = 'n';
  delay(1000);
   

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
  else if (page == 'm'){
    notePage2();
    notePageTouch2();
    //getClick();
  }

  
}


void getClick(){
  
  while(!touch.isTouching()){
    delay(1);
  }
  touch.getPosition(x, y);
  new_x = 310-y;
  new_y = x;
  //Serial.println(new_x, new_y);
}

void welcomePage(){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(5);
  tft.setCursor(30, 60);  tft.println("SIGNATURE");
  tft.setCursor(30, 130);  tft.println("PAD");
  delay(1000);
}


void notePage(){
  delay(500);
  Serial.write("pdScreen");
  delay(500);
  tft.fillScreen(ILI9341_WHITE);
  inkColour =  ILI9341_BLACK;
  backgroundColour = ILI9341_WHITE;
  buttons[0].drawButton();
  buttons[1].drawButton();
  buttons[2].drawButton();
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
        ScreenShot();
        delay(2000);
        savedSuccessfully();
      }

      else if (buttons[2].contains(new_x, new_y)){
        buttons[2].drawButton(true);
        page = 'm';
      }
    
    }
    
    else{
      tft.fillCircle(new_x, new_y, 1, ILI9341_BLACK);
      noteScreen = 1;
    }
  }
}

void notePage2(){
  delay(500);
  Serial.write("pcScreen");
  delay(500);
  tft.fillScreen(ILI9341_WHITE);
  inkColour =  ILI9341_BLACK;
  backgroundColour = ILI9341_WHITE;
  buttons[0].drawButton();
  buttons[1].drawButton();
  buttons[3].drawButton();
}



void notePageTouch2(){
  while (page == 'm'){
    getClick();
    if(new_y > 211 ){
      if (buttons[0].contains(new_x, new_y)){
        buttons[0].drawButton(true);
        Serial.write("pcResign");
        notePage2();
        noteScreen = 0;
      }
  
      else if (buttons[1].contains(new_x, new_y)){
        buttons[1].drawButton(true);
        delay(500);
        Serial.write("donePcSave");
        delay(500);
        savedSuccessfully();
      }

      else if (buttons[3].contains(new_x, new_y)){
        buttons[3].drawButton(true);
        page = 'n';
      }
    
    }
    
    else{
      tft.fillCircle(new_x, new_y, 1, ILI9341_BLACK);
      noteScreen = 1;
      //coord[0] = new_x;
      //coord[1] = new_y;
      //Serial.println((uint8_t*)coord, sizeof(coord));
      int coord[2] = {new_x, new_y};
      Serial.write((uint8_t*)coord, sizeof(coord));
      //Serial.print(sizeof(coord));
      //delay(1000);
    
    }
  }
}







void savedSuccessfully(){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(2);
  tft.setCursor(45, 100);  tft.println("SENT SUCCESSFULLY!");
  delay(1000);
  page = 'n';
  notePage();
}

void saveUnsuccessful(){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_RED);  tft.setTextSize(2);
  tft.setCursor(70, 110);  tft.println("NOT SUCCESSFUL!");
  delay(1000);
  notePage();
}


void ScreenShot() {
  uint8_t color[3*210]; // Line Buffer
  String encodedImage = "";
  tft2.setRotation(0);
  //char pic[3*320*210];

  for (uint16_t y = 0; y < 320; y++) {
    tft2.readRectRGB(0, y, 210, 1, color);
    Serial.write(color, 3 * 210);
    delay(0);
    //encodedImage.concat(base64::encode(color, 3*210));
    
    //strcat(pic, base64::encode(color, 3*210).c_str());
    //Serial.print(base64::encode(color, 3*210));
    
  }
  tft2.setRotation(3);
  
}

 
