/*
Author: Peter Britanyak
Date: 4/5/15
Purpose: Ping SONAR
Rev0- Using 4pin or 3pin Ping unit and Esplora tft display
*/

// BOF preprocessor bug prevent - insert me on top of your arduino-code
// From: http://www.a-control.de/arduino-fehler/?lang=en
#if 1
__asm volatile ("nop");
#endif

// Type of ultrasonic sensor
//#define HC-SR04            // HC-SR04 ultrasonic sensor only pick one
#define Parallax             // Parallax ultrasonic sensor only pick one

#ifdef HC-SR04
const int TRIGPIN = 15;      // HC-SR04 option pins
const int ECHOPIN = 16;      // HC-SR04 option pins
#else                        // #elif Parallax 
const int PINGPIN = 15;      // Parallax option signal pin
#endif
const int SERVOPIN = 14;     // Servo pin
const int sclk = 4;          // SPI pins for TFT display
const int mosi = 5;
const int cs =  6;
const int dc =  7;
const int rst = 8; 

const float pi = 3.1415;            // Used for radian to degree conversion
float r, last_r;
long duration, inches, dist;        // establish variables for maths
int x, y, theta, last_theta; 
int thetaInc, currentInc;
boolean right;

const int MAXRANGE = 108;      // [inch] This sets the max ping distance that will be mapped to the outer circle of the display.
const int DIVISIONS = 20;      // divisions of the view angle. PING unit claims 15 degree viewing angle
const int MAXTHETA = 140;      // Min view angle
const int MINTHETA = 40;       // Max view angle
const int SERVOPAUSE = 500;    // Time to allow servo to increment to new location before taking sample.
const int DOTSIZE = 1;         // Size of the dot on the display

int lay[DIVISIONS+1][2];            // Array stores displayed dot locations.

#include <Servo.h>                  // Servo library
#include <Adafruit_GFX.h>           // Core graphics library
#include <Adafruit_ST7735.h>        // TFT Display library
#include <SPI.h>                    // Serial Peripheral Interface (SPI) Library

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);
Servo myservo; 

void setup() 
  {
   Serial.begin(9600);                // Initialize serial for debug

   myservo.attach(SERVOPIN);          // Define Servo Pin 

   tft.initR(INITR_BLACKTAB);         // Initialize a ST7735S chip, black tab
   tft.fillScreen(ST7735_BLACK);      // Clear screen
   tft.setRotation(1);                // Screen Rotation 0-3
   screenText();                      // Write static text to screen
 
 #ifdef HC-SR04
   pinMode(TRIGPIN, OUTPUT);          // Setup HC-SR04 pin tris if used
   pinMode(ECHOPIN, INPUT);
 #endif
    
   theta = (MAXTHETA-MINTHETA)/2+MINTHETA;        // Find center angle
   thetaInc = (MAXTHETA-MINTHETA)/(DIVISIONS);    // Find angle to increment
    
   myservo.write(theta);                          // Command servo to center angle
   
   Serial.println("Centering Servo");              // Debug info sent to serial monitor
   Serial.print("servo angle= ");
   Serial.println(theta);
   Serial.print("Divisions= ");
   Serial.println(DIVISIONS);
   Serial.print(" Ping Origin= ");
   Serial.print(tft.width());
   Serial.print(" ");
   Serial.print(tft.height());
   
   currentInc = 0;                // Start the scan at the beginning
  }

