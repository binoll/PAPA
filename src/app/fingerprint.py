"""
This module provides functions to generate fingerprints.
"""

import hashlib

from nltk import ngrams
from typing import List, Tuple


def str_to_hash(s: List[Tuple[str, int]]) -> Tuple[int, int, int]:
    """
    Convert a list of tuples to a hash value.

    Args:
        s (List[Tuple[str, int]]): List of tuples containing characters and their positions.

    Returns:
        Tuple[int, int, int]: Hash value along with the position of the first and last character.
    """
    sum_hash = int(hashlib.sha1(''.join([t[0] for t in s]).encode("utf-8")).hexdigest(), 16) % (10 ** 8)
    return sum_hash, s[0][1], s[-1][1]


def right_index(el: Tuple[str, int], s: List[Tuple[str, int]]) -> int:
    """
    Find the rightmost index of a character in a list of tuples.

    Args:
        el (Tuple[str, int]): Character tuple to search for.
        s (List[Tuple[str, int]]): List of tuples to search in.

    Returns:
        int: Index of the rightmost occurrence of the character.
    """
    indices = [i for i, ltr in enumerate(s) if ltr[0] == el[0]]
    return indices[-1]


def fingerprints(data: List[str], k: int, t: int) -> List[Tuple[int, int, int]]:
    """
    Generate fingerprint for a given sequence of characters.

    Args:
        data (List[str]): Sequence of characters.
        k (int): Size of each gram.
        t (int): Size of the sliding window.

    Returns:
        List[Tuple[int, int, int]]: List of fingerprint tuples.
    """

    grams = list(ngrams(data, k))
    fingerprint = []
    hashes = []

    for gram in grams:
        hashes.append(str_to_hash(gram))

    w = t - k + 1
    t_grams = list(ngrams(hashes, w))

    m = (0, 0, 0)
    pos = 0

    for gram in t_grams:
        if m == min(gram):
            if (gram.count(m) >= 1) and (right_index(m, gram) > pos):
                pos = right_index(m, gram)
                fingerprint.append(gram[pos])
                m = gram[pos]
            else:
                pos -= 1
        else:
            m = min(gram, key=lambda x: x[0])
            pos = right_index(m, gram)
            fingerprint.append(gram[pos])
            m = gram[pos]

    result = []
    for i in fingerprint:
        if i not in result:
            result.append(i)

    return result


def report(data1: List[Tuple[int, int, int]], data2: List[Tuple[int, int, int]]) \
        -> List[Tuple[int, int, int, int, int]]:
    """
    Generate a report based on fingerprint matches between two sets of data.

    Args:
        data1 (List[Tuple[int, int, int]]): First set of fingerprint tuples.
        data2 (List[Tuple[int, int, int]]): Second set of fingerprint tuples.

    Returns:
        List[Tuple[int, int, int, int, int]]: List of tuples representing matches between the two sets of data.
    """

    result = []

    for item in data1:
        finder = list(filter(lambda x2: x2[0] == item[0], data2))
        if finder:
            for r in finder:
                result.append((item[0], item[1], item[2], r[1], r[2]))

    result.sort(key=lambda x: x[0])

    res = []
    for i in result:
        if i not in res:
            res.append(i)

    return res


def create_report(report_list: str | List, doc1: str, doc2: str) -> List[str]:
    """
    Generate a human-readable report based on fingerprint matches between two documents.

    Args:
        report_list (List[Tuple[int, int, int, int, int]]):
            List of tuples representing matches between two sets of data.
        doc1 (str): Name of the first document.
        doc2 (str): Name of the second document.

    Returns:
        List[str]: List of strings representing the human-readable report.
    """

    results = []
    buf = []

    if len(report_list) == 0:
        return []

    report_list.sort(key=lambda x: x[1])

    for str_tuple in report_list:
        buf.append(list((str_tuple[1:5])))

    report_list.clear()

    for i in buf:
        if i in report_list:
            continue
        report_list.append(i)

    buf.clear()
    buffalo = list((report_list[0][0:2]))
    buffer = []

    for item in report_list:
        if list(item[0:2]) == buffalo:
            buffer.append(item[2:5])
        else:
            buf.append(list((buffalo, buffer)))
            buffalo = list(item[0:2])
            buffer.append(item[2:5])
    buf.append(list((buffalo, buffer)))

    for item in buf:
        results.append(
            f'Строки документа "{doc1}" с номерами {item[0][0]} - '
            f'{item[0][1]} похожи на строки {item[1]} из "{doc2}"'
        )

    return results
