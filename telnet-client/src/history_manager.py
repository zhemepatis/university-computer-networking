import json

class HistoryManager():
    def __init__(self, path):
        self.path = path
        self.history = self.load_history(path)


    def load_history(self, path):
        with open(path, "r") as file:
            return json.load(file)


    def add_entry(self, host, port):
        with open(self.path, "r+") as file:
            entry_id = len(self.history)
            entry = {
                "id": entry_id,
                "host": host,
                "port": port
            }

            if self.check_if_entry_exists(entry):
                return

            self.history.append(entry)
            file.seek(0)
            json.dump(self.history, file, indent=4)


    def remove_entry(self, host, port):
        pass


    def clear_history(self, host, port):
        to_be_deleted = None
        for entry in self.history:
            if entry["host"] == host and entry["port"] == port:
                to_be_deleted = entry

        with open(self.path, "r+") as file:
            self.history.remove(to_be_deleted)
            file.seek(0)
            json.dump(self.history, file, indent=4)


    def check_if_entry_exists(self, new_entry):
        for entry in self.history:
            if new_entry["host"] == entry["host"] and new_entry["port"] == entry["port"]:
                return True

        return False
