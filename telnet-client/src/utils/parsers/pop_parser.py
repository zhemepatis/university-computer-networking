from utils.parsers.command_parser import CommandParser

class PopParser(CommandParser):
    def parse_user(self, inp):
        split = inp.split()

        if split[0] != "user":
            return 0
        
        if len(split) < 2 or len(split) > 2:
            return -1
        
        return split[1]

    
    def parse_pass(self, inp):
        split = inp.split()

        if split[0] != "pass":
            return 0
        
        if len(split) < 2 or len(split) > 2:
            return -1
        
        return split[1]
    
    
    def parse_list(self, inp):
        split = inp.split()

        if split[0] != "list":
            return 0
        
        if len(split) > 2:
            return -1
        
        if len(split) == 2:
            return split[1]
        
        return None
    

    def parse_retrieve(self, inp):
        split = inp.split()

        if split[0] != "retrieve":
            return 0
        
        if len(split) > 2:
            return -1
        
        return split[1]
    
    
    def parse_delete(self, inp):
        split = inp.split()

        if split[0] != "delete":
            return 0
        
        if len(split) > 2:
            return -1
        
        return split[1]
