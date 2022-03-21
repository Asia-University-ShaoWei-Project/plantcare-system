from django.db import models

import jsonfield


class Profile(models.Model):
  username = models.TextField(default="Username", max_length=10)
  PLANT_CHOICES = [
      ('Na', '無'),
      ('A', '金錢樹'),
      ('B', '龍血樹')
  ]
  UPDATE_CHOICES = [
      ('02', '20秒(測試)'),
      ('30', '30分'),
      ('60', '1小時'),
      ('180', '3小時'),
      ('720', '12小時')
  ]
  plant = models.CharField(
      max_length=2,
      choices=PLANT_CHOICES,
      default='Na'
  )
  update_frequency = models.CharField(
      max_length=3,
      choices=UPDATE_CHOICES,
      default='30'
  )
  created = models.DateTimeField(auto_now=True)

  def __str__(self):
    return self.username


class SaveInfo(models.Model):
  day = models.CharField(max_length=2)
  time = models.CharField(max_length=8)
  temperatures = jsonfield.JSONField()
  humidity = jsonfield.JSONField()
  water_stage = jsonfield.JSONField()
  soil_humidity = jsonfield.JSONField()


class SaveStatus(models.Model):
  led = models.BooleanField(default=False)
  water_control = models.BooleanField(default=False)
  water_sensor = models.BooleanField(default=False)
  water_board = models.BooleanField(default=False)
  watering_control = models.BooleanField(default=False)
  soil_sensor = models.BooleanField(default=False)
