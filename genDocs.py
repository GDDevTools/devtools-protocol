from __future__ import annotations

head = """GD DevTools Protocol Domains Reference (INCOMPLETE)
---

Except from some changes (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

This .md file is generated from protocols.json so it's guaranteed to match with the protocols list returned.

> **Note** most of this docs will sound similar to the docs from the aforementioned protocol and it's intended. Don't ask anything about that.

<style type="text/css">
  tbody>tr>td:first-child {text-align: right}
</style>

"""

import json
from typing import Required, TypedDict, NotRequired

file = json.load(open("protocol_definitions/protocols.json"))

class ObjectArrayType(TypedDict, total=False):
    name: Required[str]
    description: str
    optional: bool 

class Experiment(TypedDict, total=False):
    experimental: bool

class PrimitiveObjectProps(ObjectArrayType, Experiment, total=False):
    type: str
    enums: list[str]
    items: list[ObjectArrayType]

_ObjectProps = TypedDict("_ObjectProps", {
    # seems to be so
    "$ref": str
}, total=False)
class ObjectProps(_ObjectProps, PrimitiveObjectProps, total=False):
    pass


class Event(Experiment, total=False):
    name: Required[str]
    description: str
    parameters: list[ObjectProps]
class Command(Event, total=False):
    returnValue: list[ObjectProps]
class Type(ObjectProps, total=False):
    id: Required[str]
    description: str
    properties: list[ObjectProps]

class Domain(Experiment,total=False):
    domain: Required[str]
    description: str
    types: list[Type]
    commands: list[Command]
    events: list[Event]

primitiveTypes = ['integer', 'number', 'boolean', 'string', 'object',
                  'any', 'array', 'binary']
def genPropTable(d: str, p: ObjectProps):
    t = ""
    primitive = True
    if "type" in p:
        t = p['type']
    else:
        t = p.get("$ref", "")
        primitive = False
    return f"""
  <tr>
    <td><code>{p['name']}</code>{"<br>(optional)" if p.get('optional', False) else ""}</td>
    <td><strong>{t if primitive else f'<a href="#{d.lower() if "." not in t else ""}{t.replace(".","").lower()}">{t}</a>'}</strong>{"<br>"+p['description'] if "description" in p else ""}{f"<br>Allowed Values: <code>{', '.join(p.get("enums",[]))}</code>" if t == "string" and len(p.get("enums",[])) != 0 else ""}</td>
  </tr>"""

def annotateExperimental(o: Experiment):
    return "**EXPERIMENTAL FEATURE** - Things are expected to break, please do NOT send a bug report.  \n" if "experimental" in o and o["experimental"] else ""

def genTypeDocTable(d: str, cmd: Type):
    doc=f"""### {d}.`{cmd['id']}`
{annotateExperimental(cmd)}
{cmd.get("description","")}
Type: **{cmd.get('type', cmd.get("$ref","idk they didnt specify it"))}**

"""
    properties = cmd.get("properties")
    if (properties != None):
        doc+="<table>"
        doc+="""
<thead>
<tr>
<th colspan="2">Properties</th>
</tr>
</thead>
<tbody>"""
        for p in properties:
            doc+=genPropTable(d,p)
        doc+="\n</tbody>\n</thead>\n</table>\n\n"
    return doc
def genPropsTable(d: str, cmd: Event):
    doc=f"""### {d}.`{cmd['name']}`
{annotateExperimental(cmd)}
{cmd.get("description","")}

"""
    parameters = cmd.get("parameters")
    ret = cmd.get("returns")
    if (parameters != None or ret != None):
        doc+="<table>"
        if (parameters!=None):
            doc+="""
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>"""
            for p in parameters:
                doc+=genPropTable(d, p)
            doc+="\n</tbody>\n"
        if (ret!=None):
            doc+="""
<thead>
<tr>
<th colspan="2">Return Values</th>
</tr>
</thead>
<tbody>"""
            for r in ret:
                doc+=genPropTable(d, r)
            doc+="\n</tbody>\n"
        doc+="</table>\n\n"
    return doc


domains: list[Domain] = file["domains"]
for i in domains:
    d = i['domain']
    doc = f"""# `{d}` Domain
{annotateExperimental(i)}
{i.get('description','')}

"""
    
    commands = i.get("commands",[])
    if len(commands) != 0:
        doc+="## Methods\n"
        for cmd in commands:
            doc+=genPropsTable(d, cmd)
    events = i.get("events",[])
    if len(events) != 0:
        doc+="## Events\n"
        for evt in events:
            doc+=genPropsTable(d, evt)
    types = i.get("types",[])
    if len(types) != 0:
        doc+="## Types\n"
        for t in types:
            doc+=genTypeDocTable(d, t)
    head+=doc+"\n"

open("GDP_Domains_Reference.md","w").write(head)
