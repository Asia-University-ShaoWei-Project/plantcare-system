from django.urls import path
from . import views
urlpatterns = [
    path('today/', views.today),
    path('rainfall/', views.rainfall),
    path('sendInfo/', views.getInfo),
    path('plant_info/', views.plant_info),
    path('my_plant/', views.my_plant),
]
