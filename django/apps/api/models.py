from django.db import models
import jsonfield


class TodayWeather(models.Model):
  date = models.CharField(max_length=8)
  temperatures = jsonfield.JSONField()
  # 濕度百分比
  humidity = jsonfield.JSONField()
  # 降雨機率
  PoP = jsonfield.JSONField()
  created = models.DateTimeField(auto_now=True)

  def __str__(self):
    return self.date


class PlantInfo(models.Model):
  title = models.CharField(max_length=20)
  code = models.CharField(max_length=2)
  minTemp = models.IntegerField()
  maxTemp = models.IntegerField()
  humidity = models.IntegerField()
  soil_humidity = models.IntegerField()
