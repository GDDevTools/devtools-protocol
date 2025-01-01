import json
import pdl

json.dump(
    pdl.loads(open("gd_protocol.pdl").read(), "gd_protocol.pdl"),
    open("protocols.json", "w"),
    indent=2,
    separators=(",", ": ")
)