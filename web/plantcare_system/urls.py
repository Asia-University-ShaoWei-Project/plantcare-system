from django.contrib import admin
from django.urls import include, path

urlpatterns = [
    path('admin/', admin.site.urls),
    path('api/', include('api.urls')),
    path('line/', include('line_bot.urls')),
    path('user/', include('user.urls')),
]
