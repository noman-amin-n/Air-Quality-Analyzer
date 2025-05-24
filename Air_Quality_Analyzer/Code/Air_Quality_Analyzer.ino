#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLhuml4X2a"
#define BLYNK_DEVICE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "a7Q-8_ZelDvFGI1v3oegdAhBCK5MfqJ9"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include<WebServer.h>
#include "DHT.h"
#include "MQ135.h"
#include <MQUnifiedsensor.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT11
#define buz 23
// definitions for MQ135
#define placa "ESP-32"
#define Voltage_Resolution 3.3
#define pin 34 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm 
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type); 

BlynkTimer timer;

// *****AUTHENTICATION FOR IOT SERVER
const char *auth = BLYNK_AUTH_TOKEN;
const char* ssid = "Hehe";
const char* password = "noman amin";

WebServer server(80);

uint8_t DHTPin = 4;

DHT dht(DHTPin, DHTTYPE);

float temperature;
float humidity;
int threshold = 400;

void sendSensor()
{
  //************* TEMPERATUR AND HUMIDTY
  
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  float heat_index = dht.computeHeatIndex(temperature, humidity, false); //compute heat index in celcius (isFahrenheit = false)
  Serial.println("----------------------------------------------");
  Serial.print("Temperature is : ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial. print("Humidity is :");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Heat index is ");
  Serial.print(heat_index);
  Serial.println(" °C");
  Serial.println("-----------------------------------------------");
   
   // MQ Reading and updating section
  MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
  MQ135.setA(605.18); MQ135.setB(-3.937); // Configurate the ecuation values to get CO concentration
  float CO = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(77.255); MQ135.setB(-3.18); // Configurate the ecuation values to get Alcohol concentration
  float Alcohol = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(110.47); MQ135.setB(-2.862); // Configurate the ecuation values to get CO2 concentration
  float CO2 = 410 + MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(44.947); MQ135.setB(-3.445); // Configurate the ecuation values to get Toluene concentration
  float Toluene = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(102.2 ); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
  float NH4 = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(34.668); MQ135.setB(-3.369); // Configurate the ecuation values to get Acetone concentration
  float Acetone = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

  // ****** WRITING IN IOT SERVER
  Blynk.virtualWrite(V0,temperature);
  Blynk.virtualWrite(V1,humidity);
  Blynk.virtualWrite(V2, CO2);
  Blynk.virtualWrite(V8, heat_index);
  Blynk.virtualWrite(V3, NH4);
  Blynk.virtualWrite(V4, Alcohol);
  Blynk.virtualWrite(V5, Toluene);
  Blynk.virtualWrite(V6, CO);
  Blynk.virtualWrite(V7, Acetone);
  
  Serial.print("CO: "); Serial.println(CO);
  Serial.print("Alcohol: "); Serial.println(Alcohol);
 
  Serial.print("CO2: ");
  Serial.println(CO2);

  Serial.print("TOLUENE: ");
  Serial.println(Toluene);

  Serial.print("NH4: ");
  Serial.println(NH4);

  Serial.print("ACETONE: ");
  Serial.println(Acetone);

  if(CO2 > 425 || CO > 50 || Alcohol > 50 || Toluene > 50 || NH4 > 50 || Acetone > 50)
  {
    Serial.println("heavyyyy!!!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Air quality is");
    lcd.setCursor(0,1);
    lcd.print("   poor!!!");
    digitalWrite(buz,HIGH);
  }
  else
  {
    Serial.println("Good");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Air quality is");
    lcd.setCursor(0,1);
    lcd.print("   good.");
    digitalWrite(buz,LOW);
  }

  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(13,0);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.setCursor(13,1);
  lcd.print("%");
  delay(3000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CO2: ");
  lcd.print(CO2);
  lcd.setCursor(13,0);
  lcd.print("ppm");
  lcd.setCursor(0,1);
  lcd.print("CO: ");
  lcd.print(CO);
  lcd.setCursor(13,1);
  lcd.print("ppm");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alc: ");
  lcd.print(Alcohol);
  lcd.setCursor(13,0);
  lcd.print("ppm");
  lcd.setCursor(0,1);
  lcd.print("Toluene: ");
  lcd.print(Toluene);
  lcd.setCursor(13,1);
  lcd.print("ppm");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("NH4: ");
  lcd.print(NH4);
  lcd.setCursor(13,0);
  lcd.print("ppm");
  lcd.setCursor(0,1);
  lcd.print("Acetone: ");
  lcd.print(Acetone);
  lcd.setCursor(13,1);
  lcd.print("ppm");
  delay(3000);
}

void setup() {

  Serial.begin(115200);
  delay(100);
  
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Air Quality");
  lcd.setCursor(0,1);
  lcd.print("   Analyzer");

  pinMode(buz, OUTPUT);
  pinMode(DHTPin, INPUT);
  dht.begin();

  // **** CONNECTING TO WIFI
  
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status()!= WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected...!");
  Serial.print("Got IP: "); Serial.println(WiFi.localIP());
  Serial.println("");

  Serial .println("");

  // ******* CALIBRATING SENSOR
  MQ135.setRegressionMethod(1); // to set if the graph is linear or exponential 1->exponential
  MQ135.init();
  Serial.println("Calribrating please wait : ");
  float calcR0 = 0;
  for(int i = 1; i <= 10; i++)
  {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("   done!.");
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}

  //CONNECT TO IOT SERVER
  Blynk.begin(auth, ssid, password);
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();  

}
