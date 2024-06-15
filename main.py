import uvicorn

from typing import Optional, List
from pathlib import Path
from fastapi import FastAPI, Request, File, UploadFile, Form, HTTPException, Depends
from fastapi.responses import JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from starlette import status
from werkzeug.utils import secure_filename
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from src import mpi, papa
from loguru import logger

from src.models import ResultsState

HOST = '127.0.0.1'
PORT = 9200
TOKENS_JSON_PATH = 'src/tokens/mpi.json'

es = Elasticsearch([{'host': HOST, 'port': PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': PORT, 'scheme': 'http'}])

logger = logger.opt(colors=True)

app = FastAPI()
templates = Jinja2Templates(directory='web/html')
app.mount('/web', StaticFiles(directory='web'), name='web')

results_store = []

with open(TOKENS_JSON_PATH, 'r', encoding='utf-8') as tokens:
    model = papa.PAPA(es, mpi.tokenizer, tokens.read())
    model.create_index()

results_state = ResultsState()


@app.get('/')
async def home(request: Request, results: List[str] = Depends(results_state.get_results)) -> templates.TemplateResponse:
    try:
        if not results:
            results = ['Пока пусто...']

        subjects = model.get_field_values('subject')
        work_types = model.get_field_values('work_type')
        task_nums = model.get_field_values('task_num')

        return templates.TemplateResponse(
            'index.html', {
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
                content={'message': f'Файлы \"{filenames_failed_str}\" не загружены! Неправильный формат файла!'}
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
                f"Документ \"{file.filename}\" похож на {results['dst_name'][0]}",
                f"{results['diff'][0]}",
            ]

        results_state.set_results(results or ['Пока пусто...'])
        return RedirectResponse(url=app.url_path_for('home'), status_code=status.HTTP_303_SEE_OTHER)

    except Exception as e:
        results_state.set_results([str(f'Error: {e}')])
        return RedirectResponse(url=app.url_path_for('home'), status_code=status.HTTP_303_SEE_OTHER)


if __name__ == '__main__':
    uvicorn.run(app, host='127.0.0.1', port=8000)
