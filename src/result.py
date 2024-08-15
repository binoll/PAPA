"""
This module for save result of searches.
"""

from typing import List


class ResultsState:
    """
    Class for save result of searches.
    """

    def __init__(self):
        self.results = []

    def set_results(self, results: List[str]):
        self.results = results

    def get_results(self) -> List[str]:
        return self.results
