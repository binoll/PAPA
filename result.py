from typing import List


class ResultsState:
    def __init__(self):
        self.results = []

    def set_results(self, results: List[str]):
        self.results = results

    def get_results(self) -> List[str]:
        return self.results
