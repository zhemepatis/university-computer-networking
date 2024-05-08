import socket
from socket_client import SocketClient
from utils.parsers.pop_parser import PopParser
import utils.email_retriever as retriever

BUFF_SIZE = 1024

class PopClient(SocketClient):
    def __init__(self, host, port):
        super().__init__()
        self.open(host, port)

        self.username = None
        self.parser = PopParser()


    def ask_for_input(self):
        return input("pop> ")
    

    def open(self, host, port):
        if self.conn != None:
            print("Another connection has already been made, if you want to make another connection, close the previous one first.")
            return
        
        self.conn_to_pop(host, port)
        if self.conn == None:
            print(f"Coulnd't connect to server on address: {self.host}, port: {self.port}.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print(res[4:])
        elif res.startswith("-ERR"):
            print(res[5:])

    
    def user(self, username): 
        protocol = bytes('USER {}\r\n'.format(username), 'utf-8')

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print("Username accepted.")
        elif res.startswith("-ERR"):
            print(res[5:])


    def password(self, password):
        protocol = bytes('PASS {}\r\n'.format(password), 'utf-8')

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print(res[4:])
        elif res.startswith("-ERR"):
            print(res[5:])


    def stat(self):
        protocol = bytes('STAT\r\n', 'utf-8')

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print(res[4:])
        elif res.startswith("-ERR"):
            print(res[5:])

    
    def msg_list(self, parse_result):
        num = None
        protocol = None

        if parse_result == None:
            protocol = bytes('LIST\r\n', 'utf-8')
        else:
            try:
                num = int(parse_result)
                protocol = bytes(f'LIST {num}\r\n', 'utf-8')
            except ValueError:
                print("Incorrect argument.")

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print(res[4:])
        elif res.startswith("-ERR"):
            print(res[5:])


    def retrieve(self, parse_result):
        protocol = None

        try:
            num = int(parse_result)
            protocol = bytes(f'RETR {num}\r\n', 'utf-8')
        except ValueError:
            print("Incorrect argument.")

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("-ERR"):
            print(res[5:])
            return

        read_bytes = 0
        data = b''
        while True:
            read = self.conn.recv(BUFF_SIZE)
            read_bytes = len(read)
            data += read

            if read_bytes < BUFF_SIZE and read.decode().endswith(".\r\n"):
                break

        author = retriever.get_author(data)
        print(f'Author: {author}')
        date = retriever.get_date(data)
        print(f'Date: {date}')
        subject = retriever.get_subject(data)
        print(f'Subject: {subject}')
        content = retriever.get_content(data)
        print(f'Content: {content}')


    def delete(self, parse_result):
        protocol = None

        try:
            num = int(parse_result)
            protocol = bytes(f'DELE {num}\r\n', 'utf-8')
        except ValueError:
            print("Incorrect argument.")

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print(f'Message {parse_result} marked as to be deleted.')
        elif res.startswith("-ERR"):
            print(res[5:])

    
    def noop(self):
        protocol = bytes('NOOP\r\n', 'utf-8')

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print("No operation.")


    def reset(self):
        protocol = bytes('RSET\r\n', 'utf-8')

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print("Messages unmarked as to be deleted.")
        elif res.startswith("-ERR"):
            print(res[5:])

    
    def quit(self):
        protocol = bytes('QUIT\r\n', 'utf-8')

        sent = self.conn.send(protocol)
        if sent == 0:
            print("Couldn't issue the command.")
            return

        res = self.conn.recv(BUFF_SIZE).decode()
        if res.startswith("+OK"):
            print(res[4:])
        elif res.startswith("-ERR"):
            print(res[5:])


    def run(self):
        while True:
            inp = self.ask_for_input()

            if len(inp.split()) == 0:
                continue

            parse_result = self.parser.parse_user(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.user(parse_result)
                continue

            parse_result = self.parser.parse_pass(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.password(parse_result)
                continue

            parse_result = self.parser.parse_list(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.msg_list(parse_result)
                continue

            parse_result = self.parser.parse_command_without_arguments("stat", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.stat()
                continue

            parse_result = self.parser.parse_retrieve(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.retrieve(parse_result)
                continue

            parse_result = self.parser.parse_delete(inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.delete(parse_result)
                continue

            parse_result = self.parser.parse_command_without_arguments("noop", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.noop()
                continue

            parse_result = self.parser.parse_command_without_arguments("reset", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.reset()
                continue

            parse_result = self.parser.parse_command_without_arguments("quit", inp)
            if parse_result == -1:
                print("Incorrect argument number.")
                continue
            elif parse_result != 0:
                self.quit()
                self.close()
                break

            print(f"Command not found.")


if __name__ == "__main__":
    client = PopClient()
    client.run()