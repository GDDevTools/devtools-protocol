*For the mod with similar name from Geode Team, see [DevTools](mod:geode.devtools)*

Casual users should **not** download this. It does nothing (except opening a WebSocket port for the entire game idk)

# ello orld
The Geometry Dash DevTools Protocol allows for tools to instrument, inspect, debug and profile GD instance. Nothing uses this protocol and I maintains this API.

Instrumentation is divided into a number of domains (DOM, Input, Network etc.). Each domain defines a number of commands it supports and events it generates. Both commands and events are serialized JSON objects of a fixed structure.
