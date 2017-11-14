unsigned long lastReadLM35 = 0;
const int pinLM35[] = { A5 };
const long baudRate = 115200;
const char* strBoard = "UnoI_Board_4";

int averageAnalogRead(int pinToRead){
    const byte numberOfReadings = 10;
    unsigned int runningValue = 0; 
    for(int x = 0 ; x <= numberOfReadings ; x++){
        runningValue += analogRead(pinToRead);
    }
    runningValue /= numberOfReadings;
    return(runningValue);  
}
void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
}

void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
}
void loopLM35s(){
    const int interval = 1000; //milliseconds
    if( millis() - lastReadLM35 > interval ) {
        lastReadLM35 = millis();
        readLM35s();
    }
}
void readLM35s(){
    for(unsigned int i = 0; i < (sizeof(pinLM35)/sizeof(int)); i++){
        int val = averageAnalogRead(pinLM35[i]);
        float cel = val / 9.31;
        float fah = cel * 9/5 + 32;
        output("LM35_"+String(i+1)+"/Temp/Raw",String(val));
        output("LM35_"+String(i+1)+"/Temp/Cel",String(cel));
        output("LM35_"+String(i+1)+"/Temp/Fah",String(fah));
    }
}

void setup() {
    initSerial();
}

void loop() {
    loopLM35s();
}
