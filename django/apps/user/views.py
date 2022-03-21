import time

from django.shortcuts import render
from django.forms.models import model_to_dict
from django.views.decorators.csrf import csrf_exempt
from django.http.response import JsonResponse
from datetime import datetime
from django.shortcuts import redirect

from .models import Profile, SaveInfo


def setting(request):
  user = Profile.objects.get(id=1)
  data = model_to_dict(user)
  data['plant_name'] = user.get_plant_display()
  data['plant_sele'] = user.PLANT_CHOICES

  data['update_name'] = user.get_update_frequency_display()
  data['update_sele'] = user.UPDATE_CHOICES
  return render(request, 'User/setting.html', data)


def edit_profile(request):
  print(request.POST)
  user = Profile.objects.get(id=1)
  user.username = request.POST.get('username')
  user.plant = request.POST.get('plant')
  user.update_frequency = request.POST.get('update')
  user.save()
  return redirect('/user/setting/')


def info(request):
  now = datetime.now()
  db_data = SaveInfo.objects.filter(day=now.strftime("%d"))
  return render(request, 'User/info.html', {"info": db_data})


@csrf_exempt
def store_infomation(request):
  if request.method == 'POST':
    now = datetime.now()
    SaveInfo(
        day=now.strftime("%d"),
        time=now.strftime("%H:%M"),
        temperatures=request.POST.get('Te'),
        humidity=request.POST.get('Hu'),
        water_stage=request.POST.get('Wa'),
        soil_humidity=request.POST.get('SH'),
    ).save()
  return JsonResponse({'status': True})
