import os
from fuzzywuzzy import fuzz
from src import fingerprint
from elasticsearch_dsl import Search, Document, Text, Keyword

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
        name = 'papa'
        settings = {
            'number_of_shards': 4,
            'number_of_replicas': 4
        }

        blocks = {
            'read_only_allow_delete': True
        }


class PAPA:
    def __init__(self, es, tokenizer_function, token_file_content):
        self.es = es
        self.tokenizer = tokenizer_function
        self.tokens_file_content = token_file_content

    def create(self):
        """
        Создание пустого индекса с именем os.environ.get("NAME_IN")

        Returns:
            None
        """
        index_name = os.environ.get('NAME_IN')
        if not index_name:
            raise ValueError('Переменная окружения NAME_IN не установлена.')

        if self.es.indices.exists(index=index_name):
            self.es.indices.delete(index=index_name)

        Article.init()
        new_index = Article()
        new_index.save()

    def add(self, filename, file_content):
        """
        Добавление в базу

        Args:
            filename (str): Имя файла.
            file_content (Union[str, List[str]]): Содержимое файла.

        Returns:
            None
        """
        text = file_content if isinstance(file_content, list) else file_content.split('\n')

        tokens = self.tokenizer(text, self.tokens_file_content)
        tokenstring = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(os.environ.get('K')), int(os.environ.get('T')))

        buf = filename.split('/')
        docname = buf[-1]
        buf = docname.split('_')
        if len(buf) != 4:
            print(filename, ' - filename error')
            exit()
        author = buf[0]
        subject = buf[1]
        work_type = buf[2]
        task_num = buf[3].split('.')[0]

        article = Article()
        article.index_name = os.environ.get('NAME_IN')
        article.docname = docname
        article.author = author
        article.subject = subject
        article.work_type = work_type
        article.task_num = task_num
        article.text = text
        article.tokens = tokens
        article.tokenstring = tokenstring
        article.fingerprints = fingerprints
        article.save()

        self.es.indices.refresh()

    def papa(self, file_content, file_name, source):
        """
        Проверка кода с базой

        Args:
            file_content (str): Содержимое файла для проверки.
            file_name (str): Имя файла для проверки.
            source (str): Источник для проверки.

        Returns:
            dict: Содержит результаты сравнения.
        """

        tokens = self.tokenizer(file_content, self.tokens_file_content)
        tokenstring = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(os.environ.get('K')), int(os.environ.get('T')))

        buf = file_name.split('/')
        docname = buf[-1]
        buf = docname.split('_')
        if len(buf) != 4:
            return {'message': 'File name error'}

        author = buf[0]

        new_hash_fingerprints = [x[0] for x in fingerprints]

        if source.lower() == 'all':
            result = Search(using=self.es, index=os.environ.get('NAME_IN')) \
                .query('match', index_name=os.environ.get('NAME_IN'))
        else:
            buf = source.split('_')
            if len(buf) == 1:
                result = Search(using=self.es, index=os.environ.get('NAME_IN')) \
                    .query('match', subject=buf[0])
            elif len(buf) == 2:
                result = Search(using=self.es, index=os.environ.get('NAME_IN')) \
                    .query('match', subject=buf[0]) \
                    .query('match', work_type=buf[1])
            elif len(buf) == 3:
                result = Search(using=self.es, index=os.environ.get('NAME_IN')) \
                    .query('match', subject=buf[0]) \
                    .query('match', work_type=buf[1]) \
                    .query('match', task_num=buf[2])
            else:
                return {'message': 'Incorrect source!'}

        self.es.indices.refresh(index=os.environ.get('NAME_IN'))

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
        similars_proc = []

        for report in reports[:5]:
            tr = fingerprint.print_report(report[2], docname, report[3])
            if tr:
                results.append(tr)
                dst_names.append(report[3])
                similars_proc.append(f"{report[0]}%/{report[1]}%")

        return {
            'diff': results,
            'dst_code': [r[-1] for r in reports[:5]],
            'dst_name': dst_names,
            'sims': similars_proc
        }

    def get_field_values(self, field):
        """
        Получение значений поля

        Args:
            field (str): Поле, значения которого нужно получить.

        Returns:
            list: Список значений поля.
        """
        data = self.es.sql.query(
            body={'query': f'SELECT {field} FROM papa'}
        )
        result = sorted({v[0] for v in data['rows'] if v[0]})
        return result
