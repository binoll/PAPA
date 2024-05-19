from fuzzywuzzy import fuzz
from src import fingerprint
from elasticsearch_dsl import Search, Document, Text, Keyword

K = 10
T = 13
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
    def __init__(self, es, tokenzier_function, token_file_content):
        self.es = es
        self.tokenizer = tokenzier_function
        self.tokensFileContent = token_file_content

    def create(self):
        """
            Создание пустого индекса с именем 'NAME_IN'
        """
        if not self.es.indices.exists(index=NAME_IN):
            return

        Article.init()
        new_index = Article()
        new_index.save()

    def add(self, filename, file_content):
        """
            Добавление в базу
        """

        text = file_content if type(file_content) == list else file_content.split('\n')

        tokens = self.tokenizer(text, self.tokensFileContent)
        tokenstring = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(K), int(T))

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
        article.index_name = NAME_IN
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
        """

        tokens = self.tokenizer(file_content, self.tokensFileContent)
        tokenstring = ''.join([x[0] for x in tokens])
        fingerprints = fingerprint.fingerprints(tokens, int(K), int(T))

        buf = file_name.split('/')
        docname = buf[-1]
        buf = docname.split('_')
        if len(buf) != 4:
            return {'message': 'File name error'}
            exit()

        author = buf[0]
        subject = buf[1]
        work_type = buf[2]
        task_num = buf[3].split('.')[0]

        article = Article()
        article.index_name = NAME_IN
        article.docname = docname
        article.author = author
        article.subject = subject
        article.work_type = work_type
        article.task_num = task_num
        article.text = file_content
        article.tokens = tokens
        article.tokenstring = tokenstring
        article.fingerprints = fingerprints

        new_hash_fingerprints = list(x[0] for x in fingerprints)

        if source.lower() == 'all':
            result = Search(using=self.es, index=NAME_IN) \
                .query('match', index_name=NAME_IN)
        else:
            buf = source.split('_')
            if (len(buf)) == 1:
                result = Search(using=self.es, index=NAME_IN) \
                    .query('match', subject=buf[0])
            elif (len(buf)) == 2:
                result = Search(using=self.es, index=NAME_IN) \
                    .query('match', subject=buf[0]) \
                    .query('match', work_type=buf[1])
            elif (len(buf)) == 3:
                result = Search(using=self.es, index=NAME_IN) \
                    .query('match', subject=buf[0]) \
                    .query('match', work_type=buf[1]) \
                    .query('match', task_num=buf[2])
            else:
                return {'message': 'Incorrect source!'}

        self.es.indices.refresh(index=NAME_IN)

        response = result.execute()
        reports = list()

        if response.hits.total.value == 0:
            return {'message': 'ES have no docs to compare!'}

        else:
            for hit in result.scan():
                if hit.author == author:
                    continue
                if hit.docname == docname:
                    continue
                a = fuzz.ratio(tokenstring, hit.tokenstring)
                old_hash_fingerprints = list(x[0] for x in hit.fingerprints)
                intersection = list(set(new_hash_fingerprints).intersection(
                    set(old_hash_fingerprints)))
                token_distance = len(
                    intersection) / max(len(set(old_hash_fingerprints)), len(set(new_hash_fingerprints))) * 100
                reports.append((a, token_distance, fingerprint.report(
                    fingerprints, hit.fingerprints), hit.docname, list(hit.text)))

                print(hit)

        reports.sort(key=lambda x: x[0], reverse=True)

        results = []
        dst_names = []
        similars_proc = []

        for report in reports[:5]:
            tr = fingerprint.print_report(report[2], docname, report[3])
            if tr != None:
                results.append(tr)
                dst_names.append(report[3])
                similars_proc.append(str(report[0]) + '%/' + str(report[1]))

        return result

    def get_field_values(self, field):
        data = self.es.sql.query(
            body={
                'query': f'SELECT {field} FROM papa'
            }
        )
        result = list({v[0] for v in data['rows'] if v[0]})
        result.sort()
        return result

    def get_src(self, subjects, work_types, task_nums):
        s = Search(index=NAME_IN)

        filters = []
        if subjects:
            filters.append({"term": {"subject": subjects}})
        if work_types:
            filters.append({"term": {"work_type": work_types}})
        if task_nums:
            filters.append({"term": {"task_num": task_nums}})

        if filters:
            s = s.query("bool", filter=filters)

        response = s.execute()

        docnames = [hit.docname for hit in response]

        return docnames
