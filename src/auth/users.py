from typing import Optional
from fastapi import Depends, HTTPException, status
from fastapi.security import OAuth2PasswordRequestForm
from fastapi_users import BaseUserManager
from fastapi_users.authentication import (
    AuthenticationBackend,
    JWTStrategy,
    CookieTransport,
)
from sqlalchemy.future import select
from sqlalchemy.ext.asyncio import AsyncSession

from auth.models import User
from auth.database import get_async_session
from auth.crypt import hash_password, verify_password

SECRET_KEY = 'SECRET'


class UserManager(BaseUserManager[User, int]):
    reset_password_token_secret = SECRET_KEY
    verification_token_secret = SECRET_KEY

    def __init__(self, session: AsyncSession):
        super().__init__(session)
        self.session = session

    def parse_id(self, user_id: str) -> int:
        try:
            return int(user_id)
        except ValueError:
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Invalid user ID format")

    @staticmethod
    def get_id(user: User) -> str:
        return str(user.id)

    async def get(self, id: int) -> Optional[User]:
        query = select(User).where(User.id == id)
        result = await self.session.execute(query)
        user = result.scalars().first()
        if user is None:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
        return user

    async def create_user(self, username: str, password: str) -> User:
        hashed_password = hash_password(password)
        user = User(username=username, hashed_password=hashed_password)
        try:
            self.session.add(user)
            await self.session.commit()
            return user
        except Exception as e:
            await self.session.rollback()
            raise e

    async def delete_user(self, username: str) -> None:
        try:
            query = select(User).where(User.username == username)
            result = await self.session.execute(query)
            user = result.scalars().first()

            if user:
                await self.session.delete(user)
                await self.session.commit()
            else:
                raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
        except Exception as e:
            await self.session.rollback()
            raise e

    async def update_password(self, username: str, new_password: str) -> None:
        hashed_password = hash_password(new_password)
        try:
            query = select(User).where(User.username == username)
            result = await self.session.execute(query)
            user = result.scalars().first()
            if user:
                user.hashed_password = hashed_password
                self.session.add(user)
                await self.session.commit()
            else:
                raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
        except Exception as e:
            await self.session.rollback()
            raise e

    async def authenticate(self, credentials: OAuth2PasswordRequestForm) -> Optional[User]:
        try:
            query = select(User).where(User.username == credentials.username)
            result = await self.session.execute(query)
            user = result.scalars().first()
            if user and verify_password(credentials.password, user.hashed_password):
                return user
        except Exception as e:
            raise e
        return None

    async def user_exists(self, username: str) -> bool:
        try:
            query = select(User).where(User.username == username)
            result = await self.session.execute(query)
            user = result.scalars().first()
            return user is not None
        except Exception as e:
            raise e


async def get_user_manager(session: AsyncSession = Depends(get_async_session)) -> UserManager:
    return UserManager(session)


def get_jwt_strategy() -> JWTStrategy:
    return JWTStrategy(secret=SECRET_KEY, lifetime_seconds=60)


cookie_transport = CookieTransport(cookie_name='access_token', cookie_max_age=3600)

auth_backend = AuthenticationBackend(
    name='jwt',
    transport=cookie_transport,
    get_strategy=get_jwt_strategy,
)
