#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <AutoPID.h>
#include <max6675.h>

#define wifi_ssid ""
#define wifi_password ""

#define mqtt_server ""
#define mqtt_user ""
#define mqtt_password ""
#define OUTPUT_PIN D3
#define TEMP_READ_DELAY 200

#define temperature_topic "sensor/temperature"
#define output_topic "sensor/gaggia_output"

WiFiClient espClient;
PubSubClient client(espClient);

     
#define OUTPUT_MIN 0
#define OUTPUT_MAX 1023
#define KP 75  
#define KI 0.25
#define KD 2.5

double temperature, setPoint, outputVal;
AutoPID gaggiaPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
unsigned long lastTempUpdate;

int ktcSO = 12;
int ktcCS = 13;
int ktcCLK = 14;

bool integralReset = false;
 
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = ktc.readCelsius();
    lastTempUpdate = millis();
    Serial.println(temperature);
    return true;
  }
  return false;
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  temperature = ktc.readCelsius();
  setPoint=90 ;
    
}


 
long lastMsg = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now=millis();
  updateTemperature();
  gaggiaPID.run();
    if (now - lastMsg > 1000) {
    lastMsg = now;

      temp = temperature;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      Serial.print("New output:");
      Serial.println(String(outputVal).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
      client.publish(output_topic, String(outputVal).c_str(), true);
     
  
  }
  
  analogWrite(OUTPUT_PIN, outputVal);

//Reset integral term when machine is first heating up or cooling down from steam mode to avoid ridiculous overshoot
  if (gaggiaPID.atSetPoint(15) && integralReset==false){
    integralReset=true;    
    gaggiaPID.reset();
  }

  if(!gaggiaPID.atSetPoint(30) && integralReset==true){
    integralReset=false;
  }

 
}
