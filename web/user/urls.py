from django.urls import path
from . import views
urlpatterns = [
    path('setting/', views.setting, name="setting"),
    path('info/', views.info, name="info"),
    path('store_info/', views.save_infomation, name="store_info"),
    path('edit/', views.edit_profile, name="edit"),

]
