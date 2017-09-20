/********************
  Relays
*********************/
#define RELAY1  22                        
#define RELAY2  24     

void initRelays(){
  pinMode(RELAY1, OUTPUT);       
  pinMode(RELAY2, OUTPUT);
}

void onRelay1(){
   digitalWrite(RELAY1,LOW);           // Turns ON Relays 1
}
void onRelay2(){
   digitalWrite(RELAY2,LOW);           // Turns ON Relays 1
}
void offRelay1(){                                    
   digitalWrite(RELAY1,HIGH);          // Turns Relay Off
}
void offRelay2(){                                    
   digitalWrite(RELAY2,HIGH);          // Turns Relay Off
}
/********************
  DHT 11
*********************/

//DHT Library
#include <dht.h>
#define dhtPin A1     // what pin we're connected to 
dht DHT;
int dhtHum = 0;
int dhtTemp = 0;

/********************
  LCD KeyPad
*********************/

//using LiquidCrystal library
#include <LiquidCrystal.h>
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// define some values used by the panel and buttons
#define LCD_BACKLIGHT_PIN 10
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
//define lcd text messages
//char text[];
//char airTemp[ ] = "Temperature";
//char airHum[ ] = "Humidity";
//define vars
int lcdOn = 0;
int menuPos = 2;
//int menuIndex[] = {0,1};

/********************
  Soil Moisture
*********************/

//Define the moisture sensor pins
const int moistPin1 = A2;  
//the value readed from each moisture sensor
int moistVal1 = 0;
//the sum of the 30 times sampling
long int moistSum1 = 0;   //we need to sampling the moisture 30 times and get its average value

/********************
  LCD KeyPad
*********************/

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 return btnNONE;  // when all others fail, return this...
}

void toggleLight(){
  if(lcdOn == 1){
     lcd.noDisplay();
     // Turn off the LCD backlight.
      digitalWrite(LCD_BACKLIGHT_PIN, LOW);
     lcdOn = 0;
   }
   else{
     lcd.display();
     // Turn on the LCD backlight.
      digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
     lcdOn = 1;       
   }
}


void initMenu(){
  int menuPos[] = {2, 4, 8, 3, 6};
}

void initLcd(){
  // Set the LCD display backlight pin as an output.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
   // Turn on the LCD backlight.
  digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
  lcd.begin(16, 2);              // start the library
  resetLcd();
}
void resetLcd(){
  lcd.clear();
  lcd.setCursor(0,0);
}
/*
void lcdShow(){
  resetLcd();
  lcd.print();
  delay(300);
  lcd.noDisplay();
  delay(300);
  lcd.display();
  delay(300);
  lcd.noDisplay();
  delay(300);
  lcd.display();
  delay(300);
}
*/
void nextView(){
  /*int i;
  for (i = 0; i < 2; i ++) {
    
  }*/
  resetLcd();
  if(menuPos <= 2)
    menuPos ++;
  else
    menuPos = 0;
    
  switch (menuPos)               
 {
   case 0:
     {
       resetLcd();
       lcd.print("IoG v1.0"); // print a simple message     
     break;
     }
   case 1:
     {
       readDht11();
     break;
     }
   case 2:
     {
       readSoilMoisture();   
     break;
     }
   case 3:
     {
       readWaterLevel();
     break;
     }
 /*  case 4:
     {
      
     break;
     }*/
   }
}

void openMenu(){
  resetLcd();
    
  switch (menuPos)               
 {
   case 0:
     {
       lcd.print("IoG v1.0"); // print a simple message     
     break;
     }
   case 1:
     {
       readDht11();
     break;
     }
   case 2:
     {
       readSoilMoisture();   
     break;
     }
   case 3:
     {
       readWaterLevel();
     break;
     }
 /*  case 4:
     {
      
     break;
     }*/
   }
}

