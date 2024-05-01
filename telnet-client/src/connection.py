import socket

class Connection():
    def __init__(self, host, port):
        for result in socket.getaddrinfo(host, port, socket.AF_UNSPEC, socket.SOCK_STREAM):
            addr_family, socket_type, protocol, _, socket_addr = result
            
            try:
                self.server_socket = socket.socket(addr_family, socket_type, protocol)
            except OSError:
                self.server_socket = None
                continue
            
            try:
                self.server_socket.connect(socket_addr)
            except OSError:
                self.server_socket.close()
                self.server_socket = None
                continue

            break


    def close(self):
        self.server_socket.close()
