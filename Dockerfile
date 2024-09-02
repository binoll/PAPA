FROM python:latest
WORKDIR /app
COPY src/main.py /app
COPY src/web /app/web
COPY src /app/src
COPY requirements.txt /app
RUN python -m pip install -r requirements.txt
EXPOSE 8000
CMD ["uvicorn", "main:app", "--host", "127.0.0.1", "--port", "8000"]
