
#include <iostream>
#include <string>

class Network
{
public:
  HTTPClient http;
  StaticJsonDocument<200> doc;
  string domain, ssid, passwd;

  void Setup(String, String);
  void set_domain(String);
  void set_url();
  void response_collation();
  void post_infomation();
  void get_my_plant_code();
  void get_plant_info();
};