import json

class HistoryManager():
    def __init__(self, path):
        self.history = self.load_history(path)

    def load_history(self, path):
        with open(path, "r") as file:
            return json.load(file)


    def add_entry(self, host, port):
        pass


    def remove_entry(self, host, port):
        pass


    def clear_history(self, host, port):
        pass