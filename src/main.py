import uvicorn

from fastapi import FastAPI, Depends, Request, HTTPException, UploadFile, File, Form, status
from fastapi.responses import RedirectResponse, JSONResponse
from fastapi.security import OAuth2PasswordRequestForm
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from elasticsearch import Elasticsearch
from elasticsearch_dsl import connections
from loguru import logger
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

es = Elasticsearch([{'host': HOST, 'port': ES_PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': ES_PORT, 'scheme': 'http'}])
result_state = ResultsState()

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


async def render_template_page(template_name: str, request: Request, context: dict | None):
    try:
        if context is None:
            context = {}

        context.update({'request': request, 'status': status.HTTP_200_OK})
        return templates.TemplateResponse(template_name, context)
    except Exception as e:
        logger.error(f'Error rendering template {template_name}: {e}')
        raise HTTPException(status_code=status.HTTP_500_INTERNAL_SERVER_ERROR, detail='Internal Server Error')


def set_access_token_cookie(response: RedirectResponse, access_token: str):
    response.set_cookie(
        key='access_token',
        value=access_token,
        httponly=True,
        secure=True,
        max_age=3600,
        samesite='lax'
    )


@app.exception_handler(HTTPException)
async def http_exception_handler(request: Request, exc: HTTPException):
    if exc.status_code == status.HTTP_401_UNAUTHORIZED:
        return await render_template_page('unauth.html', request, None)
    return await request.app.default_exception_handler(request, exc)


@app.get('/unauthorized')
async def unauthorized_page(request: Request):
    return await render_template_page('unauth.html', request, None)


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
                detail='Invalid credentials'
            )

        access_token = await auth_backend.get_strategy().write_token(user)
        response = RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)
        set_access_token_cookie(response, access_token)

        return response
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='Invalid credentials'
        )


@app.get('/login')
async def login_page(request: Request):
    return await render_template_page('login.html', request, None)


@app.get('/register')
async def register_page(request: Request):
    return await render_template_page('register.html', request, None)


@app.post('/register')
async def register_user(
        request: Request,
        username: str = Form(...),
        password: str = Form(...),
        confirm_password: str = Form(...),
        user_manager: UserManager = Depends(get_user_manager)
):
    context = {}

    if password != confirm_password:
        context['error'] = 'Passwords do not match!'
        return await render_template_page('register.html', request, context)

    user_exists = await user_manager.user_exists(username)

    if user_exists:
        context['error'] = 'User already exists!'
        return await render_template_page('register.html', request, context)

    await user_manager.create_user(username=username, password=password)
    return RedirectResponse(url='/login', status_code=status.HTTP_303_SEE_OTHER)


@app.get('/')
async def home_page(request: Request,
                    results: List[str] = Depends(result_state.get_results),
                    user: UserRead = Depends(current_active_user)):
    try:
        if not results:
            results = ['Пока пусто...']

        subjects = MODEL.get_field_values('subject')
        work_types = MODEL.get_field_values('work_type')
        task_nums = MODEL.get_field_values('task_num')

        context = {
            'subjects': subjects,
            'work_types': work_types,
            'task_nums': task_nums,
            'results': results
        }

        return await render_template_page('main.html', request, context)
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )


@app.get('/add')
async def add_page(
        request: Request,
        user: UserRead = Depends(current_active_user)
):
    context = {}
    return await render_template_page('add.html', request, context)


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
                f'Файл \"{file.filename}\" похож на \"{results["dst_name"][0]}\'',
                f'{results["diff"][0]}'
            ]

        result_state.set_results(results or ['Пока пусто...'])

        return RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)

    except Exception as e:
        result_state.set_results([f'Error: {e}'])
        return RedirectResponse(url='/', status_code=status.HTTP_303_SEE_OTHER)


if __name__ == '__main__':
    uvicorn.run(app, host=HOST, port=UVICORN_PORT)
