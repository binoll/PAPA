FROM python:latest
WORKDIR /app
COPY src /app/src
COPY requirements.txt /app
RUN python -m pip install --upgrade pip
RUN python -m pip install -r requirements.txt
EXPOSE 8000
CMD ["uvicorn", "src.main:app", "--host", "0.0.0.0", "--port", "8000"]