void timeCount(){
 lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
 lcd.print(millis()/1000);      // display seconds elapsed since power-up

 //lcd.setCursor(0,1);            // move to the begining of the second line
}


 
/********************
  DHT 11
*********************/
void readDht11(){
  
  DHT.read11(dhtPin);
  dhtHum = DHT.humidity;
  dhtTemp = DHT.temperature;
  
  Serial.print("Temperature : "); 
  Serial.print(dhtTemp); 
  Serial.print(" - Humidity : ");
  Serial.println(dhtHum);
  
  if(menuPos == 1){
        lcd.setCursor(0,0);            // move to the begining of the second line
        lcd.print("Air Temp: ");
        lcd.print(dhtTemp);
        lcd.print(" C");
        lcd.setCursor(0,1);
        lcd.print("Air Hum: ");
        lcd.print(dhtHum);
        lcd.print(" %");
  }
  
  //delay(2000);//Wait 2 seconds before accessing sensor again.
  
}
/********************
  Soil Moisture
*********************/
void readSoilMoisture(){
  int i;
  for(i = 0; i < 30; i++){//sampling 30 time within 3 seconds
    moistSum1 = moistSum1 + analogRead(moistPin1);  
    delay(100);
  }
  moistVal1 = moistSum1 / 30;//get the average value
 // for(int i = 0; i < 3; i ++){
    Serial.print("Soil "); 
    Serial.print("1"); 
    Serial.print(" : ");
    Serial.print(moistVal1);
    Serial.println(" % ");
  //}
  
  if(menuPos == 2){
        resetLcd();            // move to the begining of the second line
        lcd.print("Soil 1 : ");
        lcd.print(moistVal1);
        lcd.print(" %");
  }
  
  if(moistVal1 > 450){
    onRelay1();
  }
  else{
    offRelay1();
  }
  
  moistSum1 = 0;
}

/********************
  Serial
*********************/
void initSerial(){
  Serial.begin(9600);
}
/********************
  Water Level
*********************/
const int waterLevelPin= A3; //sensor pin connected to analog pin A3
int waterLevelVal;

void initWaterLevel(){
  pinMode(waterLevelPin, INPUT);
}

void readWaterLevel(){
  waterLevelVal = analogRead(waterLevelPin); //arduino reads the value from the liquid level sensor
  resetLcd();
  if(menuPos == 3){
    lcd.print("Water Level");
    lcd.setCursor(0,1); 
    lcd.print(waterLevelVal);
  }
  Serial.print("Water Level : ");
  Serial.println(waterLevelVal);//prints out liquid level sensor reading
  
  if(waterLevelVal < 500){
    onRelay2();
  }
  else{
    offRelay2();
  }
 // delay(100);//delays 100ms
}
/********************
  Main
*********************/
void setup(){
  initLcd();
  initSerial();
  initWaterLevel();
  initRelays();
  
  //delay(500);//Delay to let system boot
  
  lcd.print("System OK...");
  delay(1000);//Wait before accessing Sensor
  
}

void loop()
{ 
  delay(2000); 
  readDht11(); // read DHT11
  readSoilMoisture();
  readWaterLevel();
    
  lcd_key = read_LCD_buttons();  // read the buttons
  
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
    //   resetLcd();
//       lcd.setCursor(0,1);            // move to the begining of the second line
/*       lcd.print("Temp: ");
       lcd.print(dhtTemp);
       lcd.print(" C");
       lcd.setCursor(0,1);
       lcd.print("Hum: ");
       lcd.print(dhtHum);
       lcd.print(" %");
       menuPos = 1;*/
       menuPos = 2;
       openMenu();
     break;
     }
   case btnLEFT:
     {
       menuPos = 1;
       openMenu();
     break;
     }
   case btnUP:
     { 
       
  /*     resetLcd();
       lcd.print(menuPos);
       
       if(menuPos < 2){
         menuPos ++;
       }
       else
         menuPos = 0;
       */
      // nextView();
   //   if(menuPos < 3)
//        menuPos ++;
   //   else if(menuPos == 3)
   //     menuPos = 0;
    //   openMenu();
       menuPos = 0;
       openMenu();
     break;
     }
   case btnDOWN:
     {
  //     if(menuPos > 0)
  //      menuPos --;
 //     else if(menuPos == 0)
 //       menuPos = 3;
       //menuPos = 0;
       //lcd.setCursor(0,0);   
       //lcd.print("IoG v1.0"); // print a simple message     
     //lcd.print("DOWN  ");
     menuPos = 3;
     openMenu();
     break;
     }
   case btnSELECT:
     {
       toggleLight();
     break;
     }
   case btnNONE:
     {
     //lcd.print("NONE  ");
     break;
     }
 }

}
