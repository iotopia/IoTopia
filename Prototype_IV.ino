#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const byte numReadingsECprobe = 20;
long baudRate = 115200;
const char* strBoard = "MegaII_Board_20";
const int pinRelay[] = { 34, 35, 36 };
const int pinDHT[] = { 22, 23 };
const int typeDHT[] = { DHT11, DHT11 };
const int pinTriggerHCSR04[] = { 30, 32 };
const int pinEchoHCSR04[] = { 31, 33 };
int pinStateRelay[] = { HIGH, HIGH, HIGH };
long lastReadDHT = 0;
long lastReadHCSR04 = 0;
long baudRateESP = 115200;
const int pinPerisPump[] = { 42, 43, 44 };
const int pinDallasTempBus[] = { 26 };
long lastReadDallasTempBus = 0;
long lastReadECprobe = 0;
unsigned int readingsECprobe[numReadingsECprobe];  
long lastReadPHprobe = 0;
const int pinECprobe = A5;
byte indexECprobe = 0;
long analogSampleTimeECprobe = 0;
long tempSampleTimeECprobe = 0;
long printTimeECprobe = 0;
long analogValueTotalECprobe = 0;
int analogAverageECprobe = 0;
int averageVoltageECprobe = 0;
const int pinPHprobe = A4;
String strFullTopic = String("1/20/");

DHT* sensorDHT;
OneWire* oneWireDallasTempBus;
DallasTemperature* sensorDallasTempBus;

void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
    publishESP(topic+":"+msg+"\n");
}

