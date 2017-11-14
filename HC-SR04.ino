const long baudRate = 115200;
const char* strBoard = "UnoI_Board_4";
const int pinTriggerHCSR04[] = { 5 };
const int pinEchoHCSR04[] = { 6 };
unsigned long lastReadHCSR04 = 0;

void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
}

void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
}
void initHCSR04s(){
    for(unsigned int i = 0; i < (sizeof(pinTriggerHCSR04)/sizeof(int)); i++){
        pinMode(pinTriggerHCSR04[i],OUTPUT);
        pinMode(pinEchoHCSR04[i],INPUT);
    }
}
void readHCSR04s(){
    const int interval = 1000;
    if( millis() - lastReadHCSR04 > interval ) {
        lastReadHCSR04 = millis();
        for(unsigned int i = 0; i < (sizeof(pinTriggerHCSR04)/sizeof(int)); i++){
            digitalWrite(pinTriggerHCSR04[i], LOW);
            delayMicroseconds(5);
            digitalWrite(pinTriggerHCSR04[i], HIGH);
            delayMicroseconds(10);
            digitalWrite(pinTriggerHCSR04[i], LOW);
         
            pinMode(pinEchoHCSR04[i], INPUT);
            float duration = pulseIn(pinEchoHCSR04[i], HIGH);
            output("HCSR04_"+String(i+1)+"/Dist/Cm",String((duration / 2) / 29.1));
            output("HCSR04_"+String(i+1)+"/Dist/Inch",String((duration / 2) / 74));
        }
    }
}

void setup() {
    initSerial();
    initHCSR04s();
}

void loop() {
    readHCSR04s();
}
