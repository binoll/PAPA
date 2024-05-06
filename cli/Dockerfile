FROM python:latest
WORKDIR /PAPA
COPY requirements.txt .
RUN pip install --upgrade pip
RUN pip install -r requirements.txt
COPY ./ .
ENTRYPOINT ["python", "main.py"]
