from django.contrib import admin
from django.urls import include, path

urlpatterns = [
    path('admin/', admin.site.urls),
    path('api/', include('apps.api.urls')),
    path('line/', include('apps.line.urls')),
    path('user/', include('apps.user.urls')),
]
