from django.http import HttpRequest, HttpResponse, HttpResponseBadRequest
from django.views.decorators.csrf import csrf_exempt
from apps.user.models import Profile

import os

from linebot.models import MessageEvent, TextMessage, TextSendMessage
from linebot import (LineBotApi, WebhookHandler)
from linebot.exceptions import (InvalidSignatureError)

LINE_BOT_ACCESS_TOKEN = os.getenv('LINE_BOT_ACCESS_TOKEN')
LINE_BOT_SECURE = os.getenv('LINE_BOT_SECURE')

line_bot_api = LineBotApi(LINE_BOT_ACCESS_TOKEN)
handler = WebhookHandler(LINE_BOT_SECURE)


@csrf_exempt
def callback(request: HttpRequest):
  signature = request.headers['X-Line-Signature']
  body = request.body.decode()
  try:
    handler.handle(body, signature)
  except InvalidSignatureError:

    messages = (
        "Invalid signature. Please check your channel access token/channel secret."
    )
    return HttpResponseBadRequest(messages)
  return HttpResponse('OK')

# LINE處理訊息
# 當收到 LINE 的 MessageEvent (信息事件)，而且信息是屬於 TextMessage (文字信息)的時候
# MessageEvent、FollowEvent、UnfollowEvent、JoinEvent、LeaveEvent、MemberJoinedEvent、MemberLeftEvent
# MessageEvent=[TextMessage、ImageMessage、VideoMessage、StickerMessage、FileMessage]
# event.reply_token 唯一次性(用完即丟)


@handler.add(MessageEvent, message=TextMessage)
def handle_message(event: MessageEvent):
  text = event.message.text
  if text in ['help', 'list']:
    message = "help\ninfo\nweb\nsetting\napi\ntoday"
    # event.source.user_id
  elif text in ['setting', 'web']:
    message = "https://smart-terminal-dj.herokuapp.com/set"
  else:
    message = "可以輸入以下命令\n..."
  line_bot_api.reply_message(
      reply_token=event.reply_token,
      messages=TextSendMessage(text=message)
  )


@csrf_exempt
def push(request):
  user = Profile.objects.get(id=1)
  message = "似乎元件損壞，請盡快修護!!"
  TO = os.getenv("LINE_BOT_TO")
  line_bot_api.push_message(
      TO, TextSendMessage(text=user.username+", "+message))

  return None
