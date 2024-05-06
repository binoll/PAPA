FROM python:3.8.16

RUN mkdir -p /app

COPY ./ /app

WORKDIR /app
RUN python -m pip install -r requirements.txt

CMD [ "python", "app.py" ]