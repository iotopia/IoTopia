const int voltageML8511[] = { 5 };
unsigned long lastReadML8511 = 0;
const int pinML8511[] = { A5 };
const long baudRate = 115200;
const char* strBoard = "UnoI_Board_4";

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
}

void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
}
void initML8511s(){
    for(unsigned int i = 0; i < (sizeof(pinML8511)/sizeof(int)); i++){
        pinMode(pinML8511[i], INPUT);
        //pinMode(Ref_3v3, INPUT);
    }
}
void loopML8511s(){
    const int interval = 500;
    if( millis() - lastReadML8511 > interval ) {
        lastReadML8511 = millis();
        readML8511s();
    }
}
void readML8511s(){
    for(unsigned int i = 0; i < (sizeof(pinML8511)/sizeof(int)); i++){
        int val = averageAnalogRead(pinML8511[i]);
        //int refLevel = averageAnalogRead(Ref_3v3);
        //float volt = 3.3 / refLevel * val;
        float volt = (float)voltageML8511[i] * val/1024;
        float intensity = mapfloat(volt, 0.99, 2.9, 0.0, 15.0);
        
        output("ML8511_"+String(i+1)+"/UV/Raw",String(val));
        output("ML8511_"+String(i+1)+"/UV/Volt",String(volt));
        output("ML8511_"+String(i+1)+"/UV/UVI",String(intensity));
    }
}

void setup() {
    initSerial();
    initML8511s();
}

void loop() {
    loopML8511s();
}
