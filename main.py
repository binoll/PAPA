from typing import Optional

import uvicorn

from fastapi import FastAPI, Request, File, UploadFile, Form
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

with open(tokens_json_path, 'r', encoding='utf-8') as tokens:
    model = papa.PAPA(es, mpi.tokenizer, tokens.read())
    model.create_index()


@app.get('/')
async def home(request: Request, results=None) -> templates.TemplateResponse:
    if results is None:
        results = ['Пока пусто...']

    try:
        subjects = model.get_field_values('subject')
        work_types = model.get_field_values('work_type')
        task_nums = model.get_field_values('task_num')

        return templates.TemplateResponse(
            'index.html', {
                'request': request, 'subjects': subjects,
                'work_types': work_types, 'task_nums': task_nums,
                'results': results
            }
        )
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/add_file')
async def add_document(file: UploadFile = File(...)):
    try:
        if not file:
            return JSONResponse(status_code=400, content={'message': 'File \'{filename}\' not loaded!'})

        if file.filename == '':
            return JSONResponse(status_code=400, content={'message': 'File \'{filename}\' empty!'})

        filename = secure_filename(file.filename)
        file_content = await file.read()
        model.add(filename, file_content.decode('utf-8'))

        return JSONResponse(status_code=200, content={'message': f'Файл \'{filename}\' загружен!'})
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/papa')
async def papa(request: Request, file: UploadFile = File(...),
               subject: Optional[str] = Form(None),
               work_type: Optional[str] = Form(None),
               task_num: Optional[str] = Form(None)):
    try:
        if not file:
            return JSONResponse(status_code=400, content={'results': ['File not loaded!']})
        if file.filename == '':
            return JSONResponse(status_code=400, content={'results': ['File empty!']})

        if not subject:
            subjects = model.get_field_values('subject')

        if not work_type:
            work_types = model.get_field_values('work_type')

        if not task_num:
            task_nums = model.get_field_values('task_num')

        file_content = await file.read()
        filename = secure_filename(file.filename)

        src_filename = 'all' if not subject else subject
        if work_type:
            src_filename += f'_{work_type}'
        if task_num:
            src_filename += f'_{task_num}'

        results = model.papa(file_content.decode('utf-8'), filename, src_filename)

        if not results:
            return RedirectResponse(url=app.url_path_for('home'), status_code=status.HTTP_303_SEE_OTHER)

        return templates.TemplateResponse(
            'index.html', {
                'request': request, 'subjects': subjects,
                'work_types': work_types, 'task_nums': task_nums,
                'results': results
            }
        )
    except Exception as e:
        return JSONResponse(status_code=500, content={'results': [str(e)]})


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
    uvicorn.run(app, host='0.0.0.0', port=8000)
