from typing import List, Callable, Union
from fuzzywuzzy import fuzz
from src import fingerprint
from elasticsearch import Elasticsearch
from elasticsearch_dsl import Search, Document, Text, Keyword

K = 10
T = 13
NAME_IN = 'papa'


class Article(Document):
    index_name = Keyword()
    document_name = Keyword()
    author = Keyword()
    subject = Keyword()
    work_type = Keyword()
    task_num = Keyword()
    text = Text()
    tokens = Text()
    token_string = Text()
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
    def __init__(self, es: Elasticsearch, token_function: Callable[[str, str], List[str]], token_file_content: str):
        self.es = es
        self.tokenizer = token_function
        self.tokensFileContent = token_file_content

    def create_index(self) -> None:
        if self.es.indices.exists(index=NAME_IN):
            return

        Article.init()
        new_index = Article()
        new_index.save()

    def clean_index(self) -> None:
        if self.es.indices.exists(index=NAME_IN):
            self.es.indices.delete(index=NAME_IN)
            self.create_index()

    def add(self, filename: str, file_content: Union[str, List[str]]) -> None:
        text = file_content if isinstance(file_content, list) else file_content.split('\n')

        tokens = self.tokenizer(text, self.tokensFileContent)
        token_string = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(K), int(T))

        buf = filename.split('/')
        document_name = buf[-1]
        buf = document_name.split('_')

        if len(buf) != 4:
            print(filename, ' - filename error')
            exit()

        author = buf[0]
        subject = buf[1]
        work_type = buf[2]
        task_num = buf[3].split('.')[0]

        article = Article()
        article.index_name = NAME_IN
        article.document_name = document_name
        article.author = author
        article.subject = subject
        article.work_type = work_type
        article.task_num = task_num
        article.text = text
        article.tokens = tokens
        article.token_string = token_string
        article.fingerprints = fingerprints
        article.save()

        self.es.indices.refresh()

    def papa(self, file_content: Union[str, List[str]], file_name: str, source: str) -> List[str]:
        tokens = self.tokenizer(file_content, self.tokensFileContent)
        token_string = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(K), int(T))

        buf = file_name.split('/')
        document_name = buf[-1]
        buf = document_name.split('_')

        if len(buf) != 4:
            return ['File name error']

        author = buf[0]
        subject = buf[1]
        work_type = buf[2]
        task_num = buf[3].split('.')[0]

        article = Article()
        article.index_name = NAME_IN
        article.document_name = document_name
        article.author = author
        article.subject = subject
        article.work_type = work_type
        article.task_num = task_num
        article.text = file_content
        article.tokens = tokens
        article.token_string = token_string
        article.fingerprints = fingerprints

        new_hash_fingerprints = list(x[0] for x in fingerprints)

        if source.lower() == 'all':
            result = Search(using=self.es, index=NAME_IN).query('match', index_name=NAME_IN)
        else:
            buf = source.split('_')
            if len(buf) == 1:
                result = Search(using=self.es, index=NAME_IN).query('match', subject=buf[0])
            elif len(buf) == 2:
                result = Search(using=self.es, index=NAME_IN).query('match', subject=buf[0]).query('match',
                                                                                                   work_type=buf[1])
            elif len(buf) == 3:
                result = Search(using=self.es, index=NAME_IN).query('match', subject=buf[0]).query('match',
                                                                                                   work_type=buf[
                                                                                                       1]).query(
                    'match', task_num=buf[2])
            else:
                return ['Incorrect source!']

        self.es.indices.refresh(index=NAME_IN)

        response = result.execute()
        reports = []

        if response.hits.total.value == 0:
            return ['ES have no docs to compare!']
        else:
            for hit in result.scan():
                if hit.author == author:
                    continue
                if hit.document_name == document_name:
                    continue
                a = fuzz.ratio(token_string, hit.token_string)
                old_hash_fingerprints = list(x[0] for x in hit.fingerprints)
                intersection = list(set(new_hash_fingerprints).intersection(set(old_hash_fingerprints)))
                token_distance = len(intersection) / max(len(set(old_hash_fingerprints)),
                                                         len(set(new_hash_fingerprints))) * 100
                reports.append((a, token_distance, fingerprint.report(fingerprints, hit.fingerprints), hit.document_name,
                                list(hit.text)))

        reports.sort(key=lambda x: x[0], reverse=True)

        results = []
        dst_names = []
        similars_proc = []

        for report in reports[:5]:
            results.append(
                f'Сходство  {document_name} с документом {report[3]} по Левенштейну - {report[0]} %, по отпечаткам -  {report[1]} %.')
            tr = fingerprint.print_report(report[2], document_name, report[3])

            if tr is not None:
                results.extend(tr)
                dst_names.append(report[3])
                similars_proc.append(f'{report[0]}%/{report[1]}%')

        return results

    def get_field_values(self, field: str) -> List[str]:
        data = self.es.sql.query(body={'query': f'SELECT {field} FROM {NAME_IN}'})
        result = list({v[0] for v in data['rows'] if v[0]})
        result.sort()

        return result
