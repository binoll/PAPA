from nltk import ngrams
import itertools
import hashlib


def str_to_hash(s: str) -> int:
    """
    Функция преобразует строку в хэш-код.

    Args:
        s (str): Входная строка.

    Returns:
        int: Хэш-код строки.
    """
    sum = int(hashlib.sha1(s.encode("utf-8")).hexdigest(), 16) % (10 ** 8)
    return sum


def right_index(el, s):
    """
    Функция возвращает последний индекс элемента el в строке s.

    Args:
        el: Элемент, индекс которого нужно найти.
        s: Строка, в которой ищется элемент.

    Returns:
        int: Индекс элемента в строке.
    """
    x = [i for i, ltr in enumerate(s) if ltr[0] == el[0]]
    return x[-1]


def fingerprints(data, k, t):
    """
    Функция формирует отпечатки (fingerprints) из данных.

    Args:
        data: Входные данные.
        k (int): Длина k-грамм.
        t (int): Параметр t.

    Returns:
        list: Список отпечатков.
    """
    kgrams = list(ngrams(data, k))

    hashes = list()

    for gram in kgrams:
        hashes.append(str_to_hash(gram))

    w = t - k + 1
    t_hgrams = list(ngrams(hashes, w))

    fngrprnts = list()
    m = (0, 0, 0)
    lpos = 0

    for gram in t_hgrams:
        if m == min(gram):
            if (gram.count(m) >= 1) and (right_index(m, gram) > lpos):
                lpos = right_index(m, gram)
                fngrprnts.append(gram[lpos])
                m = gram[lpos]
            else:
                lpos -= 1
        else:
            m = min(gram, key=lambda x: x[0])
            lpos = right_index(m, gram)
            fngrprnts.append(gram[lpos])
            m = gram[lpos]

    result = list()
    for i in fngrprnts:
        if i not in result:
            result.append(i)

    return result


def report(data1, data2):
    """
    Функция формирует отчет о сходстве между двумя наборами данных.

    Args:
        data1: Первый набор данных.
        data2: Второй набор данных.

    Returns:
        list: Отчет о сходстве.
    """
    result = []
    for x in data1:
        finder = list(filter(
            lambda x2: x2[0] == x[0],
            data2
        ))
        if finder:
            for r in finder:
                result.append((
                    x[0], x[1], x[2], r[1], r[2]
                ))

    result.sort(key=lambda x: x[0])

    res = list()
    for i in result:
        if i not in res:
            res.append(i)

    return res


def print_report(report, doc1, doc2):
    """
    Функция выводит отчет о сходстве между двумя документами.

    Args:
        report: Отчет о сходстве.
        doc1: Имя первого документа.
        doc2: Имя второго документа.

    Returns:
        list: Отчет о сходстве.
    """
    buf = list()
    if len(report) == 0:
        return
    report.sort(key=lambda x: x[1])

    for str in report:
        buf.append(list((str[1:5])))

    report.clear()

    for i in buf:
        if i not in report:
            report.append(i)

    buf.clear()
    bufl = list((report[0][0:2]))
    bufr = list()

    for item in report:
        if list(item[0:2]) == bufl:
            bufr.append(item[2:5])
        else:
            buf.append(list((bufl, bufr)))
            bufl = list(item[0:2])
            bufr = list()
            bufr.append(item[2:5])
    buf.append(list((bufl, bufr)))

    for item in buf:
        print('Строки документа', doc1, ' с номерами ', item[0][0], ' - ', item[0][1], ' похожи на строки ', item[1],
              ' документа ', doc2)

    result = []
    for b in buf:
        source = [x for x in range(b[0][0], b[0][1] + 1)]
        b[1] = [
            [s for s in range(x[0], x[1] + 1)]
            for x in b[1]
        ]

        similar_list = []
        for p in itertools.permutations(b[1]):
            if len(p) > 1 and (set(p[0]) & set(p[1])):
                sim_data = list(set(p[0]).union(set(p[1])))
                sim_data.sort()

                if sim_data not in similar_list:
                    similar_list.append(sim_data)
            else:
                if p[0] not in similar_list:
                    similar_list.append(p[0])

                if len(p) > 1 and p[1] not in similar_list:
                    similar_list.append(p[1])

        temp_push = [
            source, similar_list
        ]
        bulka = False
        for k in result:
            if set(k[0]) & set(source):
                k[0] = list(set(k[0]).union(set(source)))
                k[1] += similar_list
                bulka = True

        if temp_push not in result and not bulka:
            result.append(temp_push)

    return result
