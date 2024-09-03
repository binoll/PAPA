import uvicorn

from fastapi import FastAPI, Depends, Request, HTTPException, UploadFile, File, Form, status
from fastapi.responses import RedirectResponse, JSONResponse
from fastapi.security import OAuth2PasswordRequestForm
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from werkzeug.utils import secure_filename
from contextlib import asynccontextmanager
from typing import List, Optional

from auth.database import Database
from auth.models import User
from auth.schemas import UserRead
from auth.users import auth_backend, get_user_manager, UserManager
from result import ResultsState
from app import papa, mpi
from fastapi_users import FastAPIUsers

HOST = '127.0.0.1'
ES_PORT = 9200
UVICORN_PORT = 8000
TOKENS_JSON_PATH = 'src/app/tokens/mpi.json'
RESULT_STATE = ResultsState()

es = Elasticsearch([{'host': HOST, 'port': ES_PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': ES_PORT, 'scheme': 'http'}])

with open(TOKENS_JSON_PATH, 'r', encoding='utf-8') as tokens:
    MODEL = papa.PAPA(es=es, token_function=mpi.tokenizer, token_file_content=tokens.read())
    MODEL.create_index()


@asynccontextmanager
async def lifespan(app: FastAPI):
    await Database.create_db_and_tables()
    yield


app = FastAPI(lifespan=lifespan)
app.mount('/web', StaticFiles(directory='src/web'), name='web')
templates = Jinja2Templates(directory='src/web/html')

fastapi_users = FastAPIUsers[User, int](get_user_manager, [auth_backend])
current_active_user = fastapi_users.current_user(active=True)


@app.post('/login')
async def login_user(
        form_data: OAuth2PasswordRequestForm = Depends(),
        user_manager: UserManager = Depends(get_user_manager)
):
    try:
        user = await user_manager.authenticate(form_data)
        if not user:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid credentials"
            )

        access_token = await auth_backend.get_strategy().write_token(user)

        response = RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)
        response.set_cookie(
            key='access_token',
            value=access_token,
            httponly=True,
            secure=True,
            max_age=3600,
            samesite='lax'
        )

        return response
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid credentials"
        )


@app.get('/login')
async def login_page(request: Request):
    return templates.TemplateResponse('login.html', {'request': request, 'status': status.HTTP_200_OK})


@app.get('/register')
async def register_page(request: Request):
    return templates.TemplateResponse('register.html', {'request': request, 'status': status.HTTP_200_OK})


@app.post('/register')
async def register_user(
        request: Request,
        username: str = Form(...),
        password: str = Form(...),
        confirm_password: str = Form(...),
        user_manager: UserManager = Depends(get_user_manager)
):
    if password != confirm_password:
        return templates.TemplateResponse(
            'register.html',
            {'request': request, 'error': 'Passwords do not match!'}
        )

    user_exists = await user_manager.user_exists(username)
    if user_exists:
        return templates.TemplateResponse(
            'register.html',
            {'request': request, 'error': 'User already exists!'}
        )

    await user_manager.create_user(username=username, password=password)
    return RedirectResponse(url='/login', status_code=status.HTTP_303_SEE_OTHER)


@app.get('/')
async def home_page(request: Request,
                    results: List[str] = Depends(RESULT_STATE.get_results),
                    user: UserRead = Depends(current_active_user)):
    try:
        if not results:
            results = ['Пока пусто...']

        subjects = MODEL.get_field_values('subject')
        work_types = MODEL.get_field_values('work_type')
        task_nums = MODEL.get_field_values('task_num')

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
async def add_page(request: Request,
                   user: UserRead = Depends(current_active_user)):
    return templates.TemplateResponse('add.html', {'request': request})


@app.post('/add')
async def add_files(files: List[UploadFile] = File(...),
                    user: UserRead = Depends(current_active_user)):
    filenames_failed = list()

    try:
        if not files:
            return JSONResponse(
                status_code=status.HTTP_400_BAD_REQUEST,
                content={'message': 'Файлы не загружены!'}
            )

        for file in files:
            if file.filename == '':
                continue

            filename = file.filename
            file_content = await file.read()
            result = MODEL.add(filename, file_content.decode('utf-8'))

            if not result:
                filenames_failed.append(filename)

        if filenames_failed:
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
               task_num: Optional[str] = Form(None),
               user: UserRead = Depends(current_active_user)):
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

        results = MODEL.papa(file_content.decode('utf-8'), filename, src_filename)

        if isinstance(results, dict):
            results = [
                f'Файл \"{file.filename}\" похож на \"{results["dst_name"][0]}\"',
                f'{results["diff"][0]}'
            ]

        RESULT_STATE.set_results(results or ['Пока пусто...'])

        return RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)

    except Exception as e:
        RESULT_STATE.set_results([f'Error: {e}'])
        return RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)


if __name__ == '__main__':
    uvicorn.run(app, host=HOST, port=UVICORN_PORT)
