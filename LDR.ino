const int voltageLDR[] = { 5 };
const int resistorLDR[] = { 10 };
unsigned long lastReadLDR = 0; 123
const int pinLDR[] = { A4 };
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
void readLDRs(){
    for(unsigned int i = 0; i < (sizeof(pinLDR)/sizeof(int)); i++){
        int val = averageAnalogRead(pinLDR[i]);
        float volt = val*((float)voltageLDR[i]/1024);
        int lux = 500/((float)resistorLDR[i]*(((float)voltageLDR[i]-volt)/volt));
        int resistence = (float)(1023-val)*resistorLDR[i]/val;
        
        output("LDR_"+String(i+1)+"/Light/Raw",String(val));
        output("LDR_"+String(i+1)+"/Light/Volt",String(volt));
        output("LDR_"+String(i+1)+"/Light/Lux",String(lux));
        output("LDR_"+String(i+1)+"/Light/Res",String(resistence));
    }
}
void loopLDRs(){
    const int interval = 500;
    if( millis() - lastReadLDR > interval ) {
        lastReadLDR = millis();
        readLDRs();
    }
}

void setup() {
    initSerial();
}

void loop() {
    loopLDRs();
}
