import os

from sqlalchemy.ext.asyncio import AsyncSession, create_async_engine
from sqlalchemy.orm import sessionmaker
from auth.models import Base

DATABASE_DIR = 'databases'
DATABASE_URL = f'sqlite+aiosqlite:///{DATABASE_DIR}/database.db'


class Database:
    engine = create_async_engine(DATABASE_URL, echo=True)
    async_session_maker = sessionmaker(engine, class_=AsyncSession, expire_on_commit=False)

    @classmethod
    async def create_db_and_tables(cls):
        if not os.path.exists(DATABASE_DIR):
            os.makedirs(DATABASE_DIR)
        async with cls.engine.begin() as conn:
            await conn.run_sync(Base.metadata.create_all)

    @classmethod
    async def get_async_session(cls):
        async with cls.async_session_maker() as session:
            try:
                yield session
            except Exception as e:
                await session.rollback()
                raise e
            finally:
                await session.close()
