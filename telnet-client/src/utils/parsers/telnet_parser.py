from utils.parsers.command_parser import CommandParser

class TelnetParser(CommandParser):    
    def parse_host_and_port(self, args):
        host, port = None, None

        if len(args) == 1:
            host = args[0]
            port = 23
        elif len(args) == 2:
            host, port = args
        else:
            return -1
        
        return host, port
    

    def parse_open_conn(self, inp):        
        split = inp.split()

        if split[0] != "open":
            return 0
        
        if len(split) < 2 or len(split) > 3:
            return -1
        
        args = split[1:]
        return self.parse_host_and_port(args)
    

    def parse_open_mail(self, inp):
        split = inp.split()

        if len(split) < 2:
            return 0

        if split[0] != "open" or split[1] != "mail":
            return 0
        
        if len(split) < 3:
            return -1
        
        args = split[2:]
        return self.parse_host_and_port(args)
    

    def parse_cache_open(self, inp):
        split = inp.split()

        if len(split) < 3:
            return 0

        if split[0] != "cache" or split[1] != "open":
            return 0
        
        if len(split) > 3:
            return -1
        
        return split[2]
    

    def parse_cache_remove(self, inp):
        split = inp.split()

        if len(split) < 3:
            return 0

        if split[0] != "cache" or split[1] != "remove":
            return 0
        
        if len(split) > 3:
            return -1
        
        return split[2]