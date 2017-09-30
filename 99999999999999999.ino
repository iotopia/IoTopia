long baudRate = 115200;1234564879879879
const char* strBoard = "UnoI_Board_4";

void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
}

void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
}

void setup() {
    initSerial();
}

void loop() {
}
