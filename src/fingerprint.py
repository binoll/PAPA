from nltk import ngrams
import itertools, hashlib


def str_to_hash(str: str) -> int:
    sum = int(hashlib.sha1(''.join([t[0] for t in str]).encode("utf-8")).hexdigest(), 16) % (10 ** 8)
    return (sum, str[0][1], str[-1][1])


def right_index(el, str):
    x = [i for i, ltr in enumerate(str) if ltr[0] == el[0]]
    return x[-1]


def fingerprints(data, k, t):
    kgrams = list(ngrams(data, k))

    hashes = list()

    for gram in kgrams:
        hashes.append(str_to_hash(gram))

    w = t - k + 1
    tHgrams = list(ngrams(hashes, w))

    fngrprnts = list()
    m = (0, 0, 0)
    lpos = 0

    for gram in tHgrams:
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
        # item[1].sort(key=lambda x: x[0])
        print('Строки документа', doc1, ' с номерами ', item[0][0], ' - ', item[0][1], ' похожи на строки ', item[1],
              ' документа ', doc2)

    result = []
    for b in buf:
        source = [x for x in range(b[0][0], b[0][1] + 1)]
        b[1] = [
            [s for s in range(x[0], x[1] + 1)]
            for x in b[1]
        ]

        similarList = []
        for p in itertools.permutations(b[1]):
            if (len(p) > 1 and (set(p[0]) & set(p[1]))):
                simData = list(set(p[0]).union(set(p[1])))
                simData.sort()

                if simData not in similarList:
                    similarList.append(simData)
            else:
                if p[0] not in similarList:
                    similarList.append(p[0])

                if len(p) > 1 and p[1] not in similarList:
                    similarList.append(p[1])

        temp_push = [
            source, similarList
        ]
        bulka = False
        for k in result:
            if (set(k[0]) & set(source)):
                k[0] = list(set(k[0]).union(set(source)))
                k[1] += similarList
                bulka = True

        if temp_push not in result and not bulka:
            result.append(temp_push)

    return result
