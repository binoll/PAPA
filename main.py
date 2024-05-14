import os
import uvicorn

from fastapi import FastAPI, Request, File, UploadFile, Form
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from werkzeug.utils import secure_filename
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from src.models import PAPA
from src.tokenizers import mpi

HOST = '0.0.0.0'
PORT = 9200
tokens_json_path = 'src/tokens/mpi.json'

es = Elasticsearch([{'host': HOST, 'port': PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': PORT, 'scheme': 'http'}])

with open(tokens_json_path, 'r', encoding='utf8') as tf:
    model = PAPA.PAPA(es, mpi.tokenizer, tf.read())

app = FastAPI()
templates = Jinja2Templates(directory='web/html')
app.mount('/web', StaticFiles(directory='web'), name='web')


@app.get('/')
async def main_page(request: Request):
    try:
        subjects = model.get_field_values('subject')
        work_types = model.get_field_values('work_type')
        task_nums = model.get_field_values('task_num')
        return templates.TemplateResponse('index.html',
                                          {'request': request, 'subjects': subjects, 'work_types': work_types,
                                           'task_nums': task_nums})
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/api/create')
async def create_index():
    try:
        model.create()
        return JSONResponse(content={'code': 201, 'message': 'Successfully created!'})
    except Exception as e:
        return JSONResponse(status_code=400, content={'message': f'Error creating index: {str(e)}'})


@app.post('/api/add_file')
async def add_document(file: UploadFile = File(...)):
    try:
        if not file:
            return JSONResponse(status_code=400, content={'message': 'File not loaded!'})

        if file.filename == '':
            return JSONResponse(status_code=400, content={'message': 'File empty!'})

        filename = secure_filename(file.filename)
        file_content = await file.read()
        model.add(filename, file_content)
        return JSONResponse(content={'code': 200, 'message': 'File Loaded!'})
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/api/papa')
async def papa(file: UploadFile = File(...), src_filename: str = Form(...)):
    try:
        if not file:
            return JSONResponse(status_code=400, content={'message': 'File not loaded!'})

        if file.filename == '':
            return JSONResponse(status_code=400, content={'message': 'File empty!'})

        file_content = await file.read()
        result = model.papa(file_content, file.filename, src_filename)

        if 'message' in result:
            return JSONResponse(content=result)

        return JSONResponse(content={
            'src_code': file_content.decode().split('\n'),
            'dst_code': list(result['dst_code']),
            'diff': result['diff'],
            'dst_name': result['dst_name'],
            'sims': result['sims']
        })
    except Exception as e:
        return JSONResponse(status_code=500, content={'message': str(e)})


@app.post('/api/field/')
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
