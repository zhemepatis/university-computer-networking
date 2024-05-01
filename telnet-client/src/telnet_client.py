import sys
import json
import parsers
from connection import Connection

CMD_LIST_PATH = "../data/commands.json"

class TelnetClient():
    def __init__(self, host, port):
        if host != None and port != None:
            self.conn = self.open(host, port)
        else:
            self.conn = None
        self.load_cmd_list()


    def load_cmd_list(self):
        with open(CMD_LIST_PATH, "r") as file:
            self.commands = json.load(file)


    def handle_inp(self, inp):
        if self.check_cmd(self.commands["OPEN_CMD"], inp):
            self.handle_open(inp)
        elif self.check_cmd(self.commands["CLOSE_CMD"], inp):
            self.handle_close()
        elif self.check_cmd(self.commands["EXIT_CMD"], inp):
            self.handle_exit()
        elif self.check_cmd(self.commands["HELP_CMD"], inp):
            self.handle_help()
        else:
            print("Invalid command. To get more info about supported commands enter \"help\".")


    def check_cmd(self, expected_cmd, inp):
        keyword = (inp.split())[0]
        return keyword == expected_cmd


    def handle_open(self, inp):
        if self.conn != None:
            print("There is already running connection. If you want to open another, close previous first.")
            return

        result = parsers.parse_open(inp)
        if result == -1:
            print("Incorrect arguments")
            return 
            
        host, port = result
        self.open(host, port)


    def handle_close(self):
        if self.conn == None:
            print("No running connection")
            return

        self.conn.close()
        self.conn = None
        print("Connection closed")

    
    def handle_exit(self):
        print("Exiting program...")
        self.exit()        


    def handle_help(self):
        print("Supported commands:")
        for command_name in self.commands:
            print(f"\t{self.commands[command_name]}")


    def open(self, host, port):
        self.conn = Connection(host, port)
        if self.conn.server_socket == None:
            print(f"Couldn\'t establish connection (host: {host}, port: {port})")
            self.conn = None
        else: 
            print(f"Connection has been established (host: {host}, port: {port})")


    def exit(self):
        if self.conn != None:
            self.conn.close()
        
        sys.exit(0)