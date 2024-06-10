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

# OAuth2 setup
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

users_db = {
    "user": {
        "username": "user",
        "full_name": "John Doe",
        "email": "user@example.com",
        "hashed_password": "fakehashedpassword",
        "disabled": False,
    }
}


def fake_hash_password(password: str):
    return "fakehashed" + password


def get_user(db, username: str):
    if username in db:
        user_dict = db[username]
        return user_dict
    return None


def fake_decode_token(token):
    user = get_user(users_db, token)
    return user


async def get_current_user(token: str = Depends(oauth2_scheme)):
    user = fake_decode_token(token)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid authentication credentials",
            headers={"WWW-Authenticate": "Bearer"},
        )
    return user


@app.post("/token")
async def login(form_data: OAuth2PasswordRequestForm = Depends()):
    user_dict = users_db.get(form_data.username)

    if not user_dict:
        raise HTTPException(status_code=400, detail="Incorrect username or password")
    user = user_dict
    hashed_password = fake_hash_password(form_data.password)
    if not hashed_password == user['hashed_password']:
        raise HTTPException(status_code=400, detail="Incorrect username or password")

    response = RedirectResponse(url="/")
    return response


@app.get('/')
async def home(request: Request,
               results=None) -> templates.TemplateResponse:
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
        raise HTTPException(status_code=500, detail=str(e))


@app.get('/login')
async def login_page(request: Request):
    return templates.TemplateResponse('login.html', {'request': request})


@app.get('/add')
async def add_file_page(request: Request, current_user: dict = Depends(get_current_user)):
    return templates.TemplateResponse('add.html', {'request': request, 'user': current_user})


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

        if not results:
            return RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)

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


if __name__ == '__main__':
    uvicorn.run(app, host='0.0.0.0', port=8000)
