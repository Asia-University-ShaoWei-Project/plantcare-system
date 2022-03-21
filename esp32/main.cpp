#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

class Plant
{
public:
  String code;
  float s_min_temp;
  float s_max_temp;
  float s_humidity;
  float s_soil_humidity;
  float cur_temp;
  float cur_humidity;
  float cur_water;
  float cur_soil_humidity;
  void set_value(String, float, float, float, float);
  void print_all();
  bool temp_comppare(float);
  String zip_data();
};
void Plant::set_value(String code, float min_temp, float max_temp, float humidity, float soil_humidity)
{
  code = code;
  s_min_temp = min_temp;
  s_max_temp = max_temp;
  s_humidity = humidity;
  s_soil_humidity = soil_humidity;
}
void Plant::print_all()
{
  Serial.print("----- Plant information -----");
  Serial.print("code: ");
  Serial.println(code);
  Serial.print("s_min_temp: ");
  Serial.println(s_min_temp);
  Serial.print("s_max_temp: ");
  Serial.println(s_max_temp);
  Serial.print("s_humidity: ");
  Serial.println(s_humidity);
  Serial.print("s_soil_humidity: ");
  Serial.println(s_soil_humidity);
  Serial.print("cur_temp: ");
  Serial.println(cur_temp);
  Serial.print("cur_humidity: ");
  Serial.println(cur_humidity);
  Serial.print("cur_water: ");
  Serial.println(cur_water);
  Serial.print("cur_soil_humidity: ");
  Serial.println(cur_soil_humidity);
  Serial.print("-------------------------");
  Serial.println();
}
bool Plant::temp_comppare(float temp)
{
  if ((temp >= s_min_temp) && (temp <= s_max_temp))
    return true;
  return false;
}
String Plant::zip_data()
{
  // output: "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";
  return "Te=" + String(cur_temp) + "&Hu=" + String(cur_humidity) + "&Wa=" + String(cur_water) + "&SH=" + String(cur_soil_humidity);
}

Plant plant;
// ------------------------- WiFi -------------------------
const char *ssid = "wifi-id";
const char *password = "wifi-password";
void InitWiFi()
{
  Serial.println("init wifi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("connecting...");
    delay(1000);
  }
  Serial.println("WiFi connected!");
}
// ------------------------- Http Client -------------------------
HTTPClient http;
String domainname = "http://localhost:8000";
StaticJsonDocument<200> doc;
// collation response data
void http_response_collation(String response)
{
  char json[500];
  response.replace(" ", "");
  response.replace("\n", "");
  response.trim();
  response.remove(0, 1);
  response.toCharArray(json, 500);
  deserializeJson(doc, "{" + String(json));
}

void http_set_url(String url)
{
  http.begin(domainname + url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
}
// post update information Save to DB.
void http_post_infomation()
{
  http_set_url("/user/store_info/");
  http.POST(plant.zip_data());
  http_response_collation(http.getString());
  Serial.print("response data = ");
  Serial.println(doc["status"].as<bool>());
}
// setting plant code
void http_get_my_plant_code()
{
  http_set_url("/api/my_plant/");
  http.GET();
  http_response_collation(http.getString());
  plant.code = String(doc["code"].as<char *>());
}
void http_get_plant_info(String code)
{
  http_set_url("/api/plant_info/");
  http.POST("code=" + code);
  http_response_collation(http.getString());
  plant.s_min_temp = doc["minTemp"].as<float>();
  plant.s_max_temp = doc["maxTemp"].as<float>();
  plant.s_humidity = doc["humidity"].as<float>();
  plant.s_soil_humidity = doc["soil_humidity"].as<float>();
}
// ------------------------- DHT11 溫溼度檢測 -------------------------
#define DHT_PIN 6
#define DHT_TYPE DHT11
// Initialize DHT sensor.
DHT dht(DHT_PIN, DHT_TYPE);

void DHTSensor()
{
  Serial1.println("do DHTSensor");
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
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

// ------------------------- LED -------------------------
#define LED_PIN 23
void InitLED()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}
void LED()
{
}

// ------------------------- measure water -------------------------
#define WATER_SENSOR_POWER_PIN 8
#define WATER_SENSOR_DATA_PIN A16
int water_curr_value = 0;

void InitWaterSensor()
{
  pinMode(WATER_SENSOR_DATA_PIN, INPUT);
  pinMode(WATER_SENSOR_POWER_PIN, OUTPUT);
}
void WaterSensor()
{
  delay(1000);
  water_curr_value = analogRead(WATER_SENSOR_DATA_PIN);
  Serial.print("value: ");
  Serial.println(water_curr_value);
  // if (warning[water_warn_index] < 5)
  // {
  //   digitalWrite(WATER_SENSOR_POWER_PIN, HIGH);
  //   delay(1000);
  //   water_curr_value = analogRead(WATER_SENSOR_DATA_PIN);
  //   Serial.print("value: ");
  //   Serial.println(water_curr_value);
  //   if (water_curr_value > 1700)
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

// ------------------------- 伺服器馬達 -------------------------
#define ANGLE_POWER_PIN 12
Servo myservo;
int pos = 0;
void InitWaterBoard()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  // standard 50 hz servo
  myservo.setPeriodHertz(50);
  myservo.attach(ANGLE_POWER_PIN, 1000, 2000);
}
void WaterBoard()
{

  for (pos = 0; pos <= 180; pos += 2)
  { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delay(10);          // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 2)
  {                     // goes from 180 degrees to 0 degrees
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delay(10);          // waits 15ms for the servo to reach the position
  }
}
// ------------------------- 土壤濕度偵測 -------------------------
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
  // 1 second = 1000000us
  timerAlarmWrite(timer, 1000000 * 6, true /* repeatable */);
  timerAlarmEnable(timer);
}

// ------------------------- setup -------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  InitWiFi();
  http_get_my_plant_code();
  http_get_plant_info(plant.code);
  // 暫時，存前各值(cur_開頭)
  set_cur_plant_value();
  // 定期更新
  // http_post_infomation();
  plant.print_all();

  // InitTimer();
  // InitWaterSensor();
  // InitWaterBoard();
}
// ------------------------- loop -------------------------

void loop()
{
}
// printLocalTime();

// Serial.print(millis());
// dht.begin();

// pinMode(Water_Sensor, INPUT);
// pinMode(WATER_SENSOR_POWER, OUTPUT);
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
void set_cur_plant_value()
{
  plant.cur_temp = 20.0;
  plant.cur_humidity = 70.0;
  plant.cur_soil_humidity = 1000.0;
  plant.cur_water = 1400.0;
}