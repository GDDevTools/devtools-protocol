*For the mod with similar name from Geode Team, see [DevTools](mod:geode.devtools)*

Casual users should **not** download this. It does nothing (except opening a WebSocket port for the entire game idk)

# ello orld
The Geometry Dash DevTools Protocol allows for tools to instrument, inspect, debug and profile GD instance. Nothing uses this protocol and I maintains this API.

Instrumentation is divided into a number of domains (DOM, Input, Network etc.). Each domain defines a number of commands it supports and events it generates. Both commands and events are serialized JSON objects of a fixed structure.

## Protocol API Docs
uhm

An offline copy of it can be found in the mod's `.geode` file, under `GDP_Domains_Reference.md`

## Resources
In the future I might add the ability to test domain methods in-game, but for now, https://tryitands.ee

# Frequently Asked Questions
## What's the port
1412

## How is the protocol defined?
idk i just did

## Can I get the protocol as JSON?
resources/protocols.json

## Does the protocol support multiple simultaneous clients?
I haven't tested it yet but I think if IXWebSocket supports it then yeah you can

## Can I have a catgirl?
[no](https://www.pixiv.net/artworks/100528610)