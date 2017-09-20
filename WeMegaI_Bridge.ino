#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

String strFullTopic = String("1/87/");
const char* strBoard = "WeMegaI-WiFi_Board_87";
const char* ssid = "WiChezNous";
const char* password = "WiChezPass0.";
//const char* mqtt_server = "iotopia.io";
const char* brokerMQTT = "iotopia.io";
long baudRate = 115200;

WiFiClient clientESP;
PubSubClient client(clientESP);
String readString;

String displayAddress(IPAddress address){
    return String(address[0]) + "." +
           String(address[1]) + "." +
           String(address[2]) + "." +
           String(address[3]);
}

void output(String topic, String msg){
    Serial.println(msg);
   
    if(topic == "MQTT"){
        input(topic,msg);
    }
    else{
        if(client.connected()) publish(topic,msg);
    }
}

void input(String topic, String msg){
    char msgCharArray[msg.length() + 1];
    msg.toCharArray(msgCharArray, msg.length() + 1);
    Serial.write(msgCharArray);
}

void initSerial(){
    Serial.begin(baudRate);
    output("Board",String(strBoard));
}
long lastBlink = 0;
void blink(){
    const int blinkInterval = 1000;
    if(millis() - lastBlink >= blinkInterval) {
        if(digitalRead(BUILTIN_LED) == LOW){
            digitalWrite(BUILTIN_LED, HIGH);
        }
        else {
            digitalWrite(BUILTIN_LED, LOW);  // Turn the LED off by making the voltage HIGH
        }
    }
}

void initWiFi() {
    delay(10);
    WiFi.begin(ssid, password);
    WiFi.mode(WIFI_STA);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
     digitalWrite(BUILTIN_LED, LOW); 
    output("Status/Network",String(strBoard)+" Wi-Fi IP Address - "+displayAddress(WiFi.localIP()));
   
}

void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    String strPayload = String((char*)payload);
    String strTopic = String((char*)topic);
    
  /*  if ((char)payload[0] == '1') {
        digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is acive low on the ESP-01)
    } 
    else {
        digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }*/
    Serial.write((char*)payload);
    //output("MQTT",strPayload);
    //input("MQTT",strTopic+"/"+strPayload);

}

void initClientESP() {
    client.setServer(brokerMQTT, 1884);
    client.setCallback(callback);
    // Loop until we're reconnected
    while (!client.connected()) {
        output("Status/Network","Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(strBoard)) {
            output("Status/Network","Connected to iotopia.io");
            output("Status/MQTT"," Client ESP is now online & connected to iotopia.io - LAN IP Address - "+displayAddress(WiFi.localIP()));
        
          // Once connected, publish an announcement...
            publish("Board", strBoard);
            subscribe();
          // ... and resubscribe
          //client.subscribe("inTopic");
        } 
        else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void publish(String topic, String msg){
   // if (client.connect(strBoard)) {
     digitalWrite(BUILTIN_LED, HIGH);
        String outTopic = strFullTopic+topic;
        char outTopicCharArray[outTopic.length() + 1];
        outTopic.toCharArray(outTopicCharArray, outTopic.length() + 1);
        char outMsgCharArray[msg.length() + 1];
        msg.toCharArray(outMsgCharArray, msg.length() + 1);
        client.publish(outTopicCharArray,outMsgCharArray);
        digitalWrite(BUILTIN_LED, LOW);
        //clientMQTT_1.publish(toChar(strFullTopic+topic),toChar(msg));
    //}
}

void subscribe(){
    String strTopic = strFullTopic+"bridge";
    int topicLen = strTopic.length() + 1;
    char topicCharArray[topicLen];
    strTopic.toCharArray(topicCharArray, topicLen);
    client.subscribe(topicCharArray);
}

void readSerial(){
     while (Serial.available()) {
        delay(1);
        if (Serial.available() > 0) {
            
           // Serial.find("\n");
           String s = Serial.readStringUntil('\n');
           publish("bridgeESP",String(s.c_str()));
       }
        
    }
}

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    digitalWrite(BUILTIN_LED, LOW);
    initSerial();
    initClientESP();
}


void loop() {
    if (!client.connected()) {
        initClientESP();
    }
    else{
        //digitalWrite(BUILTIN_LED, LOW);
        client.loop();
        blink();
        //digitalWrite(BUILTIN_LED, HIGH);
    }
    readSerial();
    delay(500);
}


