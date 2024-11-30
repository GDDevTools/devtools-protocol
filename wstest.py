import json
import websocket
from functools import partial

ws = websocket.WebSocket()
ws.connect("ws://127.0.0.1:1412")

jid = 0
class Domain(object):
    def __init__(self, d) -> None:
        self.domainName = d

    def j(self, name, **args):
        global jid
        ws.send(json.dumps({
            "id": jid,
            "method": self.domainName+"."+name,
            "params": args
        }))
        jid+=1

    def __getattribute__(self, name: str):
        if name == "domainName": return super().__getattribute__(name)
        return partial(super().__getattribute__("j"), name)

class Socketer:
    def __getattribute__(self, name: str):
        return Domain(name)

while True:
    s = Socketer()
    try:
        m = input("< ")
        exec("protocol."+m, globals(), {"protocol": s, "jid": jid})
        while True:
            recv = ws.recv()
            if ("id" not in json.loads(recv)):
                print(">",recv)
            else:
                print(">",recv)
                break
    except KeyboardInterrupt:
        ws.close()
        break
    except Exception as e:
        print(e)