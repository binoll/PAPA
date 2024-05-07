FROM python:latest
RUN mkdir -p /app
COPY ./ /app
WORKDIR /app
RUN python -m pip install -r requirements.txt
CMD [ "python", "app.py" ]
