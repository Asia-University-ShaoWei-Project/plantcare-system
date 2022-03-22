class Plant
{
public:
  String code;
  float min_temp;
  float max_temp;
  float humidity;
  float soil_humidity;
  float tmp_temp;
  float tmp_humidity;
  float tmp_water;
  float tmp_soil_humidity;
  float get_humidity() void Setup(String, float, float, float, float);
  void Info();
  bool CheckTemperature(float);

  String Zip();
};