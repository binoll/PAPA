import os

from elasticsearch import Elasticsearch
from fuzzywuzzy import fuzz
from src import fingerprint
from elasticsearch_dsl import Search, Document, Text, Keyword, function

NAME_IN = 'papa'


class Article(Document):
    index_name = Keyword()
    docname = Keyword()
    author = Keyword()
    subject = Keyword()
    work_type = Keyword()
    task_num = Keyword()
    text = Text()
    tokens = Text()
    tokenstring = Text()
    fingerprints = Text()

    class Index:
        name = NAME_IN
        settings = {
            'number_of_shards': 4,
            'number_of_replicas': 4
        }
        blocks = {
            'read_only_allow_delete': True
        }


class PAPA:
    def __init__(self, es: Elasticsearch, tokenizer_function: function, token_file_content: str) -> None:
        """
        Инициализация класса PAPA

        Args:
            es (Elasticsearch): Экземпляр Elasticsearch.
            tokenizer_function (function): Функция токенизации.
            token_file_content (str): Содержимое файла токенов.
        """
        self.es = es
        self.tokenizer = tokenizer_function
        self.tokens_file_content = token_file_content

    def create(self) -> None:
        """
        Создание пустого индекса с именем os.environ.get("NAME_IN")

        Returns:
            None
        """
        index_name = os.environ.get(NAME_IN)
        if index_name:
            self.es.indices.delete(index=index_name)

        Article.init()
        new_index = Article()
        new_index.save()

    @staticmethod
    def process_file(filename: str, file_content: str | list, tokenizer: function, tokens_file_content: str) -> tuple:
        """
        Обрабатывает файл, выполняет токенизацию и генерирует отпечатки.

        Args:
            filename (str): Имя файла.
            file_content (str | list): Содержимое файла.
            tokenizer (function): Функция токенизации.
            tokens_file_content (str): Содержимое файла токенов.

        Returns:
            tuple: Кортеж, содержащий docname, author, subject, work_type, task_num, tokens, tokenstr и fingerprints.
        """
        text = file_content if isinstance(file_content, list) else file_content.split('\n')
        tokens = tokenizer(text, tokens_file_content)
        tokenstr = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(os.environ.get("K")), int(os.environ.get("T")))

        buf = filename.split('/')
        docname = buf[-1]
        buf = docname.split('_')
        if len(buf) != 4:
            raise ValueError(f"{filename} - filename error")

        author, subject, work_type, task_num = buf[0], buf[1], buf[2], buf[3].split('.')[0]

        return docname, author, subject, work_type, task_num, tokens, tokenstr, fingerprints

    def add(self, filename: str, file_content: str | list) -> None:
        """
        Добавление в базу

        Args:
            filename (str): Имя файла.
            file_content (str | list): Содержимое файла.

        Returns:
            None
        """
        docname, author, subject, work_type, task_num, tokens, tokenstr, fingerprints = self.process_file(
            filename, file_content, self.tokenizer, self.tokens_file_content
        )

        article = Article(
            index_name=os.environ.get(NAME_IN),
            docname=docname,
            author=author,
            subject=subject,
            work_type=work_type,
            task_num=task_num,
            text=file_content if isinstance(file_content, list) else file_content.split('\n'),
            tokens=tokens,
            tokenstring=tokenstr,
            fingerprints=fingerprints
        )
        article.save()
        self.es.indices.refresh()

    def papa(self, file_content: str | list, file_name: str, source: str) -> dict:
        """
        Проверка кода с базой

        Args:
            file_content (str | list): Содержимое файла.
            file_name (str): Имя файла.
            source (str): Источник.

        Returns:
            dict: Результат проверки.
        """
        docname, author, subject, work_type, task_num, tokens, tokenstring, fingerprints = self.process_file(
            file_name, file_content, self.tokenizer, self.tokens_file_content
        )

        new_hash_fingerprints = [x[0] for x in fingerprints]

        index_name = os.environ.get(NAME_IN)
        if not index_name:
            raise ValueError('Переменная окружения NAME_IN не установлена.')

        if source.lower() == 'all':
            result = Search(using=self.es, index=index_name).query('match', index_name=index_name)
        else:
            buf = source.split('_')
            result = Search(using=self.es, index=index_name)
            if len(buf) == 1:
                result = result.query('match', subject=buf[0])
            elif len(buf) == 2:
                result = result.query('match', subject=buf[0]).query('match', work_type=buf[1])
            elif len(buf) == 3:
                result = result.query('match', subject=buf[0]).query('match', work_type=buf[1]).query('match',
                                                                                                      task_num=buf[2])
            else:
                return {'message': 'Incorrect source!'}

        self.es.indices.refresh(index=index_name)

        response = result.execute()
        reports = []

        if response.hits.total.value == 0:
            return {'message': 'ES have no docs to compare!'}

        for hit in result.scan():
            if hit.author == author or hit.docname == docname:
                continue
            a = fuzz.ratio(tokenstring, hit.tokenstring)
            old_hash_fingerprints = [x[0] for x in hit.fingerprints]
            intersection = list(set(new_hash_fingerprints).intersection(set(old_hash_fingerprints)))
            token_distance = len(intersection) / max(len(set(old_hash_fingerprints)),
                                                     len(set(new_hash_fingerprints))) * 100
            reports.append(
                (a, token_distance, fingerprint.report(fingerprints, hit.fingerprints), hit.docname, list(hit.text)))

        reports.sort(key=lambda x: x[0], reverse=True)

        results = []
        dst_names = []
        similar_proc = []

        for report in reports[:5]:
            tr = fingerprint.print_report(report[2], docname, report[3])
            if tr:
                results.append(tr)
                dst_names.append(report[3])
                similar_proc.append(f"{report[0]}%/{report[1]}%")

        return {
            'diff': results,
            'dst_code': [r[-1] for r in reports[:5]],
            'dst_name': dst_names,
            'sims': similar_proc
        }

    def get_field_values(self, field: str) -> list:
        """
        Получить значения поля

        Args:
            field (str): Название поля.

        Returns:
            list: Список уникальных значений поля.
        """
        data = self.es.sql.query(body={'query': f'SELECT {field} FROM {NAME_IN}'})
        result = sorted({v[0] for v in data['rows'] if v[0]})
        return result
