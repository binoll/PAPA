# PAPA

## Зависимости
```bash
echo "deb [trusted=yes] https://mirror.yandex.ru/mirrors/elastic/8/ stable main" | sudo tee /etc/apt/sources.list.d/elastic-8.x.list
sudo apt-get install elasticsearch

pip install -r requirements.txt
```

**PAPA** - Plagiated Anti-Plagiarism Application - автоматизированное средство определения заимствований в исходных
кодах программ.

## Построение Docker-образа:

```bash
docker build -t app .
```

## Запуск Docker-контейнера:

```bash
docker run -p 8000:8000 app
```
