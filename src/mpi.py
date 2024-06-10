import re
import json
from typing import List


def tokenizer(filesource: List, filetokens: str) -> List:
    """
    Tokenizer from MPI projects

    Args:
        filesource (list[<string>]): Lines from filesource
        filetokens (src): Content from file with tokens

    Returns:
        _type_: _description_
    """
    SPECIALS = ['T', 'C', 'K', 'A', 'S', 'L', 'B', 'V', 'F']

    TOKENS = json.loads(filetokens)
    data = filesource if type(filesource) is list else filesource.split('\n')

    # Чистка числовых констант
    data = list(map(lambda d: re.sub(r"\-?[\d]+[\.\d]*", '', d), data))

    # Чистка строчных комментариев
    for i in range(len(data)):
        for ph in re.findall(r"(\/\/.*\n)", data[i]):
            data[i] = data[i].replace(ph, '')

    # Чистка мультистрочных комментариев
    for i in range(len(data)):
        for ph in re.findall(r"(\/\*[\d\D]*)", data[i]):

            j = i
            while not len(re.findall(r"\*\/", data[j])):
                data[j] = ""
                j += 1
            data[j] = re.sub(r"\*\/", '', data[j])
            data[i] = data[i].replace(ph, '')
            i = j - 1

    for i in range(len(data)):

        # Чистка подключаемых библиотек комментариев
        for ph in re.findall(r"#[^\n]+", data[i]):
            data[i] = data[i].replace(ph, '')

        # Чистка строчных аргументов
        for ph in re.findall(r"[\"\']+.*[\"\']+", data[i]):
            data[i] = data[i].replace(ph, '')

            # Чистка строчных аргументов
    for i in range(len(data)):
        for ph in re.findall(r"[\"\']+.*[\"\']+", data[i]):
            data[i] = data[i].replace(ph, '')

    for k in TOKENS['TYPES']:
        data = list(map(lambda d: d.replace(k, 'T'), data))

    for k in TOKENS['CYCLES']:
        data = list(map(lambda d: d.replace(k, 'C'), data))

    for k in TOKENS['K_WORDS']:
        data = list(map(lambda d: d.replace(k, 'K'), data))

    for k in TOKENS['OPERATORS_BIT']:
        data = list(map(lambda d: d.replace(k, 'B'), data))

    for k in TOKENS['OPERATORS_SRVN']:
        data = list(map(lambda d: d.replace(k, 'S'), data))

    for k in TOKENS['OPERATORS_ARIFM']:
        data = list(map(lambda d: d.replace(k, 'A'), data))

    for k in TOKENS['OPERATORS_LOG']:
        data = list(map(lambda d: d.replace(k, 'L'), data))

    # Поиск методов и функций
    data = list(map(lambda d: re.sub(r'\.([\d\w\_]*)\(', 'F', d), data))
    data = list(map(lambda d: re.sub(r' ?([\d\w\_]+)\(', 'F', d), data))

    # Поиск переменных
    for i in range(len(data)):
        for v in re.findall(r'[TCRBSAL]+ +([A-z0-9\_]+)', data[i]):
            data[i] = data[i].replace(v, 'V')

    result = []
    for i in range(len(data)):
        for symbol in data[i]:
            if symbol in SPECIALS:
                result.append((symbol, i + 1))

    return result
