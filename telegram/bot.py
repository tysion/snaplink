import logging
import requests
import qrcode
import io
import os
from telegram import Update, InputMediaPhoto
from telegram.ext import (
    ApplicationBuilder,
    CommandHandler,
    MessageHandler,
    ContextTypes,
    filters,
    ConversationHandler,
)

INTERNAL_API_URL = "http://snaplink-service:8080/api/v1/shorten"
EXTERNAL_API_URL = os.getenv("EXTERNAL_API_URL", default="http://localhost:8080")

logging.basicConfig(level=logging.INFO)


def shorten_url(url: str) -> str:
    response = requests.post(INTERNAL_API_URL, json={"link": url})
    if response.status_code == 200:
        data = response.json()
        return f"{EXTERNAL_API_URL}/t/{data['id']}"
    else:
        return "Ошибка при сокращении ссылки. Попробуйте позже."


def generate_qr_code(url: str) -> io.BytesIO:
    qr = qrcode.make(url)
    img_bytes = io.BytesIO()
    qr.save(img_bytes, format="PNG")
    img_bytes.seek(0)
    return img_bytes


async def handle_message(update: Update, context: ContextTypes.DEFAULT_TYPE):
    message_text = update.message.text
    if message_text.startswith("http://") or message_text.startswith("https://"):
        short_link = shorten_url(message_text)
        qr_code = generate_qr_code(short_link)
        await update.message.reply_media_group(
            media=[
                InputMediaPhoto(qr_code, caption=short_link)
            ],
            reply_to_message_id=update.message.message_id
        )
    else:
        await update.message.reply_text("Пожалуйста, отправьте корректную ссылку.")


def main():
    token = os.getenv("TELEGRAM_BOT_TOKEN")
    application = ApplicationBuilder().token(token).build()
    application.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, handle_message))
    application.run_polling()


if __name__ == "__main__":
    main()