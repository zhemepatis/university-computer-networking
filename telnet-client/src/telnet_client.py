from socket_client import SocketClient
from pop_client import PopClient
from utils.parsers.telnet_parser import TelnetParser
from utils.history_manager import HistoryManager

HISTORY_DB_PATH = "../data/connection_history.json"

class TelnetClient(SocketClient):
    def __init__(self):
        super().__init__()

        self.parser = TelnetParser()
        self.history_manager = HistoryManager(HISTORY_DB_PATH)


    def ask_for_input(self):
        return input("telnet> ")
    

    def open(self, parse_result):
        if self.conn != None:
            print("Another connection has already been made, if you want to make another connection, close the previous one first.")
            return
        
        host, port = parse_result
        self.conn_to_server(host, port)
        if self.conn != None:
            print(f"Connection was successfully opened (address: {self.host}, port: {self.port}).")
        else:
            print(f"Coulnd't connect to server on address: {self.host}, port: {self.port}.")

        self.history_manager.add_entry(host, port)

    
    def open_mail(self, parse_result):
        if self.conn != None:
            print("Another connection has already been made, if you want to make another connection, close the previous one first.")
            return
        
        host, port = parse_result
        pop_client = PopClient(host, port)
        pop_client.run()

    
    def exec_close(self):
        if self.conn != None:
            self.close()
            print("Connection successfully closed.")
        else:
            print("No connection is running.")


    def list_cache(self, list):
        if len(list) == 0:
            print("Cache is empty.")

        for entry in list:
            print(entry)


    def cache_open(self, parse_result):
        entry_id = None

        try:
            entry_id = int(parse_result)

            entry = self.history_manager.get_entry_by_id(entry_id)
            if entry == None:
                print("There is no entry with such id.")
            else:
                args = [entry["host"], entry["port"]]
                self.open(args)
        except ValueError:
            print("Incorrect argument.")

    
    def clear_cache(self):
        print("Clearing cache...")
        self.history_manager.clear_history()


    def remove_from_cache(self, parse_result):
        try:
            entry_id = int(parse_result)
            self.history_manager.remove_entry_by_id(entry_id)
            print("Entry removed.")
        except ValueError:
            print("Incorrect argument.")


    def run(self):
        while True:
            inp = self.ask_for_input()

            if len(inp.split()) == 0:
                continue

            parse_result = self.parser.parse_open_mail(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.open_mail(parse_result)
                continue

            parse_result = self.parser.parse_open_conn(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.open(parse_result)
                continue

            parse_result = self.parser.parse_command_without_arguments("close", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.exec_close()
                continue

            parse_result = self.parser.parse_command_without_arguments("exit", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.exit()

            parse_result = self.parser.parse_command_without_arguments("cache list", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.list_cache(self.history_manager.history)
                continue   

            parse_result = self.parser.parse_cache_open(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.cache_open(parse_result)
                continue   

            parse_result = self.parser.parse_command_without_arguments("cache clear", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.clear_cache()
                continue   

            parse_result = self.parser.parse_cache_remove(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.remove_from_cache(parse_result)
                continue  

            print(f"Command not found.")


if __name__ == "__main__":
    client = TelnetClient()
    client.run()

