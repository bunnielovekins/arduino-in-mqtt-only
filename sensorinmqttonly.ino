#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte mac[] = { 0x86, 0x8D, 0xBE, 0x8F, 0xFE, 0xED };
//char server[] = "ec2-54-213-70-96.us-west-2.compute.amazonaws.com";
char server[] = "54.213.172.75";
int myId = -1;
char myName[] = "PetersDesk";
int sensorPin = A0;
int sensorValue = 0;
int lastValue = 0;
char messageBuffer[10];
char meta[] = "sens/meta";

EthernetClient client;
PubSubClient mqclient(server, 1883, callback, client);

char myTopic[] = "sens/ ";

void setup() {
  Serial.begin(9600);
  Serial.println(".");
  if(Ethernet.begin(mac)==0)
    Serial.println("Failed to configure Ethernet with DHCP");
  else 
    Serial.println("Phew");
  delay(1000);
  
  while(!mqclient.connect("Ardyin")){
    Serial.println("Connection failed, trying again in 1s");
    delay(1000);
  }
  
  mqclient.subscribe(meta);
  mqclient.publish(meta,"add");
  //topic[5] = (char)(myId+48);
  //Serial.println(topic);
  

  pinMode(9,OUTPUT);
  //digitalWrite(9,HIGH);
}



void loop()
{
  if(myId == -1){
    mqclient.loop();
  }
  else{
    //Serial.print(',');
    lastValue = sensorValue;
    sensorValue = analogRead(sensorPin);
    int diff = lastValue-sensorValue;
    if(diff>2 || diff<-2){
      if(!mqclient.connected()){
        while(!mqclient.connect("Ardy")){
          Serial.println("Connection failed, trying again in 1s");
          delay(1000);
        }
      }
      intToStr(sensorValue,messageBuffer);
      mqclient.publish(myTopic,messageBuffer);
      Serial.println(messageBuffer);
    }
  }
  delay(50);
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(myId==-1){
    Serial.print("Meta reply: ");
    Serial.println((char*)payload);
    if(payload[0]>='0' && payload[0]<='9'){
      myTopic[5] = payload[0];
      myId=(int)(payload[0]-48);
    }
    Serial.print("Id:");
    Serial.println(myId);
    Serial.print("Topic:");
    Serial.println(myTopic);
    digitalWrite(9,HIGH);
  }
}

char *intToStr(int num, char *buffer){
  String str = "";
  str+=num;
  str.toCharArray(buffer,10);
  return buffer;
}
