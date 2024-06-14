"""
Tasks models
"""
from pydantic import BaseModel
from typing import Optional, List
from sqlalchemy import Column, Integer, Boolean, Text, Enum
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()


class User(BaseModel):
    username: str
    password: str
    email: str
    full_name: Optional[str] = None


class UserInDB(User):
    hashed_password: str


class Token(BaseModel):
    access_token: str
    token_type: str


class ResultsState:
    def __init__(self):
        self.results = []

    def set_results(self, results: List[str]):
        self.results = results

    def get_results(self) -> List[str]:
        return self.results
