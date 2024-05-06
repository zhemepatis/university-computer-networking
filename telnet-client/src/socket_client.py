import sys
import socket

class SocketClient():
    def __init__(self):
        self.host = None
        self.port = None
        self.conn = None

    
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