String splitVal(String data, char separator, int index){
    int found = 0;
    int strIndex[] = {0, -1};
    for(int i = 0; i <= data.length()-1 && found <= index; i++){
        if(data.charAt(i) == separator || i == data.length()-1){
            found++;
            strIndex[0] = strIndex[1]+1;
            strIndex[1] = (i ==  data.length()-1) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void input(String source, String input){
    output(source+" Input",input);
    int ref = (source == "Serial") ? 0 : 2;
    int id = splitVal(input,'/',ref+1).toInt();
    String component = splitVal(input,'/',ref+0);
    String operation = splitVal(input,'/',ref+2);

    if(component == "relay"){
        (operation == "get") ? readRelays() : setRelay(id,operation.toInt());
    }
    if(component == "perispump"){
        doPerisPump(id,operation.toInt());
    }
}
void readSerial(){
    if (Serial.available() > 0) {
        input("Serial",Serial.readString());
    }
}
void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
}
void initRelays() {
    for(int i = 0; i < (sizeof(pinRelay)/sizeof(int)); i++){
        digitalWrite(pinRelay[i], pinStateRelay[i]);
        pinMode(pinRelay[i], OUTPUT);
    }
    readRelays();
}
void setRelay(int idRelay, int state) {
    digitalWrite(pinRelay[idRelay-1], state);
    readRelays();
}
void initDHTs() {
    sensorDHT = (DHT*)malloc(sizeof(pinDHT)*sizeof(DHT));
    for(int i = 0; i < (sizeof(pinDHT)/sizeof(int)); i++){
        sensorDHT[i] = DHT(pinDHT[i], typeDHT[i]);
        sensorDHT[i].begin();
    }
}
void readDHTs(){
    long interval = 2 * 1000;
    if( millis() - lastReadDHT > interval ) {
        lastReadDHT = millis();
        for(int i = 0; i < (sizeof(pinDHT)/sizeof(int)); i++){
            if(isnan(sensorDHT[i].readHumidity())){
                output("Status","DHT_"+String(i+1)+" Reading Failed");
            }
            else{
                output("DHT_"+String(i+1)+"/Hum/Rel", String(sensorDHT[i].readHumidity(), 0));
                output("DHT_"+String(i+1)+"/Temp/Cel", String(sensorDHT[i].readTemperature(), 0));
                output("DHT_"+String(i+1)+"/HI/Cel", String(sensorDHT[i].computeHeatIndex(sensorDHT[i].readTemperature(), sensorDHT[i].readHumidity(), false), 2));
            }
        }
    }
}
void initHCSR04s(){
    for(int i = 0; i < (sizeof(pinTriggerHCSR04)/sizeof(int)); i++){
        pinMode(pinTriggerHCSR04[i],OUTPUT);
        pinMode(pinEchoHCSR04[i],INPUT);
    }
}
void readHCSR04s(){
    long interval = 1000;
    if( millis() - lastReadHCSR04 > interval ) {
        lastReadHCSR04 = millis();
        for(int i = 0; i < (sizeof(pinTriggerHCSR04)/sizeof(int)); i++){
            digitalWrite(pinTriggerHCSR04[i], LOW);
            delayMicroseconds(5);
            digitalWrite(pinTriggerHCSR04[i], HIGH);
            delayMicroseconds(10);
            digitalWrite(pinTriggerHCSR04[i], LOW);
         
            pinMode(pinEchoHCSR04[i], INPUT);
            float duration = pulseIn(pinEchoHCSR04[i], HIGH);
            output("HCSR04_"+String(i+1)+"/Dist/Cm",String((duration / 2) / 29.1));
        }
    }
}
void initESP(){
    Serial2.begin(baudRateESP);
    publishESP("ESP:"+String(strBoard)+"\n");
}
void publishESP(String msg){
    String pub = strFullTopic+msg;
    char msgCharArray[pub.length() + 1];
    pub.toCharArray(msgCharArray, pub.length() + 1);
    Serial2.write(msgCharArray);
}
void readESP(){
    if (Serial2.available() > 0) {
        input("Serial",Serial2.readString());
    }
}
void initDallasTemperatureMultiBus(){
    oneWireDallasTempBus = (OneWire*)malloc(sizeof(pinDallasTempBus)*sizeof(OneWire));
    sensorDallasTempBus = (DallasTemperature*)malloc(sizeof(pinDallasTempBus)*sizeof(DallasTemperature));
    DeviceAddress deviceAddress;
    for (int i=0; i < (sizeof(pinDallasTempBus)/sizeof(int)); i++) {
        oneWireDallasTempBus[i] = OneWire(pinDallasTempBus[i]);
        sensorDallasTempBus[i] = DallasTemperature(&oneWireDallasTempBus[i]);
        sensorDallasTempBus[i].begin();
    if (sensorDallasTempBus[i].getAddress(deviceAddress, 0)) sensorDallasTempBus[i].setResolution(deviceAddress, 12);
  }
}
void readDallasTemperatureMultiBus(){
    long interval = 1 * 1000;
    if( millis() - lastReadDallasTempBus > interval ) {
        lastReadDallasTempBus = millis();
        for (int i=0; i < (sizeof(pinDallasTempBus)/sizeof(int)); i++) {
            sensorDallasTempBus[i].requestTemperatures();
            output("DallasBus_"+String(i+1)+"/Temp/Cel", String(sensorDallasTempBus[i].getTempCByIndex(0), 2));
        }
    }
}
void readECprobe(){
    float ECcurrent = 0;
    float temperatureECsolution = 0;

    const int AnalogSampleInterval=25;
    const int printInterval = 700;
    const int tempSampleInterval = 850;
     
     if(millis()-analogSampleTimeECprobe>=AnalogSampleInterval)  
  {
    analogSampleTimeECprobe=millis();
     // subtract the last reading:
    analogValueTotalECprobe = analogValueTotalECprobe - readingsECprobe[indexECprobe];
    // read from the sensor:
    readingsECprobe[indexECprobe] = analogRead(pinECprobe);
    // add the reading to the total:
    analogValueTotalECprobe = analogValueTotalECprobe + readingsECprobe[indexECprobe];
    // advance to the next position in the array:
    indexECprobe = indexECprobe + 1;
    // if we're at the end of the array...
    if (indexECprobe >= numReadingsECprobe)
    // ...wrap around to the beginning:
    indexECprobe = 0;
    // calculate the average:
    analogAverageECprobe = analogValueTotalECprobe / numReadingsECprobe;
  }
  /*
   Every once in a while,MCU read the temperature from the DS18B20 and then let the DS18B20 start the convert.
   Attention:The interval between start the convert and read the temperature should be greater than 750 millisecond,or the temperature is not accurate!
  */
   if(millis() - tempSampleTimeECprobe >= tempSampleInterval) 
  {
    tempSampleTimeECprobe=millis();
    //temperature = TempProcess(ReadTemperature);  // read the current temperature from the  DS18B20
    //TempProcess(StartConvert);  
    sensorDallasTempBus[0].requestTemperatures();//after the reading,start the convert for next reading
    temperatureECsolution = sensorDallasTempBus[0].getTempCByIndex(0);
  }
   /*
   Every once in a while,print the information on the serial monitor.
  */
  if(millis() - printTimeECprobe >= printInterval)
  {
    printTimeECprobe = millis();
    averageVoltageECprobe = analogAverageECprobe * (float)5000/1024;
    //output("ECprobe/AnalogAverage/Value", String(analogAverageECprobe));//analog average,from 0 to 1023
    //output("ECprobe/Voltage/mV", String(averageVoltageECprobe));//millivolt average,from 0mv to 4995mV
    //output("ECprobe/Temperature/ºC", String(temperatureECsolution, 2));//current temperature
    
    float TempCoefficient = 1.0+0.0185*(temperatureECsolution-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.0185*(fTP-25.0));
    float CoefficientVolatge = (float)averageVoltageECprobe/TempCoefficient;   
    if(CoefficientVolatge<20){}//output("ECprobe/Status", "No solution");   //25^C 1413us/cm<-->about 216mv  if the voltage(compensate)<150,that is <1ms/cm,out of the range
    else if(CoefficientVolatge>3300){}//output("ECprobe/Status", "Out of the range!");  //>20ms/cm,out of the range
    else
    { 
      if(CoefficientVolatge<=448)ECcurrent = 6.84*CoefficientVolatge-64.32;   //1ms/cm
      else if(CoefficientVolatge<=1457)ECcurrent = 6.98*CoefficientVolatge-127;  //3ms/cm
      else ECcurrent = 5.3*CoefficientVolatge+2278;                           //10ms/cm<EC<20ms/cm
      ECcurrent/=1000;    //convert us/cm to ms/cm
      //Serial.print(ECcurrent,2);  //two decimal
      //Serial.println("ms/cm");
      output("ECprobe/EC/EC", String(ECcurrent, 2));
    }
  }
}
void initECprobe(){
    int readingsECprobe[numReadingsECprobe];
    for (byte thisReading = 0; thisReading < numReadingsECprobe; thisReading++){
        readingsECprobe[thisReading] = 0;
    }
}
void readPHprobe(){
    unsigned long int avgValue;  //Store the average value of the sensor feedback
    int buf[10],temp;
    int interval = 2 * 1000;
    if( millis() - lastReadPHprobe > interval ) {
        lastReadPHprobe = millis();
        for(int i=0;i<10;i++){
            //Get 10 sample value from the sensor for smooth the value
            buf[i]=analogRead(pinPHprobe);
            delayMicroseconds(10);
        }
        for(int i=0;i<9;i++){//sort the analog from small to large
            for(int j=i+1;j<10;j++){
                if(buf[i]>buf[j]){
                    temp=buf[i];
                    buf[i]=buf[j];
                    buf[j]=temp;
                }
            }
        }
        avgValue = 0;
        for(int i = 2; i < 8; i++){                      //take the average value of 6 center sample
            avgValue += buf[i];
        }
        float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
        phValue=3.5*phValue;            //convert the millivolt into pH value
        output("pHprobe/pH/Value", String(phValue, 2));
    }
}
void readRelays(){
    for(int i = 0; i < (sizeof(pinRelay)/sizeof(int)); i++){
        output("Relay_"+String(i+1)+"/Status",String(digitalRead(pinRelay[i])));
    }
}
void doPerisPump( unsigned int idPump, unsigned long ml){
    long previousMillis = millis();
    long currentMillis = previousMillis;
    long multiplier = 1000; //value to change based on calibration
    //multiplier = milliseconds needed to fill 1ml liquid
    /*
    if(idPump == 2 || idPump == 3){
        multiplier = 500;
        ml = ml*2;
    } 
    */
    unsigned long interval = ml *multiplier;
    long currentValue = 0;
    digitalWrite(pinPerisPump[idPump-1],1);
    while(currentMillis - previousMillis < interval){
        currentMillis = millis();
        
        if((currentMillis - previousMillis)/1000 != currentValue){
            currentValue = ((currentMillis - previousMillis)/1000);
            output("PerisPump_"+String(idPump)+"/Dose/Ml",String(currentValue));
        }
    }
    output("PerisPump_"+String(idPump)+"/Status","Finished dosing "+String(currentValue)+" Ml");
    output("PerisPump_"+String(idPump)+"/Dose/Ml",String(0));
    digitalWrite(pinPerisPump[idPump-1],0);
}
void initPerisPumps() {
    for(int i = 0; i < (sizeof(pinPerisPump)/sizeof(int)); i++){
        pinMode(pinPerisPump[i], OUTPUT);
    }
}

void setup() {
    initSerial();
    initRelays();
    initDHTs();
    initHCSR04s();
    initESP();
    initDallasTemperatureMultiBus();
    initECprobe();
    initPerisPumps();
}

void loop() {
    readDHTs();
    readHCSR04s();
    readESP();
    readDallasTemperatureMultiBus();
    readECprobe();
    readPHprobe();
    readSerial();
}