void loop() 
  {                                               // Servo Positioning
    theta = (thetaInc * currentInc) + MINTHETA;   // Calc the new angle
    myservo.write(theta);                         // Command servo to new angle
    delay(SERVOPAUSE);                            // Pause to allow servo to move to selected angle
 
#ifdef HC-SR04                             // Pinging 
      digitalWrite(TRIGPIN, LOW);          // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
      delayMicroseconds(2);
      digitalWrite(TRIGPIN, HIGH);         // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
      delayMicroseconds(10);
      digitalWrite(TRIGPIN, LOW);
      duration = pulseIn(ECHOPIN, HIGH);           
#else                                      // #elif Parallax
      pinMode(PINGPIN, OUTPUT);            // Turn PINGPIN to output
      digitalWrite(PINGPIN, LOW);          // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
      delayMicroseconds(2);
      digitalWrite(PINGPIN, HIGH);         // PINGPIN high to initiate ultrasonic ping
      delayMicroseconds(5);
      digitalWrite(PINGPIN, LOW);
      pinMode(PINGPIN, INPUT);             // PINGPIN input to measure ultrasonic time of flight (TOF)
      duration = pulseIn(PINGPIN, HIGH);   // Measure TOF      
#endif

    dist = duration/74/2;                          // Convert the ping time into inches. if centemeters use duration/29/2
    r = map(dist,0,MAXRANGE,0,tft.width()/2-2);    // map the range of the ping sensor to the graphic area
    if(r > tft.width()/2-2) r = tft.width()/2-2;   // Set max for r to outer ring
   
    tft.setTextSize(2);                            // Clear Previous Text
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(15, 25);
    tft.print(last_r);
    tft.setCursor(115, 25);
    tft.print(last_theta-MINTHETA);   
   
    last_r = r;                         // New displayed values to be cleared next loop
    last_theta = theta;
   
    tft.setTextSize(2);                 // Print New Text 
    tft.setTextColor(ST7735_GREEN);
    tft.setCursor(15, 25);
    tft.print((int)(r+.5));
    tft.setCursor(115, 25);
    tft.print(theta-MINTHETA);  
    
    x = tft.width()/2 - r*cos((float)theta*pi/180);      // Calculate new ping postion
    y = tft.height() - r*sin((float)theta*pi/180);
    
    lay[currentInc][0]= x;
    lay[currentInc][1]= y;
    tft.fillCircle(x,y,DOTSIZE,ST7735_RED);         // Write new ping dot
    
    Serial.print(" dist= ");                        // Debug to serial monitor
    Serial.print(dist);
    Serial.print(" in ");    
    Serial.print(" r= ");
    Serial.print(r);
    Serial.print(" currentInc= ");
    Serial.print(currentInc);
    Serial.print(" theta= ");
    Serial.print(theta-MINTHETA);
    Serial.print(" TFT Point = ");
    Serial.print(x);
    Serial.print(" ");
    Serial.println(y);

    if(right == false) currentInc++;                // Increment angle in the direction we are rotating
    if(right == true)  currentInc--;
    if(currentInc == 0)          right = false;     // Reverse direction when we reach the extent
    if(currentInc >= DIVISIONS)  right = true;
    
    x = lay[currentInc][0];
    y = lay[currentInc][1];
    tft.fillCircle(x,y,DOTSIZE,ST7735_BLACK);       // Clear old ping location 
  }

//******************************* SubRoutines ******************************************** 
void softwareReset()                    
  {                                      
    asm volatile ("  jmp 0");           // Restarts program from beginning
  }
  
void screenText()
  {
    tft.setTextColor(ST7735_GREEN);     // Printing static Items to speed up the loop void
    tft.setTextSize(2);
    tft.setCursor(50,2);
    tft.println("SONAR");

    tft.drawLine(0, 20, tft.width()-1, 20, ST7735_WHITE);
    
    tft.drawLine(tft.width()/2, tft.height(), tft.width()/2 + ((tft.width()/2)-1)*cos(MINTHETA*pi/180), tft.height() - ((tft.height()/2)-1)*sin(MINTHETA*pi/180), ST7735_GREEN);
    tft.drawLine(tft.width()/2, tft.height(), tft.width()/2 + ((tft.width()/2)-1)*cos(MAXTHETA*pi/180), tft.height() - ((tft.height()/2)-1)*sin(MAXTHETA*pi/180), ST7735_GREEN);

    tft.drawCircle(tft.width()/2, tft.height(), tft.width()/2-1, ST7735_GREEN);   // x0, y0, r, color
    tft.drawCircle(tft.width()/2, tft.height(), tft.width()*3/8-1, ST7735_GREEN); // x0, y0, r, color
    tft.drawCircle(tft.width()/2, tft.height(), tft.width()/4-1, ST7735_GREEN);   // x0, y0, r, color
    tft.drawCircle(tft.width()/2, tft.height(), tft.width()/8-1, ST7735_GREEN);   // x0, y0, r, color
  }
