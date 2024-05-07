import os
import uvicorn

from fastapi import Request
from fastapi import FastAPI, File, UploadFile, Form
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from dotenv import load_dotenv
from web.models.PAPA import PAPA
from web.controllers.tokenizers.mpi import tokenizer

from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates

from werkzeug.utils import secure_filename

app = FastAPI()
templates = Jinja2Templates(directory='web/static/html')
app.mount('/web/static', StaticFiles(directory='web/static'), name='static')

HOST = '0.0.0.0'
PORT = 9200

dotenv_path = os.path.join(os.path.dirname(__file__), '.env')
if os.path.exists(dotenv_path):
    load_dotenv(dotenv_path)

es = Elasticsearch([{'host': HOST, 'port': PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': PORT, 'scheme': 'http'}])

tokens_json_path = os.path.join(os.path.dirname(__file__), 'data', 'tokens', 'mpi.json')
with open(tokens_json_path, 'r', encoding='utf8') as tf:
    model = PAPA(es, tokenizer, tf.read())


@app.get('/')
async def main_page(request: Request):
    subjects = model.get_field_values('subject')
    work_types = model.get_field_values('work_type')
    task_nums = model.get_field_values('task_num')

    return templates.TemplateResponse('index.html', {'request': request, 'subjects': subjects,
                                                     'work_types': work_types, 'task_nums': task_nums})


@app.post('/api/create')
async def create_index():
    try:
        # Create index if not exist
        model.create()
        return JSONResponse(content={'code': 201, 'message': 'Successfully created!'})

    except Exception as e:
        print(e)
        return JSONResponse(content={'code': 400, 'message': 'Error on creating index!'})


@app.post('/api/add_file')
async def add_document(file: UploadFile = File(...)):
    if not file:
        return JSONResponse(content={'code': 400, 'message': 'File not loaded!'})

    if file.filename == '':
        return JSONResponse(content={'code': 400, 'message': 'File empty!'})

    filename = secure_filename(file.filename)
    file_content = await file.read()

    model.add(filename, file_content)

    return JSONResponse(content={'code': 200, 'message': 'File Loaded!'})


@app.post('/api/papa')
async def papa(file: UploadFile = File(...), src_filename: str = Form(...)):
    if not file:
        return JSONResponse(content={'code': 400, 'message': 'File not loaded!'})

    if file.filename == '':
        return JSONResponse(content={'code': 400, 'message': 'File empty!'})

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


@app.post('/api/field/')
async def field_list(name: str = Form(...)):
    data = model.get_field_values(name)

    return JSONResponse(content={'result': data})


@app.get('/add')
async def add_file_page(request: Request):
    return templates.TemplateResponse('add.html', {'request': request})


if __name__ == '__main__':
    uvicorn.run(app, host='0.0.0.0', port=int(os.environ.get('PORT_WEB', 8000)))
