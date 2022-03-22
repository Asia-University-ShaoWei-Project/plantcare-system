from django.db import models
import jsonfield


class Profile(models.Model):
  username = models.TextField(max_length=10)
  # https://kknews.cc/zh-tw/news/mqnpykp.html
  PLANT_CHOICES = [
      ('nan', 'Nan'),
      ('rose', 'Rose'),
      ('tulip', 'Tulip')
  ]
  UPDATE_CHOICES = [
      # testing
      ('02', '20 second'),
      ('30', '30 minute'),
      ('60', '1 hour'),
      ('180', '3 hour'),
      ('720', '12 hour')
  ]
  plant = models.CharField(
      max_length=2,
      choices=PLANT_CHOICES,
      default='nan'
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
  water_storage = jsonfield.JSONField()
  soil_humidity = jsonfield.JSONField()


class SaveStatus(models.Model):
  soil_sensor = models.BooleanField(default=False)
  LED = models.BooleanField(default=False)
  water_storage_control = models.BooleanField(default=False)
  water_sensor = models.BooleanField(default=False)
  water_board = models.BooleanField(default=False)
  watering_control = models.BooleanField(default=False)
