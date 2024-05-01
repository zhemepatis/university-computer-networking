import sys
from telnet_client import TelnetClient


arg_num = len(sys.argv)
host, port = None, None

if arg_num == 2:
    _, host = sys.argv
    port = 23
elif arg_num == 3:
    _, host, port = sys.argv


client = TelnetClient(host, port)
while True:
    inp = input("telnet-client> ")
    client.handle_inp(inp)