from django.shortcuts import render
from django.http.response import JsonResponse
# from django.core import serializers
from django.views.decorators.csrf import csrf_exempt
from .models import TodayWeather, PlantInfo
from user.models import Profile
from datetime import datetime
import requests


@csrf_exempt
def getInfo(request):
    # POST: api_key, sensor
  return JsonResponse({'id': 115})


def testModel(request):
  data = {
      'status': True,
      "test": "hello world",
      # 'info':[1,2,3],
      # 'data':{
      #     'g':'e',
      #     'gg':123
      # }
  }
  # data = serializers.serialize("json", API.objects.all())
  return JsonResponse(data)


def today(request):
  result_date = {"T": None, "RH": None, "PoP": None}
  now = datetime.now()
  date = now.strftime("%Y-%m-%d")
  if TodayWeather.objects.filter(date=date).count() > 0:
    print('Get DB data')
    title = ["T", "RH", "PoP"]
    db_data = TodayWeather.objects.get(date=date)
    data = [db_data.temperatures, db_data.humidity, db_data.PoP]
    for i, v in enumerate(data):
      result_date[title[i]] = v['value'][minHour(v['time'])]
    #   Take the data of weather forecast API
  else:
    print('Get API data')
    db_data = {}
    api_url = "https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-D0047-073?Authorization=CWB-8754F957-FEDB-4FD0-B8B8-FB3E5E7A361E&format=JSON&locationName=大里區&elementName=T,RH,PoP6h"
    api_data = req_OpenSource(url=api_url)[0]['location'][0]['weatherElement']
    sele = ['T', 'RH', 'PoP']
    date_attr = ['dataTime', 'dataTime', 'startTime']
    today = now.strftime("%d")
    for i, v in enumerate(sele):
      tmp = {"time": [], "value": []}
      tmp_date_attr = date_attr[i]
      for j in api_data[i]['time']:
          # day
        if j[tmp_date_attr][8:10] == today:
            # hour
          tmp['time'].append(int(j[tmp_date_attr][11:13]))
          # value
          tmp['value'].append(int(j['elementValue'][0]['value']))
      db_data[v] = tmp
      index = minHour(tmp['time'])
      # 最近時間點數據
      result_date[v] = tmp['value'][index]

    TodayWeather(
        date=date,
        temperatures=db_data['T'],
        humidity=db_data['RH'],
        PoP=db_data['PoP']
    ).save()
  return JsonResponse(result_date)


def minHour(data):
  hour = int(datetime.now().strftime("%H"))
  tmp_min = 24
  tmp_value = 0
  for i, v in enumerate(data):
    tmp = hour - v
    if (tmp >= 0) and (tmp < tmp_min):
      tmp_min = tmp
      tmp_value = i
    else:
      if tmp_min < abs(tmp):
        return i-1
      return i


def plant_infomation(request):
  # Wx: 天氣現象, MaxT: 最高溫度, MinT: 最低溫度, CI: 舒適度, Pop: 降雨機率
  api_url = "https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-C0032-001?Authorization=CWB-8754F957-FEDB-4FD0-B8B8-FB3E5E7A361E&format=JSON&locationName=%E8%87%BA%E4%B8%AD%E5%B8%82&elementName=Wx,PoP,MinT,MaxT"
  data = req_OpenSource(url=api_url)
  if data:
    result = {
        'status': True,
        'mes': 'today infomation',
        'info': data
    }
  return JsonResponse(result)


def two_days_prediction(request):
  now = datetime.now()
  print(now.strftime("%Y-%m-%d %H:%M:%S"))

  api_url = "https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-D0047-073?Authorization=CWB-8754F957-FEDB-4FD0-B8B8-FB3E5E7A361E&format=JSON&locationName=大里區&elementName=T,RH,PoP6h"
  data = req_OpenSource(url=api_url)
  if data:
    data = data['weatherElement']
    result = {
        'status': True,
        'mes': 'today infomation',
        'info': data
    }
  return JsonResponse(result)


def rainfall(request):
  #   api_url = "https://opendata.cwb.gov.tw/api/v1/rest/datastore/O-A0002-001?Authorization=CWB-8754F957-FEDB-4FD0-B8B8-FB3E5E7A361E&format=JSON&locationName=%E5%A4%A7%E9%87%8C,%E6%A1%90%E6%9E%97&parameterName=CITY_SN"
  #   data = requests.get(api_url)
  data = True
  if data:
    result = {
        'status': True,
        'mes': '會下雨',
        'age': '111'
    }
  return JsonResponse(result)


def req_OpenSource(url):
  req = requests.get(url)
  if req.status_code == 200:
    req = req.json()
    if req['success'] == 'true':
      return req['records']['locations']
  return {'status': False}


@csrf_exempt
def plant_info(request):
  code = request.objects.get('code')
  db_data = PlantInfo.objects.get(code=code)
  return JsonResponse({
      'minTemp': db_data.minTemp,
      'maxTemp': db_data.maxTemp,
      'humidity': db_data.humidity,
      'soil_humidity': db_data.soil_humidity
  })


def my_plant(request):
  user = Profile.objects.get(id=1)
  return JsonResponse({'code': user.plant})
