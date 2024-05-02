import sys
import socket
from telnet_parser import TelnetParser
from history_manager import HistoryManager

COMMANDS_DB_PATH = "../data/telnet_commands.json"
HISTORY_DB_PATH = "../data/connection_history.json"

class TelnetClient():
    def __init__(self):
        self.host = None
        self.port = None
        self.conn = None

        self.parser = TelnetParser(COMMANDS_DB_PATH)
        self.history_manager = HistoryManager(HISTORY_DB_PATH)


    def ask_for_input(self):
        return input("telnet-client> ")
    
    
    def open(self, host, port):
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


    def exit(self):
        print("Exiting program...")  
        self.close()    
        sys.exit(0)


    def help(self, commands):
        command_num = len(commands)

        print("Supported commands:")
        for i in range(0, command_num):
            print(f"\t{commands[i]["keyword"]}")


    def list_cache(self):
        history = self.history_manager.history
        for entry in history:
            print(entry)

    
    def run(self):
        while True:
            inp = self.ask_for_input()

            if self.parser.check_command("open", inp):
                result = self.parser.parse_open(inp)
                if result == -1:
                    print("Incorrect arguments.")
                    continue

                if self.conn != None:
                    print("Another connection has already been made, if you want to make another connection, close the previous one first.")
                    continue
                
                host, port = result
                self.open(host, port)
                if self.conn != None:
                    print(f"Connection was successfully opened (address: {self.host}, port: {self.port})")
                else:
                    print(f"Coulnd't connect to server on address: {self.host}, port: {self.port}.")
                    
            elif self.parser.check_command("close", inp):
                result = self.parser.parse_close(inp)
                if result == -1:
                    print("Incorrect arguments.")
                    continue

                if self.conn != None:
                    self.close()
                    print("Connection successfully closed.")
                else:
                    print("No connection is running.")

            elif self.parser.check_command("exit", inp):
                self.exit()
            elif self.parser.check_command("help", inp):
                self.help(self.parser.commands)
            elif self.parser.check_command("cache", inp):
                self.list_cache()
            else:
                print(f"Command not found. Type \'help\' for help.")


if __name__ == "__main__":
    client = TelnetClient()
    client.run()

