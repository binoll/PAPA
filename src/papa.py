"""
This module searches for a document for plagiarism.
"""

from typing import List, Callable, Union, Any
from fuzzywuzzy import fuzz
from elasticsearch import Elasticsearch
from elasticsearch_dsl import Search, Document, Text, Keyword
from src.fingerprint import fingerprints, report, create_report

K = 10
T = 13
NAME_IN = 'papa'


class Article(Document):
    """
    Represents an article document for Elasticsearch.
    """

    index_name = Keyword()
    document_name = Keyword()
    author = Keyword()
    subject = Keyword()
    work_type = Keyword()
    task_num = Keyword()
    text = Text()
    tokens = Text()
    token_string = Text()
    fingerprint = Text()

    class Index:
        """
        Indexes all article documents.
        """

        name = NAME_IN
        settings = {
            'number_of_shards': 4,
            'number_of_replicas': 4
        }
        blocks = {
            'read_only_allow_delete': True
        }


class PAPA:
    """
    A class to manage and compare articles in an Elasticsearch index.

    This class provides functionality to create and clean Elasticsearch indices,
    add documents, and compare document similarities using tokenization and fingerprinting.

    Attributes:
        es (Elasticsearch): Elasticsearch instance.
        tokenizer (Callable[[str, str], List[str]]):
            Function that takes a string and returns a list of tokens.
        tokens_file_content (str): Token file content.
    """

    def __init__(self, es: Elasticsearch,
                 token_function: Callable,
                 token_file_content: str):
        """
        Initializes the PAPA class.

        Args:
            es (Elasticsearch): Elasticsearch instance.
            token_function (Callable[[str, str], List[str]]):
                Function that takes a string and returns a list of tokens.
            token_file_content (str): Token file content.
        """
        self.es = es
        self.tokenizer = token_function
        self.tokens_file_content = token_file_content

    def create_index(self) -> None:
        """
        Creates an Elasticsearch index if it does not already exist.
        """

        if self.es.indices.exists(index=NAME_IN):
            return

        Article.init()
        new_index = Article()
        new_index.save()

    def clean_index(self) -> None:
        """
        Deletes and recreates the Elasticsearch index.
        """

        if self.es.indices.exists(index=NAME_IN):
            self.es.indices.delete(index=NAME_IN)
            self.create_index()

    def process_tokens(self, file_name: str, file_content: Union[str, List[str]]) \
            -> None | List[str | List[str] | Any]:
        """
        Processes the tokens from the file content.

        Args:
            file_name (str): Token file name.
            file_content (Union[str, List[str]]): Token file content.

        Returns:
            Tuple[str, str, List[str], List[str], str, Any]: Tuple containing document name,
            buffer split parts, tokens, token string, and fingerprints.
        """

        tokens = self.tokenizer(file_content, self.tokens_file_content)
        token_string = ''.join([x[0] for x in tokens])
        fingerprint = fingerprints(tokens, int(K), int(T))

        buf = file_name.split('/')
        document_name = buf[-1]
        buf = document_name.split('_')

        if len(buf) != 4:
            return [None, None, None, None, None, None, None]

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
        article.fingerprint = fingerprint

        return [article, author, document_name, buf, tokens, token_string, fingerprint]

    def add(self, file_name: str, file_content: Union[str, List[str]]) -> bool:
        """
        Adds a document to the Elasticsearch index.

        Args:
            file_name (str): File name.
            file_content (Union[str, List[str]]): File content.

        Returns:
            bool: True if the document was added, False otherwise.
        """

        file_content = file_content if isinstance(file_content, list) else file_content.split('\n')
        article = self.process_tokens(file_name, file_content)

        if article == [None, None, None, None, None, None, None]:
            return False

        article[0].save()
        self.es.indices.refresh()
        return True

    def papa(self, file_content: Union[str, List[str]], file_name: str, source: str) \
            -> Union[List[str], dict[str, Union[List[str], List[List[Any]], List[Any]]]]:
        """
        Compares the given document with other documents
            in the index and returns similarity reports.

        Args:
            file_content (Union[str, List[str]]): File content to compare.
            file_name (str): File name.
            source (str): Source identifier for comparison.

        Returns:
            Union[List[str], dict[str, Union[List[str], List[List[Any]], List[Any]]]]:
            Similarity reports or an error message.
        """

        reports = []
        results = []
        dst_names = []

        (article, author, document_name, buf,
         tokens, token_string, fingerprint) = self.process_tokens(file_name, file_content)

        if [article, author, document_name, buf, tokens,
            token_string, fingerprint] == [None, None, None, None, None, None, None]:
            return ['Имя файла не верно!']

        new_hash_fingerprints = list(x[0] for x in fingerprint)

        if source.lower() == 'all':
            result = Search(using=self.es, index=NAME_IN).query('match', index_name=NAME_IN)
        else:
            buf = source.split('_')
            if len(buf) == 1:
                result = (Search(using=self.es, index=NAME_IN)
                          .query('match', subject=buf[0]))
            elif len(buf) == 2:
                result = (Search(using=self.es, index=NAME_IN)
                          .query('match', subject=buf[0])
                          .query('match', work_type=buf[1]))
            elif len(buf) == 3:
                result = (Search(using=self.es, index=NAME_IN)
                          .query('match', subject=buf[0])
                          .query('match', work_type=buf[1])
                          .query('match', task_num=buf[2]))
            else:
                return ['Неправильный формат файла!']

        self.es.indices.refresh(index=NAME_IN)
        response = result.execute()

        if response.hits.total.value == 0:
            return ['Совпадений не было найдено!']

        for hit in result.scan():
            if hit.author == author and hit.document_name == document_name:
                return ['Этот документ уже есть в базе!']

            a = fuzz.ratio(token_string, hit.token_string)
            old_hash_fingerprints = list(x[0] for x in hit.fingerprint)
            intersection = list(set(new_hash_fingerprints).intersection(set(old_hash_fingerprints)))
            token_distance = len(intersection) / max(len(set(old_hash_fingerprints)),
                                                     len(set(new_hash_fingerprints))) * 100
            token_distance = round(token_distance, 2)
            reports.append((a, token_distance,
                            report(fingerprint, hit.fingerprint),
                            hit.document_name,
                            list(hit.text)))

        reports.sort(key=lambda x: x[0], reverse=True)

        for item in reports[:5]:
            results.append(
                f'Сходство по Левенштейну - {item[0]} %, по отпечаткам -  {item[1]} %.')
            tr = create_report(item[2], document_name, item[3])

            if tr is not None:
                # results.extend(tr)
                dst_names.append(item[3])

        return {
            'diff': results,
            'dst_name': dst_names
        }

    def get_field_values(self, field: str) -> List[str]:
        """
        Retrieves distinct values of a specified field from the Elasticsearch index.

        Args:
            field (str): Field name to retrieve values for.

        Returns:
            List[str]: Sorted list of distinct field values.
        """

        data = self.es.sql.query(body={'query': f'SELECT {field} FROM {NAME_IN}'})
        result = list({v[0] for v in data['rows'] if v[0]})
        result.sort()
        return result
