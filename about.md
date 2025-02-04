*For the mod with similar name from Geode Team, see [DevTools](mod:geode.devtools)*

Casual users should **not** download this. It does nothing (except opening a WebSocket port for the entire game idk)

# ello orld
The Geometry Dash DevTools Protocol allows for tools to instrument and inspect GD instance. Nothing uses this protocol and I maintains this API.

Instrumentation is divided into a number of domains (DOM, Input, Network etc.). Each domain defines a number of commands it supports and events it generates. Both commands and events are serialized JSON objects of a fixed structure.

## Protocol API Docs
uhm

An offline copy of it can be found in the mod's `.geode` file, under `GDP_Domains_Reference.md`

Or you can get the protocol as a JSON file via the files listed in the relevant FAQ below.

## Resources
Check out https://github.com/HenrySck075/gd-devtools-playground for the playground application.

# Frequently Asked Questions
## What's the port
1412

## How is the protocol defined?
The canonical protocol definitions live in the mod's source tree: (gd_protocol.pdl). They are maintained manually by the DevTools engineering team. The declarative protocol definitions are used across tools; for instance, a documentation generator for the definitions, and the playground app that is used for, well, testing.

## Can I get the protocol as JSON?
These canonical .pdl files are then used to generate the `protocols.json` file -and subsequently, the .md docs file-, which is included in the mod as a resource, and used as the response for the `getProtocolInformation` method.

## Does the protocol support multiple simultaneous clients?
yeah sure go ahead

## Can I have a catgirl?
[no](https://www.pixiv.net/artworks/100528610)
