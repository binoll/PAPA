"""
This module provides functions for tokenizing source code files according to specific rules
defined in a tokens configuration file.
"""

import re
import json

from typing import List

SPECIALS = ['T', 'C', 'K', 'A', 'S', 'L', 'B', 'V', 'F']


def remove_patterns(data: List[str], pattern: str) -> List[str]:
    """
    Remove specific patterns from each line in data.

    Args:
        data (List[str]): Lines of text to process.
        pattern (str): The regex pattern to remove from each line.

    Returns:
        List[str]: The processed lines with the pattern removed.
    """

    return [re.sub(pattern, '', line) for line in data]


def replace_patterns(data: List[str], pattern: str, replacement: str) -> List[str]:
    """
    Replace specific patterns in each line with a replacement string.

    Args:
        data (List[str]): Lines of text to process.
        pattern (str): The regex pattern to replace in each line.
        replacement (str): The string to replace the pattern with.

    Returns:
        List[str]: The processed lines with the pattern replaced.
    """

    return [re.sub(pattern, replacement, line) for line in data]


def tokenizer(file_source: List[str] | str, file_tokens: str) -> List[tuple]:
    """
    Tokenizer from MPI projects

    Args:
        file_source (List[str]): Lines from file source.
        file_tokens (str): Content from file with tokens.

    Returns:
        List[tuple]: List of tokens with their line numbers.
    """

    result = []

    tokens = json.loads(file_tokens)
    data = file_source if isinstance(file_source, list) else file_source.split('\n')

    data = remove_patterns(data, r"\-?[\d]+[\.\d]*")

    data = remove_patterns(data, r"(\/\/.*\n)")
    data = remove_patterns(data, r"#.*")

    for i in range(len(data)):
        if "/*" in data[i]:
            end_comment_index = i
            while end_comment_index < len(data) and "*/" not in data[end_comment_index]:
                data[end_comment_index] = ""
                end_comment_index += 1
            if end_comment_index < len(data):
                data[end_comment_index] = re.sub(r"\*/", '', data[end_comment_index])
            data[i] = re.sub(r"/\*[\d\D]*", '', data[i])

    data = remove_patterns(data, r"[\"\']+.*[\"\']+")

    token_categories = [
        ('TYPES', 'T'), ('CYCLES', 'C'), ('K_WORDS', 'K'),
        ('OPERATORS_BIT', 'B'), ('OPERATORS_SRVN', 'S'),
        ('OPERATORS_ARIFM', 'A'), ('OPERATORS_LOG', 'L')
    ]

    for category, replacement in token_categories:
        for k in tokens[category]:
            data = [line.replace(k, replacement) for line in data]

    data = replace_patterns(data, r'\.([\d\w\_]*)\(', 'F')
    data = replace_patterns(data, r' ?([\d\w\_]+)\(', 'F')

    for i in range(len(data)):
        for v in re.findall(r'[TCRBSAL]+ +([A-z0-9_]+)', data[i]):
            data[i] = data[i].replace(v, 'V')

    for i, line in enumerate(data):
        for symbol in line:
            if symbol in SPECIALS:
                result.append((symbol, i + 1))

    return result
