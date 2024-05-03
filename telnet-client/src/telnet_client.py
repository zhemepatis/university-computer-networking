import sys
import socket
from telnet_parser import TelnetParser
from history_manager import HistoryManager

HISTORY_DB_PATH = "../data/connection_history.json"

class TelnetClient():
    def __init__(self):
        self.host = None
        self.port = None
        self.conn = None

        self.parser = TelnetParser()
        self.history_manager = HistoryManager(HISTORY_DB_PATH)


    def ask_for_input(self):
        return input("telnet-client> ")
    

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
        print(f"Connecting to server on address {host}, port {port}.")

    
    def exec_close(self):
        if self.conn != None:
            self.close()
            print("Connection successfully closed.")
        else:
            print("No connection is running.")


    def conn_to_server(self, host, port):
        for result in socket.getaddrinfo(host, port, socket.AF_UNSPEC, socket.SOCK_STREAM):
            addr_family, socket_type, protocol, _, socket_addr = result
            
            try:
                self.conn = socket.socket(addr_family, socket_type, protocol)
            except OSError:
                self.conn = None
                continue
            
            try:
                self.conn.connect(socket_addr)
            except OSError:
                self.conn.close()
                self.conn = None
                continue

            break

        self.host = host
        self.port = port


    def close(self):
        if self.conn != None:
            self.conn.close()
            self.conn = None


    def exit(self):
        print("Exiting program...")  
        self.close()    
        sys.exit(0)


    def list_cache(self, list):
        if len(list) == 0:
            print("Cache is empty.")

        for entry in list:
            print(entry)

    
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
            elif parse_result != 0:
                self.list_cache(self.history_manager.history)
                continue   

            parse_result = self.parser.parse_command_without_arguments("cache clear", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
            elif parse_result != 0:
                self.clear_cache()
                continue   

            parse_result = self.parser.parse_cache_remove(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
            elif parse_result != 0:
                self.remove_from_cache(parse_result)
                continue  

            print(f"Command not found.")


if __name__ == "__main__":
    client = TelnetClient()
    client.run()

