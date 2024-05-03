import json

class HistoryManager():
    def __init__(self, path):
        self.path = path
        self.history = self.load_history(path)


    def load_history(self, path):
        with open(path, "r") as file:
            return json.load(file)
        
    
    def get_entry_by_id(self, entry_id):
        for item in self.history:
            if item["id"] == entry_id:
                return item


    def add_entry(self, host, port):
        with open(self.path, "w") as file:
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


    def remove_entry_by_id(self, id):
        to_be_deleted = self.get_entry_by_id(id)

        with open(self.path, "w") as file:
            self.history.remove(to_be_deleted)
            file.seek(0)
            json.dump(self.history, file, indent=4)


    def clear_history(self):
        with open(self.path, "w") as file:
            self.history.clear()
            file.seek(0)
            json.dump(self.history, file, indent=4)


    def check_if_entry_exists(self, new_entry):
        for entry in self.history:
            if new_entry["host"] == entry["host"] and new_entry["port"] == entry["port"]:
                return True

        return False
