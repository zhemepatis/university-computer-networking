import json

class CommandParser():
    def parse_keyword(self, inp):
        if len(inp.split()) == 0:
            return -1

        return (inp.split())[0]
    
    
    def check_next_keyword(self, expected, inp):
        keyword = self.parse_keyword(inp)
        return keyword == expected
    

    def check_arg_num(self, expected, args):
        return expected == len(args)
    
    
    def parse_command_without_arguments(self, expected_command, inp):        
        split_expected = expected_command.split()
        split_inp = inp.split()
        marker = " "

        if marker.join(split_expected) != marker.join(split_inp):
            return 0
         