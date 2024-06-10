"""
The module searches for a document for plagiarism.
"""

from typing import List, Callable, Union, Any
from fuzzywuzzy import fuzz
from elasticsearch import Elasticsearch
from elasticsearch_dsl import Search, Document, Text, Keyword

import fingerprint

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
    fingerprints = Text()

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
                 token_function: Callable[[str, str], List[str]],
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
            -> list[None] | list[str | list[str] | Any]:
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
        fingerprints = fingerprint.fingerprints(tokens, int(K), int(T))

        buf = file_name.split('/')
        document_name = buf[-1]
        buf = document_name.split('_')

        if len(buf) != 4:
            return [None, None, None, None, None, None]

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

        return [author, document_name, buf, tokens, token_string, fingerprints]

    def add(self, file_name: str, file_content: Union[str, List[str]]) -> None:
        """
        Adds a document to the Elasticsearch index.

        Args:
            file_name (str): File name.
            file_content (Union[str, List[str]]): File content.
        """
        file_content = file_content if isinstance(file_content, list) else file_content.split('\n')
        self.process_tokens(file_name, file_content)
        self.es.indices.refresh()

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
        (author, document_name, buf,
         tokens, token_string, fingerprints) = self.process_tokens(file_name, file_content)

        if [author, document_name, buf, tokens,
                token_string, fingerprints] == [None, None, None, None, None, None]:
            return ['File name is incorrect!']

        new_hash_fingerprints = list(x[0] for x in fingerprints)

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
                return ['Incorrect source!']

        self.es.indices.refresh(index=NAME_IN)
        response = result.execute()
        reports = []

        if response.hits.total.value == 0:
            return ['ES have no docs to compare!']

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
            reports.append((a, token_distance,
                            fingerprint.report(fingerprints, hit.fingerprints),
                            hit.document_name,
                            list(hit.text)))

        reports.sort(key=lambda x: x[0], reverse=True)
        results = []
        dst_names = []
        similar_proc = []

        for report in reports[:5]:
            results.append(
                f'Сходство  {document_name} с документом {report[3]} '
                f'по Левенштейну - {report[0]} %, по отпечаткам -  {report[1]} %.')
            tr = fingerprint.print_report(report[2], document_name, report[3])

            if tr is not None:
                results.extend(tr)
                dst_names.append(report[3])
                similar_proc.append(f'{report[0]}%/{report[1]}%')

        return {
            'diff': results,
            'dst_code': [r[-1] for r in reports[:5]],
            'dst_name': dst_names,
            'sims': similar_proc
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
