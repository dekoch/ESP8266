
#include <stdio.h>

#include <Adafruit_GFX.h>       // Core graphics library
#include <Adafruit_ST7735.h>    // Hardware-specific library
#include <SPI.h>
#include <Timezone.h>           //https://github.com/JChristensen/Timezone
#include <ESP8266TrueRandom.h>  //https://github.com/marvinroger/ESP8266TrueRandom
#include "mandelbrot.h"

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )


/*
wiring

TFT     ESP8266

VCC     VCC, EN
GND     GND, GPI015
SCL     GPI014
SDA     GPI013
RS/DC   GPI02
RES     RST
CS      GPI05
*/

#define TFT_CS     5
#define TFT_RST    0
#define TFT_DC     2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

int intImageWidth = 128;
int intImageHeight = 160;


//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t cet;

time_t previousCET;


long lPrevMill = 0;


Mandelbrot mandel;

struct set
{
   float fXmin;
   float fXmax;
   float fYmin;
   float fYmax;
   int intIteration;
};

//http://www.atopon.org/mandel
set mandelset[11];

boolean bolWorking = false;
int intCurrentY = 0;

int intCurrentSet = 1;


void setup(void) 
{
  Serial.begin(115200);

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  tft.fillScreen(ST7735_BLACK);
  
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);

  tft.println("starting...");

  mandelset[1].fXmin = -2.2;
  mandelset[1].fXmax = 0.6;
  mandelset[1].fYmin = -1.2;
  mandelset[1].fYmax = 1.2;
  mandelset[1].intIteration = 65;

  mandelset[2].fXmin = -1.14375;
  mandelset[2].fXmax = -1.0975;
  mandelset[2].fYmin = 0.26875;
  mandelset[2].fYmax = 0.31499;
  mandelset[2].intIteration = 30;

  mandelset[3].fXmin = -1.27604166;
  mandelset[3].fXmax = -1.22430555;
  mandelset[3].fYmin = -0.13784722;
  mandelset[3].fYmax = -0.08611111;
  mandelset[3].intIteration = 50;

  mandelset[4].fXmin = -1.24809698;
  mandelset[4].fXmax = -1.23779766;
  mandelset[4].fYmin = -0.12339584;
  mandelset[4].fYmax = -0.11309652;
  mandelset[4].intIteration = 80;

  mandelset[5].fXmin = -1.44569444;
  mandelset[5].fXmax = -1.41138888;
  mandelset[5].fYmin = -0.01717592;
  mandelset[5].fYmax = 0.01717592;
  mandelset[5].intIteration = 30;

  mandelset[6].fXmin = -0.79705671;
  mandelset[6].fXmax = -0.78599305;
  mandelset[6].fYmin = 0.15634722;
  mandelset[6].fYmax = 0.16741087;
  mandelset[6].intIteration = 65;

  mandelset[7].fXmin = -0.79004972;
  mandelset[7].fXmax = -0.78854179;
  mandelset[7].fYmin = 0.16365377;
  mandelset[7].fYmax = 0.16515171;
  mandelset[7].intIteration = 75;

  mandelset[8].fXmin = -0.75007976;
  mandelset[8].fXmax = -0.74952064;
  mandelset[8].fYmin = -0.04186508;
  mandelset[8].fYmax = -0.04130596;
  mandelset[8].intIteration = 250;

  mandelset[9].fXmin = 0.16979166;
  mandelset[9].fXmax = 0.19916666;
  mandelset[9].fYmin = -0.60187499;
  mandelset[9].fYmax = -0.57249999;
  mandelset[9].intIteration = 60;

  mandelset[10].fXmin = 0.17832764;
  mandelset[10].fXmax = 0.17861277;
  mandelset[10].fYmin = -0.58028664;
  mandelset[10].fYmax = -0.5800015;
  mandelset[10].intIteration = 260;

  mandelset[0] = mandelset[intCurrentSet];

  mandel.SetImageColors(ST7735_BLACK, ST7735_BLUE);
  mandel.SetImageSize(intImageWidth, intImageHeight);

  draw();
  
  tft.println("initialized");
  
  delay(2000);

  tft.fillScreen(ST7735_BLACK);
}

void loop() 
{
  previousCET = cet;
  cet = CE.toLocal(now(),&tcr);
  unsigned long currentMillis = millis();

  if (currentMillis - lPrevMill >= 40000)
  {
    lPrevMill = currentMillis;
    
    intCurrentSet++;

    if (intCurrentSet >= arr_len(mandelset))
    {
      intCurrentSet = 1;
    }


    //intCurrentSet = ESP8266TrueRandom.random(1, arr_len(mandelset));

    
    mandelset[0] = mandelset[intCurrentSet];
    
    draw();


    //int intInvert = ESP8266TrueRandom.random(2);

    //if (intInvert == 0)
    //{
    //  mandel.SetImageColors(ST7735_BLACK, ST7735_BLUE);
    //}
    //else
    //{
    //  mandel.SetImageColors(ST7735_BLUE, ST7735_BLACK);
    //}
  }

  if (second(previousCET) != second(cet))
  {
    
  }
  

  update();

}


void draw() {
  mandel.SetMandelbrotParameters(mandelset[0].fXmin, mandelset[0].fXmax, mandelset[0].fYmin, mandelset[0].fYmax, mandelset[0].intIteration);
  bolWorking = true;
  intCurrentY = 0;
}

void update()
{
  if (bolWorking)
  { 
    RenderLine(intCurrentY);  
        
    if (intCurrentY == intImageHeight)
    {
      bolWorking = false;
    } 
       
    intCurrentY++;
  }
}

void RenderLine(int y) 
{
  uint16_t col;
  
  for(int x = 0; x < intImageWidth; ++x)
  {     
    col = mandel.RenderPixel(x, y);
    tft.drawPixel(x, y, col);    
  }
}
   
