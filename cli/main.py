from elasticsearch import Elasticsearch

from detectors import fingerprint as finger
from tokenizers import tokenizer_MPI as token
import click
import os
from fuzzywuzzy import fuzz
from elasticsearch_dsl import Document, connections, Search, Text, Keyword

K = 10
T = 13
NAME_IN = 'papa'
TOKENS_JSON = 'cli/tokens/tokens_MPI.json'
HOST = '0.0.0.0'
PORT = 9200

es = Elasticsearch([{'host': HOST, 'port': PORT, 'scheme': 'http'}])
connections.create_connection(hosts=[{'host': HOST, 'port': PORT, 'scheme': 'http'}])


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
            "number_of_shards": 4,
            "number_of_replicas": 4,
        }


class Start(object):
    @staticmethod
    def create():
        """
            Создание пустого индекса с именем name_in
        """
        if es.indices.exists(index=NAME_IN):
            es.indices.delete(index=NAME_IN)
        Article.init()
        new_index = Article()
        new_index.save()
        print("Index created!")

    @staticmethod
    def add(filename):
        """
            Добавление в базу
        """

        with open(filename, 'r', encoding='utf-8') as f:
            text = f.readlines()

        tokens = token.tokenizer(filename, TOKENS_JSON)
        tokenstring = ''.join([x[0] for x in tokens])
        fingerprints = finger.fingerprints(tokens, K, T)

        buf = filename.split('/')
        docname = buf[-1]
        buf = docname.split('_')
        if len(buf) != 4:
            print(filename, " - filename error")
            exit()
        author = buf[0]
        subject = buf[1]
        work_type = buf[2]
        task_num = buf[3].split('.')[0]

        article = Article()
        article.index_name = NAME_IN
        article.docname = docname

        result = Search(using=es, index=NAME_IN) \
            .query("match", docname=docname)
        response = result.execute()
        if response.hits.total.value != 0:
            print(docname, " already in ES!")
            return

        article.author = author
        article.subject = subject
        article.work_type = work_type
        article.task_num = task_num
        article.text = text
        article.tokens = tokens
        article.tokenstring = tokenstring
        article.fingerprints = fingerprints
        article.save()

        es.indices.refresh(index=NAME_IN)
        print(filename, " added!")

    @staticmethod
    def PAPA(filename, source):
        """
            Проверка кода с базой
        """

        with open(filename, 'r', encoding='utf-8') as f:
            text = f.readlines()

        tokens = token.tokenizer(filename, TOKENS_JSON)
        tokenstring = ''.join([x[0] for x in tokens])
        fingerprints = finger.fingerprints(tokens, K, T)

        buf = filename.split('/')
        docname = buf[-1]
        buf = docname.split('_')
        if len(buf) != 4:
            print("Filename error")
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

        new_hash_fingerprints = list(x[0] for x in fingerprints)

        if source.lower() == 'all':
            result = Search(using=es, index=NAME_IN) \
                .query("match", index_name=NAME_IN)
        else:
            buf = source.split('_')
            if (len(buf)) == 1:
                result = Search(using=es, index=NAME_IN) \
                    .query("match", subject=buf[0])
            elif (len(buf)) == 2:
                result = Search(using=es, index=NAME_IN) \
                    .query("match", subject=buf[0]) \
                    .query("match", work_type=buf[1])
            elif (len(buf)) == 3:
                result = Search(using=es, index=NAME_IN) \
                    .query("match", subject=buf[0]) \
                    .query("match", work_type=buf[1]) \
                    .query("match", task_num=buf[2])
                print(buf)
            else:
                print("Incorrect source!")
                exit()

        es.indices.refresh(index=NAME_IN)

        response = result.execute()
        reports = list()

        if response.hits.total.value == 0:
            print(" ES have no docs to compare!")
            return False
        else:
            for hit in result.scan():
                if hit.author == author:
                    continue
                if hit.docname == docname:
                    continue
                a = fuzz.ratio(tokenstring, hit.tokenstring)
                old_hash_fingerprints = list(x[0] for x in hit.fingerprints)
                intersection = list(set(new_hash_fingerprints).intersection(set(old_hash_fingerprints)))
                token_distance = len(intersection) / min(len(set(old_hash_fingerprints)),
                                                         len(set(new_hash_fingerprints))) * 100
                reports.append((a, token_distance, finger.report(fingerprints, hit.fingerprints), hit.docname))

        reports.sort(key=lambda x: x[0], reverse=True)

        for report in reports[:5]:
            print()
            print("Сходство ", docname, " с документом ", report[3], " по Левенштейну - ", report[0],
                  "%, по отпечаткам - ", report[1], "%.")
            finger.print_report(report[2], docname, report[3])
            print()
            if ((report[0] >= 85) or (report[1] >= 35)):
                return True

        return False


elastic = Start()


@click.group()
def main():
    @main.command(name='create', help='Create index')
    def help():
        print("Справка")


@main.command(name='create', help='Create index')
def create_click():
    elastic.create()


@main.command(name='add', help='Load doc from file. Params: -f [path to file]')
@click.option('-f', '--filename', type=click.Path(exists=True, file_okay=True, dir_okay=False), required=True,
              prompt=True, help='Filename/path')
def add_click(filename):
    elastic.add(filename)


@main.command(name='add_dir', help='Load docs from dir. Params: -p [path to dir (without dirs) with files')
@click.option('-p', '--path', type=click.Path(exists=True, file_okay=False, dir_okay=True), required=True, prompt=True,
              help='Path to dir')
def add_dir_click(path):
    for filename in os.listdir(path):
        elastic.add(path + '/' + filename)


@main.command(name='PAPA_dir',
              help='PAPA-ing docs from dir. Params: -p [path to dir (without dirs) with files to check] -s [source in ES]')
@click.option('-p', '--path', type=click.Path(exists=True, file_okay=False, dir_okay=True), required=True, prompt=True,
              help='Path to dir')
@click.option('-s', '--source', required=True, prompt=True, help='Source')
def PAPA_dir_click(path, source):
    for filename in os.listdir(path):
        if elastic.PAPA(path + '/' + filename, source) == False:
            print("Случай плагиата не зафиксирован, документ сохранён, см. отчёт в консоли")
            elastic.add(path + '/' + filename)
        else:
            print("Обнаружен плагиат, см. отчёт в консоли")
            return 1
    return 0


@main.command(name='PAPA', help='PAPA. Params: -f [path to file to check] -s [source in ES]')
@click.option('-f', '--filename', type=click.Path(exists=True, file_okay=True, dir_okay=False), required=True,
              prompt=True, help='Filename/path')
@click.option('-s', '--source', required=True, prompt=True, help='Source')
def PAPA_click(filename, source):
    if elastic.PAPA(filename, source) == False:
        print("Случай плагиата не зафиксирован, документ сохранён, см. отчёт в консоли")
        elastic.add(filename)
    else:
        print("Обнаружен плагиат, см. отчёт в консоли")
        return 1
    return 0


if __name__ == "__main__":
    main()
