import json
import websocket
from functools import partial

ws = websocket.WebSocket()
ws.connect("ws://127.0.0.1:1313")

id = 0
class Domain(object):
    def __init__(self, d) -> None:
        self.domainName = d

    def j(self, name, **args):
        ws.send(json.dumps({
            "id": id,
            "method": self.domainName+"."+name,
            "params": args
        }))

    def __getattribute__(self, name: str):
        return partial(super().__getattribute__("j"), name)

class Socketer:
    def __getattribute__(self, name: str):
        return Domain(name)

while True:
    s = Socketer()
    try:
        m = input("< ")
        exec("protocol."+m, globals(), {"protocol": s})
        print(">",ws.recv())
    except:
        ws.close()