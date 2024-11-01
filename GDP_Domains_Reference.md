# GD DevTools Protocol Domains Reference

Except from some derivations (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

**Note** that this reference ==only documents what's currently available in the mod itself==, so don't expect every features of CDP available (especially the early releases of the mod)

## Syntax
idk its `{domain}.{methods/events/types}`

## `DOM` Domain
its what you see, you made that super duper excellently!  
(thats the cocos2d node tree)  
(node ids is actually the address of the node, the more you know)  

### Methods

#### DOM.describeNode {#DOM-describeNode}
Describes node given its id, does not require domain to be enabled. Does not start tracking any objects, can be used for automation.

> Parameters

| Name | Description |
|---|---|
| nodeId | The node identifier |
| depth | (optional) The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0. |

> Return object

| Name | Description |
|---|---|
| node | [Node](#DOM-Node)<br>Node description |

### Types 

#### DOM.Node {#DOM-Node}
DOM interaction is implemented in terms of mirror objects that represent the actual DOM nodes. DOMNode is a base node mirror type.  
Type: **object**

> Properties

| Name | Description |
|---|---|
| nodeId | Node identifier that is passed into the rest of the DOM messages as the `nodeId`. Backend will only push node with given `id` once. It is aware of all requested nodes and will only fire DOM events for nodes known to the client. |
| parentId | (optional) The id of the parent node if any. |
| childNodeCount | Child count for this node. |
| children | Child nodes of this node when requested with children. |
| attributes | Attributes of this node in the form of an object `{name1: value1, name2: value2, ...}` |

#### DOM.NodeId
Unique DOM node identifier.  
In GDP's case, it's the address of the node.  
Type: **integer**
