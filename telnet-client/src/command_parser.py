import json

class CommandParser():   
    def __init__(self, path):
        self.commands = self.load_commands(path)


    def load_commands(self, path):
            with open(path, "r") as file:
                return json.load(file)
            

    def parse_keyword(self, inp):
        return (inp.split())[0]
    

    def check_command(self, expected, inp):
        keyword = self.parse_keyword(inp)
        return keyword == expected