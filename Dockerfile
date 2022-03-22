FROM python:3
WORKDIR /web
COPY web/ /web
COPY requirements.txt /web
RUN pip install -r requirements.txt
