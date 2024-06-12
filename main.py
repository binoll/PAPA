from typing import Optional

import uvicorn
from fastapi import FastAPI, Request, File, UploadFile, Form, Depends, HTTPException
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
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

HOST = '127.0.0.1'
PORT = 9200
TOKENS_JSON_PATH = 'src/tokens/mpi.json'

es = Elasticsearch([{'host': HOST, 'port': PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': PORT, 'scheme': 'http'}])

logger = logger.opt(colors=True)

app = FastAPI()
templates = Jinja2Templates(directory='web/html')
app.mount('/web', StaticFiles(directory='web'), name='web')

with open(TOKENS_JSON_PATH, 'r', encoding='utf-8') as tokens:
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
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )


@app.get('/add')
async def add_file_page(request: Request):
    return templates.TemplateResponse('add.html', {'request': request})


@app.post('/add_file')
async def add_document(file: UploadFile = File(...)):
    try:
        if not file:
            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': 'Файл не загружен!'}
            )

        if file.filename == '':
            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': 'Пустое имя файла!'}
            )

        filename = secure_filename(file.filename)
        file_content = await file.read()

        result = model.add(filename, file_content.decode('utf-8'))

        if result:
            return JSONResponse(
                status_code=status.HTTP_200_OK,
                content={'message': f'Файл \"{filename}\" загружен!'}
            )

        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={'message': f'Файл \"{filename}\" не загружен! Неправильный формат файла!'}
        )
    except Exception as e:
        return JSONResponse(status_code=status.HTTP_500_INTERNAL_SERVER_ERROR, content={'message': str(e)})


@app.post('/papa')
async def papa(request: Request, file: UploadFile = File(...),
               subject: Optional[str] = Form(None),
               work_type: Optional[str] = Form(None),
               task_num: Optional[str] = Form(None)):
    try:
        if not file:
            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': 'Файл не загружен!'}
            )

        if file.filename == '':
            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': 'Пустое имя файла!'}
            )

        subjects = model.get_field_values('subject')
        work_types = model.get_field_values('work_type')
        task_nums = model.get_field_values('task_num')

        file_content = await file.read()
        filename = secure_filename(file.filename)

        src_filename = 'all' if not subject else subject
        if work_type:
            src_filename += f'_{work_type}'
        if task_num:
            src_filename += f'_{task_num}'

        results = model.papa(file_content.decode('utf-8'), filename, src_filename)

        if len(results) == 1:
            results = results[0]
        elif len(results) > 1 and len(results['diff']) == 0:
            results = None
        else:
            results = (results['dst_name'] + ' ' + results['dst_code'] +
                       ' ' + results['diff'] + ' ' + results['sims'])

        return templates.TemplateResponse(
            'index.html', {
                'request': request, 'subjects': subjects,
                'work_types': work_types, 'task_nums': task_nums,
                'results': results
            }
        )
    except Exception:
        return RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)


if __name__ == '__main__':
    uvicorn.run(app, host='127.0.0.1', port=8000)
