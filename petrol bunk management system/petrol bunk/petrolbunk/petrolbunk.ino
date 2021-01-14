
#include <LiquidCrystal.h>
LiquidCrystal lcd(16, 5, 4, 0, 2, 14);
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SoftwareSerial.h>
//SoftwareSerial rfid(15,13);
#define trigger 12
#define echo 13
#define pump 15
#define val 10
//#define sol 10
float time1 = 0, distance = 0;
const char* ssid = "Brinda";
const char* password = "bunna123";
const char* mqtt_server = "broker.mqttdashboard.com";
char a[20];
WiFiClient espClient;
PubSubClient client(espClient);
void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(trigger, OUTPUT);
  pinMode(pump, OUTPUT);

  pinMode(val, OUTPUT);
  pinMode(echo, INPUT);

  lcd.print(" Ultra sonic");
  lcd.setCursor(0, 1);
  lcd.print("Distance Meter");
  delay(2000);
  lcd.clear();
  //  rfid.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);///connecting router

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    a[i] = ((char)payload[i]);
  }
  a[length] = '\0';
  Serial.println(a[0]);
  lcd.clear();
  if (a[0] == 'a')
  {
    //lcd.clear();
    lcd.print("pump on");
    Serial.println("pump on");
    digitalWrite(pump, HIGH);
  }
  else if (a[0] == 'b')
  { //lcd.clear();
    lcd.print("pump off");
    Serial.println("pump off");
    digitalWrite(pump, LOW);
  }
  else if (a[0] == 'c')
  {
    //lcd.clear();
    lcd.print("val on:");
    Serial.println("val on");
    digitalWrite(val, HIGH);
  }
  else if (a[0] == 'd')
  {
    // lcd.clear();
    lcd.print("val off");
    Serial.println("val off");
    analogWrite(val, LOW);
  }


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("slekin/pump");
      // ... and resubscribe

    } else {
      Serial.print("failed, rc=");
      //Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
char dist[32];
void loop()
{
  lcd.clear();
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  time1 = pulseIn(echo, HIGH);
  distance = time1 * 340 / 20000;
  //int h=100;
  //distance=h-distance;
  lcd.clear();
  lcd.print("Distance:");
  lcd.print(distance);
  lcd.print("cm");
  String dis = String(distance);
  dis = "{\"loc\":1,\"val\":\"" + String(dis) + "\"}";
  dis.toCharArray(dist, 100);
  client.publish("slekin/distance", dist);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Flame()
  if (Serial.available() > 0)
  {
    String card = Serial.readStringUntil('\n');
    char card1[100];

    card.toCharArray(card1, 32);
    client.publish("slekin/petrol", card1);
    Serial.println(card1);
    lcd.setCursor(0, 1);
    lcd.print(card1);
  }


  delay(1000);
}

