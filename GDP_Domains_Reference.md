GD DevTools Protocol Domains Reference (INCOMPLETE)
---

Except from some derivations (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

> **Note** that this reference ==only documents what's currently available in the mod itself==, so don't expect every features of CDP available (especially the early releases of the mod)
> 
> **Note 2** most of this docs will sound similar to the docs from the aforementioned protocol and it's intended. Don't ask anything about that.


<style type="text/css">
  tbody>tr>td:first-child {text-align: right}
</style>

# `DOM` Domain
its what you see, you made that super duper excellently!  
(thats the cocos2d node tree)  
(node ids is actually the address of the node, the more you know)  

## Methods

### DOM.`describeNode`
Describes node given its id, does not require domain to be enabled. Does not start tracking any objects, can be used for automation.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>Id of the node to retrieve attributes for.</td>
  </tr>
  <tr>
    <td><code>depth</code></td>
    <td><strong>integer</strong><br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0.</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>node</code></td>
    <td><strong><a href="#domnode">Node</a></strong><br>Node description</td>
  </tr>
</tbody>
</table>

### DOM.`disable`
Disables DOM agent.

### DOM.`enable`
Enables DOM agent.

### DOM.`getAttributes`
Returns attributes for the specified node.  

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>Id of the node to retrieve attributes for.</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>attributes</code></td>
    <td><strong>object[string, <a href="#domattributetype">AttributeType</a>]</strong><br>An interleaved array of node attribute names and values.</td>
  </tr>
</tbody>
</table>

### DOM.`getAttribute`
Returns a specific attribute for the specified node.  

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>Id of the node to retrieve attributes for.</td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong><br>Name of the attribute.</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>value</code></td>
    <td><strong><a href="#domattributetype">AttributeType</a></strong><br>Value of the requested attribute.</td>
  </tr>
</tbody>
</table>

### DOM.`getBoxModel`
Returns boxes for the given node.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>Identifier of the node.</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>model</code></td>
    <td><strong><a href="#domboxmodel">BoxModel</a></strong><br>Box model for the node.</td>
  </tr>
</tbody>
</table>

### DOM.`getDocument`
Returns the root DOM node (which is the current scene) (and optionally the subtree) to the caller. Implicitly enables the DOM domain events for the current target.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>depth</code></td>
    <td><strong>integer</strong><br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0.</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>node</code></td>
    <td><strong><a href="#domnode">Node</a></strong><br>Node description.</td>
  </tr>
</tbody>
</table>

### DOM.`getNodeForLocation`
Returns node id at given location. Depending on whether DOM domain is enabled, nodeId is either returned or not.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>x</code></td>
    <td><strong>integer</strong><br>X coordinate.</td>
  </tr>
  <tr>
    <td><code>y</code></td>
    <td><strong>integer</strong><br>Y coordinate.</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>Id of the node at given coordinates, only when enabled and requested document.</td>
  </tr>
</tbody>
</table>

### DOM.`moveTo`
Moves node into the new container, optionally places it before/after the given anchor.
> `insertBeforeNodeId` and `insertAfterNodeId` **cannot** both be defined.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Id of the node to move.</td>
  </tr>
  <tr>
    <td><code>targetNodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Id of the element to drop the moved node into.</td>
  </tr>
  <tr>
    <td><code>insertBeforeNodeId</code><br>(optional)</td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Drop node before this one (if absent, the moved node becomes the last child of <code>targetNodeId</code>).</td>
  </tr>
  <tr>
    <td><code>insertAfterNodeId</code><br>(optional)</td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Drop node after this one (if absent, the moved node becomes the last child of <code>targetNodeId</code>).</td>
  </tr>
</tbody>

<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>Id of the node at given coordinates, only when enabled and requested document.</td>
  </tr>
</tbody>
</table>

## Types 
### DOM.`BoxModel`
Box model.  
Type: **object**

<table>
<thead>
  <tr>
    <th colspan="2">Properties</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>width</code></td>
    <td><strong>integer</strong><br>Node width.</td>
  </tr>
  <tr>
    <td><code>height</code></td>
    <td><strong>integer</strong><br>Node height.</td>
  </tr>
</tbody>
</table>

### DOM.`AttributeType`
The supported attribute type.  
Allowed Types: `string, int, bool, array, object, {nodeId: int}`  
TODO: figure out how to differentiate between Node and object

### DOM.`Node`
DOM interaction is implemented in terms of mirror objects that represent the actual DOM nodes. DOMNode is a base node mirror type.  
Type: **object**

<table>
<thead>
  <tr>
    <th colspan="2">Properties</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Node identifier that is passed into the rest of the DOM messages as the <code>nodeId</code>. Backend will only push node with given <code>id</code> once. It is aware of all requested nodes and will only fire DOM events for nodes known to the client.</td>
  </tr>
  <tr>
    <td><code>parentId</code><br>(optional)</td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>The id of the parent node if any.</td>
  </tr>
  <tr>
    <td><code>childNodeCount</code></td>
    <td><strong>integer</strong><br>The id of the parent node if any.</td>
  </tr>
  <tr>
    <td><code>children</code></td>
    <td><strong>array[<a href="#domnode">Node</a>]</strong><br>TChild nodes of this node when requested with children.</td>
  </tr>
  <tr>
    <td><code>attributes</code></td>
    <td><strong>object[string, <a href="#domattributetype">AttributeType</a>]</strong><br>Attributes of this node in the form of an object <code>{name1: value1, name2: value2, ...}</code></td>
  </tr>
</tbody>
</table>

### DOM.`NodeId`
Unique DOM node identifier.  
In GDP's case, it's the address of the node.  
Type: **integer**

# `Game` Domain
held the player's game session hostage

## Methods
### Game.`close`
Close game gracefully.

### Game.`crash`
Close game gracelessly. (raises an std::runtime_error)

### Game.`restart`
Restart game gracefully.

### Game.`getVersion`
Returns version information.

<table>
<thead>
  <tr>
    <th colspan="2">Return Object</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>protocolVersion</code></td>
    <td><strong>tuple[int, int, int]</strong><br>This mod's version in the style of <code>[major, minor, patch]</code></td>
  </tr>
  <tr>
    <td><code>loaderVersion</code></td>
    <td><strong>tuple[int, int, int]</strong><br>Geode version in the style of <code>[major, minor, patch]</code></td>
  </tr>
  <tr>
    <td><code>gameVersion</code></td>
    <td><strong>string</strong><br>Geometry Dash version.</td>
  </tr>
</tbody>
</table>

# `Input` Domain
Will this be the next macro bot?
> **NOTE:** Input events are ALWAYS sent 1 frame later because it has to occur on main thread

## Methods
### Input.`dispatchKeyEvent`
Dispatches a key event to the game.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>type</code></td>
    <td><strong>string</strong><br>Type of the key event.<br>Allowed values: `keyDown, keyUp, char`</td>
  </tr>
  <tr>
    <td><code>modifiers</code><br>(optional)</td>
    <td><strong>integer</strong><br>Bit field representing pressed modifier keys. Alt=1, Ctrl=2, Meta/Command=4, Shift=8 (default: 0).</td>
  </tr>
  <tr>
    <td><code>text</code><br>(optional)</td>
    <td><strong>string</strong><br>Text as generated by processing a virtual key code with a keyboard layout. Does nothing if left empty or <code>type</code> is not <code>char</code></td>
  </tr>
  <tr>
    <td><code>code</code><br>(optional)</td>
    <td><strong>char</strong><br>just a character. might change in future but this is reasonable.<br>only supports some ascii characters defined in geode enumKeyCodes</td>
  </tr>
</tbody>
</table>

### Input.`dispatchMouseEvent`
Dispatches a mouse event to the page.

<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>type</code></td>
    <td><strong>string</strong><br>Type of the key event.<br>Allowed values: `keyDown, keyUp, char`</td>
  </tr>
  <tr>
    <td><code>x</code></td>
    <td><strong>integer</strong><br>X coordinate of the event relative to the main frame's viewport in Cocos2d point.</td>
  </tr>
  <tr>
    <td><code>y</code></td>
    <td><strong>integer</strong><br>Y coordinate of the event relative to the main frame's viewport in Cocos2d point (which is 0 in the bottom and screen height on the top).</td>
  </tr>
  <tr>
    <td><code>modifiers</code><br>(optional)</td>
    <td><strong>integer</strong><br>Bit field representing pressed modifier keys. Alt=1, Ctrl=2, Meta/Command=4, Shift=8 (default: 0).</td>
  </tr>
  <tr>
    <td><code>button</code></td>
    <td><strong>string</strong><br>Mouse button (default: "none").<br>Allowed Values: <code>none, left, middle, right, back, forward</code></td>
  </tr>
  <tr>
    <td><code>deltaX</code><br>(optional)</td>
    <td><strong>integer</strong><br>X delta in Cocos2d point for mouse wheel event (default: 0).</td>
  </tr>
  <tr>
    <td><code>deltaY</code><br>(optional)</td>
    <td><strong>integer</strong><br>Y delta in Cocos2d point for mouse wheel event (default: 0).</td>
  </tr>
</tbody>
</table>

# `Network` Domain (Unavailable)
Network domain allows tracking network activities of the page. It exposes information about http, data and other requests and responses, their headers, bodies, etc.

## Methods
### Network.`getResponseBody`
e

## Events
### Network.`requestWillBeSent`
Fired when page is about to send HTTP request.
<table>
<thead>
  <tr>
    <th colspan="2">Parameters</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td><code>requestId</code></td>
    <td><strong>integer</strong><br>Request identifier<br>Note that this is only unique across an <code>initiator</code></td>
  </tr>
  <tr>
    <td><code>request</code></td>
    <td><strong><a href="#networkrequest">Request</a></strong><br>Request data.</td>
  </tr>
  <tr>
    <td><code>timestamp</code></td>
    <td><strong><a href="#networktimesinceepoch">TimeSinceEpoch</a></strong><br>Timestamp.</td>
  </tr>
  <tr>
    <td><code>initiator</code></td>
    <td><strong>string</strong><br>Request initiator<br>Allowed Values: <code>cocos, geode</code></td>
  </tr>
</tbody>
</table>

## Types
### Network.`Request`
HTTP request data.  
Type: **object**

### Network.`TimeSinceEpoch`
UTC time in seconds, counted from January 1, 1970.  
Type: **integer**
