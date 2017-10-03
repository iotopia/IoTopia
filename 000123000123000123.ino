#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <PubSubClient.h>

long baudRate = 115200;
byte mac[]  = {0xDE,0xED,0xBA,0xFE,0xFE,0xED};
const char* strBoard = "UnoI_Board_4";
const int pinDHT[] = { 4 };
const int typeDHT[] = { DHT11 };
long lastReadDHT = 0;
const int portMQTT_1 = 1884;
String strFullTopic = String("1/4/");
const char* brokerMQTT_1 = "iotopia.io";

IPAddress ip(192,168,1,200);
IPAddress gateway(192,168,1,254);
IPAddress dnServer(192,168,1,254);
IPAddress subnet(255,255,255,0);
DHT* sensorDHT;
EthernetClient clientEthMQTT_1;
PubSubClient clientMQTT_1(clientEthMQTT_1);

String displayAddress(IPAddress address){
    return String(address[0]) + "." +
           String(address[1]) + "." +
           String(address[2]) + "." +
           String(address[3]);
}

char* toChar(String sourceString){
    char stringCharArray[sourceString.length() + 1];
    sourceString.toCharArray(stringCharArray, sourceString.length() + 1);
    return stringCharArray;
}
void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    
    String strPayload = String((char*)payload);
    String strTopic = String((char*)topic);
    output("MQTT","In | Topic : "+strTopic+" | "+strPayload);
    input("MQTT",strTopic+"/"+strPayload);
    
    //output("MQTT","In | Topic : "+String((char*)topic)+" | "+String((char*)payload));
    //input("MQTT",String((char*)topic)+"/"+String((char*)payload));
}
void output(String topic, String msg){
    Serial.println(topic+" | "+msg);
    publish(topic,msg);
}

void publish(String topic, String msg){
    if (clientMQTT_1.connect(strBoard)) {
        String outTopic = strFullTopic+topic;
        char outTopicCharArray[outTopic.length() + 1];
        outTopic.toCharArray(outTopicCharArray, outTopic.length() + 1);
        char outMsgCharArray[msg.length() + 1];
        msg.toCharArray(outMsgCharArray, msg.length() + 1);
        clientMQTT_1.publish(outTopicCharArray,outMsgCharArray);
        //clientMQTT_1.publish(toChar(strFullTopic+topic),toChar(msg));
    }
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
void initEthernet(){
    if (Ethernet.begin(mac) == 0) {
        output("Status/network","Failed to configure Ethernet using DHCP");
        Ethernet.begin(mac, ip, dnServer, gateway, subnet);
    }
    delay(1500);
    output("Status/Network",String(strBoard)+" Ethernet IP Address - "+displayAddress(Ethernet.localIP()));
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
void initClientMQTT_1(){
    String topicSubMQTT[] = {};
    clientMQTT_1.setServer(brokerMQTT_1, portMQTT_1);
    clientMQTT_1.setCallback(callback);
    if (clientMQTT_1.connect(strBoard)) {
        output("Status/MQTT"," Client MQTT_1 is now online & connected - LAN IP Address - "+displayAddress(Ethernet.localIP()));
        for(int i = 0; i < (sizeof(topicSubMQTT)/sizeof(String)); i++){
            String inTopic = strFullTopic+topicSubMQTT[i];
            char topicCharArray[inTopic.length() + 1];
            inTopic.toCharArray(topicCharArray, inTopic.length() + 1);
            clientMQTT_1.subscribe(topicCharArray);
            output("Status/MQTT_1","Subscribed "+String(topicCharArray));
            /*
            clientMQTT_1.subscribe(toChar(strFullTopic+topicSubMQTT[i]));
            output("Status/MQTT_1","Subscribed "+strFullTopic+topicSubMQTT[i]);
            */
        }
    }
}

void setup() {
    initSerial();
    initEthernet();
    initDHTs();
    initClientMQTT_1();
}

void loop() {
    readDHTs();
    clientMQTT_1.loop();
    readSerial();
}
