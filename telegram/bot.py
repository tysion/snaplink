import logging
import requests
import os
from telegram import Update, ReplyKeyboardRemove
from telegram.ext import (
    ApplicationBuilder,
    CommandHandler,
    MessageHandler,
    ContextTypes,
    filters,
    ConversationHandler,
)

API_URL = "http://snaplink-service:8080/api/v1/shorten"

logging.basicConfig(level=logging.INFO)


def shorten_url(url: str) -> str:
    response = requests.post(API_URL, json={"link": url})
    if response.status_code == 200:
        data = response.json()
        return f"http://localhost:8080/t/{data['id']}"
    else:
        return "Ошибка при сокращении ссылки. Попробуйте позже."


async def handle_message(update: Update, context: ContextTypes.DEFAULT_TYPE):
    message_text = update.message.text
    if message_text.startswith("http://") or message_text.startswith("https://"):
        short_link = shorten_url(message_text)
        await update.message.reply_text(short_link)
    else:
        await update.message.reply_text("Пожалуйста, отправьте корректную ссылку.")


def main():
    token = os.getenv("TELEGRAM_BOT_TOKEN")
    application = ApplicationBuilder().token(token).build()
    application.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, handle_message))
    application.run_polling()


if __name__ == "__main__":
    main()