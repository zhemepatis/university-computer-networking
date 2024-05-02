from command_parser import CommandParser

class TelnetParser(CommandParser):
    def __init__(self, path):
        super().__init__(path)  
        

    def parse_open(self, inp):        
        split_line = inp.split()
        arg_num = len(split_line)
        host, port = None, None

        if arg_num == 2:
            _, host = split_line
            port = 23
        elif arg_num == 3:
            _, host, port = split_line
        else:
            return -1
        
        return host, port
    

    def parse_close(self, inp):        
        split_line = inp.split()
        arg_num = len(split_line)
        return arg_num != 1


