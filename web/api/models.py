from django.db import models
# from django import forms
# class CreateUserCollectFolder(forms.Form):
#     folder_name = forms.CharField(max_length=15, label="Folder Name")
#     book_id = forms.IntegerField()
import jsonfield


class TodayWeather(models.Model):
  date = models.CharField(max_length=8)
  temperature = jsonfield.JSONField()
  humidity = jsonfield.JSONField()
  PoP = jsonfield.JSONField()  # Probability of Precipitation
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
