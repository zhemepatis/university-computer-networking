from socket_client import SocketClient
from utils.parsers.pop_parser import PopParser

class PopClient(SocketClient):
    def __init__(self):
        super().__init__()

        self.parser = PopParser()


    def ask_for_input(self):
        return input("pop> ")


    def run(self):
        while True:
            inp = self.ask_for_input()

            parse_result = self.parser.parse_command_without_arguments("quit", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.exit()

            print(f"Command not found.")


if __name__ == "__main__":
    client = PopClient()
    client.run()