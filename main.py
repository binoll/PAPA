import uvicorn

from typing import Optional, List
from pathlib import Path
from fastapi import File, UploadFile, Form, HTTPException, FastAPI, Request, status, Depends
from fastapi.responses import RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from werkzeug.utils import secure_filename
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from fastapi.responses import JSONResponse
from loguru import logger

from src import mpi, papa
from src.result import ResultsState

HOST = '127.0.0.1'
ES_PORT = 9200
UVICORN_PORT = 8000
TOKENS_JSON_PATH = 'src/tokens/mpi.json'
RESULT_STATE = ResultsState()

es = Elasticsearch([{'host': HOST, 'port': ES_PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': ES_PORT, 'scheme': 'http'}])

app = FastAPI()
templates = Jinja2Templates(directory='web/html')
app.mount('/web', StaticFiles(directory='web'), name='web')
logger = logger.opt(colors=True)

with open(TOKENS_JSON_PATH, 'r', encoding='utf-8') as tokens:
    model = papa.PAPA(es, mpi.tokenizer, tokens.read())
    model.create_index()


@app.get('/login')
async def login(request: Request):
    return templates.TemplateResponse(
        'login.html', {'request': request}
    )


@app.get('/register')
def register_route(request: Request):
    return templates.TemplateResponse(
        'register.html', {'request': request}
    )


@app.get('/')
async def home(request: Request,
               results: List[str] = Depends(RESULT_STATE.get_results)) \
        -> templates.TemplateResponse:
    try:
        if not results:
            results = ['Пока пусто...']

        subjects = model.get_field_values('subject')
        work_types = model.get_field_values('work_type')
        task_nums = model.get_field_values('task_num')

        return templates.TemplateResponse(
            'main.html', {
                'request': request,
                'subjects': subjects,
                'work_types': work_types,
                'task_nums': task_nums,
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


@app.post('/add_files')
async def add_files(files: List[UploadFile] = File(...)):
    filenames_failed = []

    try:
        if not files:
            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': 'Файлы не загружены!'}
            )

        for file in files:
            if file.filename == '':
                continue

            filename = Path(file.filename).name
            file_content = await file.read()
            result = model.add(filename, file_content.decode('utf-8'))

            if not result:
                filenames_failed.append(filename)

        if len(filenames_failed) != 0:
            filenames_failed_str = ', '.join(filenames_failed)

            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': f'Файлы \'{filenames_failed_str}\' не загружены! Неправильный формат файла!'}
            )

        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={'message': 'Все файлы успешно загружены!'}
        )
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            content={'message': str(e)}
        )


@app.post('/papa')
async def papa(file: UploadFile = File(...),
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

        file_content = await file.read()
        filename = secure_filename(file.filename)

        src_filename = 'all' if not subject else subject
        if work_type:
            src_filename += f'_{work_type}'
        if task_num:
            src_filename += f'_{task_num}'

        results = model.papa(file_content.decode('utf-8'), filename, src_filename)

        if isinstance(results, dict):
            results = [
                f'Файл \'{file.filename}\' похож на \'{results["dst_name"][0]}\'',
                f'{results["diff"][0]}'
            ]

        RESULT_STATE.set_results(results or ['Пока пусто...'])
        return RedirectResponse(url=app.url_path_for('home'), status_code=status.HTTP_303_SEE_OTHER)

    except Exception as e:
        RESULT_STATE.set_results([str(f'Error: {e}')])
        return RedirectResponse(url=app.url_path_for('home'), status_code=status.HTTP_303_SEE_OTHER)


if __name__ == '__main__':
    uvicorn.run(app, host=HOST, port=UVICORN_PORT)
