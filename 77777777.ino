#include <DHT.h>

long baudRate = 115200;
const char* strBoard = "UnoI_Board_4";
const int pinDHT[] = { 4 };
const int typeDHT[] = { DHT11 };
long lastReadDHT = 0;

DHT* sensorDHT;

void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
}

void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
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
                output("DHT_"+String(i+1)+"/Temp/Fah", String(sensorDHT[i].readTemperature(true), 2));
                output("DHT_"+String(i+1)+"/HI/Fah", String(sensorDHT[i].computeHeatIndex(sensorDHT[i].readTemperature(true), sensorDHT[i].readHumidity()), 2));    
            }
        }
    }
}

void setup() {
    initSerial();
    initDHTs();
}

void loop() {
    readDHTs();
}
