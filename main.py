import os
import uvicorn

from fastapi import FastAPI, Request, File, UploadFile, Form, HTTPException
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from starlette import status
from starlette.responses import RedirectResponse
from werkzeug.utils import secure_filename
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from src import mpi, papa
from loguru import logger

HOST = '0.0.0.0'
PORT = 9200
tokens_json_path = 'src/tokens/mpi.json'

es = Elasticsearch([{'host': HOST, 'port': PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': PORT, 'scheme': 'http'}])

logger = logger.opt(colors=True)

app = FastAPI()
templates = Jinja2Templates(directory='web/html')
app.mount('/web', StaticFiles(directory='web'), name='web')

with open(tokens_json_path, 'r', encoding='utf8') as tokens:
    model = papa.PAPA(es, mpi.tokenizer, tokens.read())
    model.create()


@app.get('/')
async def home(request: Request, result: str = 'Пока пусто...') -> templates.TemplateResponse:
    try:
        subjects = model.get_field_values('subject')
        work_types = model.get_field_values('work_type')
        task_nums = model.get_field_values('task_num')

        return templates.TemplateResponse(
            'index.html', {
                'request': request, 'subjects': subjects,
                'work_types': work_types, 'task_nums': task_nums,
                'result': result
            }
        )
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/add_file')
async def add_document(file: UploadFile = File(...)):
    try:
        if not file:
            return JSONResponse(status_code=400, content={'message': 'File not loaded!'})

        if file.filename == '':
            return JSONResponse(status_code=400, content={'message': 'File empty!'})

        filename = secure_filename(file.filename)
        file_content = await file.read()
        model.add(filename, file_content.decode('utf-8'))

        return JSONResponse(content={'code': 200, 'message': 'Файл загружен!'})
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/papa')
async def papa(request: Request, file: UploadFile = File(...), subject: str = Form(...), work_type: str = Form(...),
               task_num: str = Form(...)):
    result = str()

    try:
        if not file:
            return JSONResponse(status_code=400, content={'code': 400, 'message': 'File not loaded!'})
        if file.filename == '':
            return JSONResponse(status_code=400, content={'code': 400, 'message': 'File empty!'})

        file_content = await file.read()
        filename = secure_filename(file.filename)
        src_filenames = model.get_src(subject, work_type, task_num)

        if not src_filenames:
            src_filename = 'all'
            print(model.papa(file_content.decode('utf-8'), filename, src_filename))
        else:
            for src_filename in src_filenames:
                print(model.papa(file_content.decode('utf-8'), filename, src_filename))

        return templates.TemplateResponse('/', {'request': request, 'result': result})
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/field/')
async def field_list(name: str = Form(...)):
    try:
        data = model.get_field_values(name)
        return JSONResponse(content={'result': data})
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.get('/add')
async def add_file_page(request: Request):
    return templates.TemplateResponse('add.html', {'request': request})


if __name__ == '__main__':
    uvicorn.run(app, host='0.0.0.0', port=int(os.environ.get('PORT_WEB', 8000)))
