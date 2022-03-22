#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include "plant.h"
#include "network.h"
#include <iostream>

void Plant::Setup(String code, float min_temp, float max_temp, float humidity, float soil_humidity)
{
  code = code;
  min_temp = min_temp;
  max_temp = max_temp;
  humidity = humidity;
  soil_humidity = soil_humidity;
}
float Plant::get_humidity()
{
  return humidity
}
void Plant::Info()
{
  Serial.println("code: ", code);
  Serial.println("min temperature: ", min_temp);

  Serial.println("max temperature: ", max_temp);

  Serial.println("humidity: ", humidity);

  Serial.println("soil humidity: ", soil_humidity);

  Serial.println("cur temp: ", tmp_temp);

  Serial.println("cur humidity: ", tmp_humidity);

  Serial.println("tmp_water: ", tmp_water);

  Serial.println("tmp_soil_humidity: ", tmp_soil_humidity);
}
bool Plant::CheckTemperature(float temp)
{
  if ((temp >= min_temp) && (temp <= max_temp))
    return true;
  return false;
}
String Plant::Zip()
{
  return "Te=" + String(tmp_temp) + "&Hu=" + String(tmp_humidity) + "&Wa=" + String(tmp_water) + "&SH=" + String(tmp_soil_humidity);
}

Plant plant;
Network network;

network.set_domain("localhost");
void Network::Setup()
{
}
void Network::set_domain(String url)
{
  domain = url;
}
void Network::response_collation(String response)
{
  char json[500];
  response.replace(" ", "");
  response.replace("\n", "");
  response.trim();
  response.remove(0, 1);
  response.toCharArray(json, 500);
  deserializeJson(doc, "{" + String(json)); //! 不明原因，缺了"{"
}
void Network::set_url(String path)
{
  http.begin(domain + path);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
}
void Network::post_infomation()
{
  set_url("/user/store_info/");
  http.POST(plant.Zip());
  response_collation(http.getString());
  Serial.print("response data = ");
  Serial.println(doc["status"].as<bool>());
}
void Network::get_my_plant_code()
{
  set_url("/api/my_plant/");
  http.GET();
  response_collation(http.getString());
  plant.code = String(doc["code"].as<char *>());
}
//! return multi float
float Network::GetUserConfig(Plant &p)
{
  set_url("/api/plant_info/");
  http.POST("code=" + p.get_code());
  response_collation(http.getString());
  return doc["minTemp"].as<float>(), doc["maxTemp"].as<float>(), doc["humidity"].as<float>();
  , doc["soil_humidity"].as<float>();
}

//! DHT11 溫溼度檢測
#define DHT_PIN 6
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE); // init

void DHTSensor()
{
  delay(2000);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    float hic = dht.computeHeatIndex(temperature, humidity, false);

    Serial.print(F("Humidity: "), humidity);
    Serial.print(F("%  Temperature: ", temperature));
    Serial.print(F(" ˙C, "));
    Serial.print(F(" Heat index: "));
    Serial.print(hic);
    Serial.println(F(" ˙C "));
  }
  else
  {
    delay(200);
    Serial1.println("DHTSensor > not working!!!");
  }
}

//! LED
#define LED_PIN 23
void InitLED()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}
void LED()
{
}

//! measure water
#define WATER_SENSOR_POWER_PIN 8
#define WATER_SENSOR_DATA_PIN A16
int water_curr = 0;

void InitWaterSensor()
{
  pinMode(WATER_SENSOR_DATA_PIN, INPUT);
  pinMode(WATER_SENSOR_POWER_PIN, OUTPUT);
}
void WaterSensor()
{
  delay(1000);
  water_curr = analogRead(WATER_SENSOR_DATA_PIN);
  Serial.println("current water: ", water_curr);
  // if (warning[water_warn_index] < 5)
  // {
  //   digitalWrite(WATER_SENSOR_POWER_PIN, HIGH);
  //   delay(1000);
  //   water_curr = analogRead(WATER_SENSOR_DATA_PIN);
  //   Serial.print("value: ");
  //   Serial.println(water_curr);
  //   if (water_curr > 1700)
  //   {
  //     Serial.println("over");
  //   }
  //   else
  //   {
  //     Serial.println("not");
  //   }
  //   digitalWrite(WATER_SENSOR_POWER_PIN, LOW);
  // }
}

//! Motor
#define ANGLE_POWER_PIN 12
Servo motor;
int position = 0;
int min_degrees = 0;
int max_degrees = 180;
int increase = 2;
void InitWaterBoard()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  motor.setPeriodHertz(50);
  motor.attach(ANGLE_POWER_PIN, 1000, 2000);
}
void WaterBoard()
{
  for (position = min_degrees; position <= max_degrees; position += increase)
  {
    motor.write(position);
    delay(10);
  }
  for (position = max_degrees; position >= min_degrees; position -= increase)
  {
    motor.write(position);
    delay(10);
  }
}
//! Solid humidity sensor
#define SOIL_PIN A0

void InitSoilSensor()
{
}
void SoilSensor()
{
  // D(0, 1) -> digitalRead(D_PIN)
  Serial.println(analogRead(SOIL_PIN));
}

// #include "time.h"
// const char *ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 3600;
// const int daylightOffset_sec = 3600;
// void printLocalTime()
// {
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo))
//   {
//     Serial.println("Failed to obtain time");
//     return;
//   }
//   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
// }
// ------------------------- Timer -------------------------

hw_timer_t *timer = NULL;
void IRAM_ATTR onTimer()
{
}
void InitTimer()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // 1 second = 1000000 us
  timerAlarmWrite(timer, 1000000 * 6, tru /* repeatable */);
  timerAlarmEnable(timer);
}

const char *wifi_id = "wifi_ID";
const char *wifi_passwd = "wifi_password";
void InitWiFi()
{
  Serial.println("Init WIFI...");
  WiFi.begin(wifi_id, wifi_passwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting...");
    delay(1000);
  }
  Serial.println("Connected!");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("==== init ====");
  InitWiFi();
  network.get_my_plant_code();
  network.get_plant_info(plant.code);
  // 暫時，存前各值(tmp_開頭)
  set_tmp_plant_value();
  // Regularly updated
  // http_post_infomation();
  plant.Setup(network.GetUserConfig(plant.code))
      plant.Info();

  // InitTimer();
  // InitWaterSensor();
  // InitWaterBoard();
}

void loop()
{
}
// printLocalTime();

// Serial.print(millis());
// dht.begin();

// pinMode(Water_Sensor, INPUT);
// pinMode(WATER_SENSOR_POWER, OUTPUT); ESP給電予你
// digitalWrite(WATER_SENSOR_POWER, LOW);

// pinMode(SOIL_PIN, INPUT); //D(input), A(input)

// esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_NUM_22, 0);
// xTaskCreatePinnedToCore(
//     Task1code, /* Task function. */
//     "Task1",   /* name of task. */
//     10000,     /* Stack size of task */
//     NULL,      /* parameter of the task */
//     1,         /* priority of the task */
//     &Task1,    /* Task handle to keep track of created task */
//     0);        /* pin task to core 0 */

// --------------------- 暫時 -----------------
void set_tmp_plant_value()
{
  plant.tmp_temp = 20.0;
  plant.tmp_humidity = 70.0;
  plant.tmp_soil_humidity = 1000.0;
  plant.tmp_water = 1400.0;
}