GD DevTools Protocol Domains Reference (INCOMPLETE)
---

Except from some changes (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

This .md file is generated from protocols.json so it's guaranteed to match with the protocols list returned.

> **Note** most of this docs will sound similar to the docs from the aforementioned protocol and it's intended. Don't ask anything about that.


<style type="text/css">
  tbody>tr>td:first-child {text-align: right}
</style>

# `DOM` Domain
This domain exposes DOM read/write operations. Each DOM Node is represented with its mirror object
that has an `id`. This `id` can be used to get additional information on the Node, and more.
It is important that client receives DOM events only for the nodes that are known to the client.
Backend keeps track of the nodes that were sent to the client and never sends the same node twice.
It is client's responsibility to collect information about the nodes that were sent to the client.

## Methods
### DOM.`click`
Try sending a click to the node without actually clicking.
Only works for `button` nodes.

First it tries to cast the node to `CCMenuItem` and clicks it.
If it isn't a `CCMenuItem` then it will send a `DispatchEvent("puppeteer/nodeClicked")` event
to the game so nodes implmenting that can handle as it needs.

If you want real click events then you can use [Input.dispatchMouseEvent](method:Input.dispatchMouseEvent) instead.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>The node id. Must be a button node</td>
  </tr>
</tbody>
</table>

### DOM.`describeNode`
Describes node given its id, does not require domain to be enabled. Does not start tracking any
objects, can be used for automation.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Identifier of the node.</td>
  </tr>
  <tr>
    <td><code>depth</code><br>(optional)</td>
    <td><strong>integer</strong><br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the
entire subtree or provide an integer larger than 0.</td>
  </tr>
</tbody>
</table>

### DOM.`disable`
Disables DOM agent.

### DOM.`enable`
Enables DOM agent.

### DOM.`getAttribute`
Get a node's attribute.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>The target node's ID</td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong><br>The attribute's name</td>
  </tr>
</tbody>
</table>

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
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>The target node's ID</td>
  </tr>
</tbody>
</table>

### DOM.`getBoxModel`


<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>The target node's ID</td>
  </tr>
</tbody>
</table>

### DOM.`getDocument`
Returns the root DOM node (the scene) (and optionally the subtree) to the caller.
Implicitly enables the DOM domain events for the current target.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>depth</code><br>(optional)</td>
    <td><strong>integer</strong><br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0.</td>
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
</table>

### DOM.`moveTo`
Moves node into the new parent, places it before the given anchor.

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
</tbody>
</table>

### DOM.`querySelector`
Executes querySelector on a given node.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Id of the node to query upon.</td>
  </tr>
  <tr>
    <td><code>selector</code></td>
    <td><strong>string</strong><br>Selector string</td>
  </tr>
</tbody>
</table>

### DOM.`removeAttribute`
Removes attribute with given name from an element with given id.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong></td>
  </tr>
</tbody>
</table>

### DOM.`removeNode`
Removes node with given id.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Id of the node to remove.</td>
  </tr>
</tbody>
</table>

### DOM.`setAttribute`
Sets attribute for an element with given id.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong></td>
  </tr>
  <tr>
    <td><code>value</code></td>
    <td><strong>string</strong></td>
  </tr>
</tbody>
</table>

## Events
### DOM.`attributeModified`
Fired when [Node](type:DOM.Node)'s attribute is modified.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>ID of the node that has changed.</td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong><br>Attribute name</td>
  </tr>
  <tr>
    <td><code>value</code></td>
    <td><strong>string</strong><br>Attribute value</td>
  </tr>
</tbody>
</table>

### DOM.`attributeRemoved`
Fired when [Node](type:DOM.Node)'s attribute is removed.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>ID of the node that has changed.</td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong><br>Attribute name</td>
  </tr>
</tbody>
</table>

### DOM.`childNodeCountUpdated`
Fired when Node's children count has changed.
Called in conjunction with other child-related events.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
  <tr>
    <td><code>childNodeCount</code></td>
    <td><strong>integer</strong></td>
  </tr>
</tbody>
</table>

### DOM.`childNodeInserted`
Fired when Node added a child.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>parentNodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
  <tr>
    <td><code>previousNodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
  <tr>
    <td><code>node</code></td>
    <td><strong><a href="#domnode">Node</a></strong></td>
  </tr>
</tbody>
</table>

### DOM.`childNodeRemoved`
Fired when Node removed a child.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>parentNodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong></td>
  </tr>
</tbody>
</table>

### DOM.`documentUpdated`
Fired when the Scene has been replaced.

## Types
### DOM.`NodeId`
the

### DOM.`BoxModel`
Box model.

<table>
<thead>
<tr>
<th colspan="2">Properties</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>width</code></td>
    <td><strong>integer</strong><br>Node width</td>
  </tr>
  <tr>
    <td><code>height</code></td>
    <td><strong>integer</strong><br>Node height</td>
  </tr>
</tbody>
</thead>
</table>

### DOM.`Node`
DOM interaction is implemented in terms of mirror objects that represent the actual DOM nodes.
DOMNode is a base node mirror type.

<table>
<thead>
<tr>
<th colspan="2">Properties</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>nodeId</code></td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>Node identifier that is passed into the rest of the DOM messages as the `nodeId`. Backend
will only push node with given `id` once. It is aware of all requested nodes and will only
fire DOM events for nodes known to the client.</td>
  </tr>
  <tr>
    <td><code>parentId</code><br>(optional)</td>
    <td><strong><a href="#domnodeid">NodeId</a></strong><br>The id of the parent node if any.</td>
  </tr>
  <tr>
    <td><code>nodeType</code></td>
    <td><strong>integer</strong><br>`Node`'s nodeType.</td>
  </tr>
  <tr>
    <td><code>childNodeCount</code><br>(optional)</td>
    <td><strong>integer</strong><br>Child count for this node.</td>
  </tr>
  <tr>
    <td><code>children</code><br>(optional)</td>
    <td><strong>array</strong><br>Child nodes of this node when requested with children.</td>
  </tr>
  <tr>
    <td><code>attributes</code><br>(optional)</td>
    <td><strong>object</strong><br>Attributes of the node. (unavailable for now)</td>
  </tr>
</tbody>
</thead>
</table>


# `Log` Domain
Get events whenever a log is pushed.

## Methods
### Log.`disable`


### Log.`enable`


## Events
### Log.`entryAdded`
Issued when a new message was logged.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>entry</code></td>
    <td><strong><a href="#loglogentry">LogEntry</a></strong></td>
  </tr>
</tbody>
</table>


# `Level` Domain
Manages the levels list.

## Methods
### Level.`disable`
Disables the Level agent.

### Level.`enable`
Enables the Level agent.

### Level.`getList`
Get a local list.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>id</code></td>
    <td><strong>integer</strong><br>The list ID.</td>
  </tr>
  <tr>
    <td><code>type</code></td>
    <td><strong><a href="#levelleveltype">LevelType</a></strong><br>The level type.</td>
  </tr>
  <tr>
    <td><code>includeLevels</code><br>(optional)</td>
    <td><strong>boolean</strong><br>Bundle the levels with the response. Defaults to false.</td>
  </tr>
</tbody>
</table>

### Level.`getLevel`
Get a locally saved level, either created level or saved level.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>id</code></td>
    <td><strong>integer</strong><br>The level ID.</td>
  </tr>
  <tr>
    <td><code>type</code></td>
    <td><strong><a href="#levelleveltype">LevelType</a></strong><br>The level type.</td>
  </tr>
</tbody>
</table>

### Level.`getLevels`
Get some online level.
To get levels from a list, use [getList](method:Level.getList) instead

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>page</code><br>(optional)</td>
    <td><strong>integer</strong><br>The page to retrieve the list from. Leave empty to fetch all of them</td>
  </tr>
</tbody>
</table>

### Level.`deleteLevel`
Delete a level. Asks the user before doing so and returns an error if the user rejects the request.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>id</code></td>
    <td><strong>integer</strong><br>The level ID.</td>
  </tr>
  <tr>
    <td><code>type</code></td>
    <td><strong><a href="#levelleveltype">LevelType</a></strong><br>The level type.</td>
  </tr>
</tbody>
</table>

### Level.`createLevel`
Create a new local level.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong><br>The level name.</td>
  </tr>
  <tr>
    <td><code>description</code><br>(optional)</td>
    <td><strong>string</strong><br>The level description.</td>
  </tr>
</tbody>
</table>

## Events
### Level.`listCreated`
Fired when a new list was created

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>list</code></td>
    <td><strong><a href="#levellevellist">LevelList</a></strong></td>
  </tr>
</tbody>
</table>

### Level.`listDeleted`
Fired when a list was deleted

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>list</code></td>
    <td><strong><a href="#levellevellist">LevelList</a></strong></td>
  </tr>
</tbody>
</table>

### Level.`levelCreated`
Fired when a new level was created

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>level</code></td>
    <td><strong><a href="#levellevel">Level</a></strong></td>
  </tr>
</tbody>
</table>

### Level.`levelDeleted`
Fired when a level was deleted

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>level</code></td>
    <td><strong><a href="#levellevel">Level</a></strong></td>
  </tr>
</tbody>
</table>

## Types
### Level.`LevelList`
yo list

<table>
<thead>
<tr>
<th colspan="2">Properties</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>type</code></td>
    <td><strong>string</strong><br>The list type. This indicate which getList function to use.</td>
  </tr>
  <tr>
    <td><code>id</code></td>
    <td><strong>integer</strong><br>The list ID.</td>
  </tr>
  <tr>
    <td><code>downloads</code><br>(optional)</td>
    <td><strong>integer</strong><br>Downloads count</td>
  </tr>
  <tr>
    <td><code>author</code></td>
    <td><strong>string</strong><br>The creator of the list.</td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong></td>
  </tr>
  <tr>
    <td><code>levels</code></td>
    <td><strong>array</strong><br>list of levels, empty if includeLevels is false</td>
  </tr>
</tbody>
</thead>
</table>

### Level.`LevelType`


### Level.`Level`
yo level

<table>
<thead>
<tr>
<th colspan="2">Properties</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>type</code></td>
    <td><strong><a href="#levelleveltype">LevelType</a></strong><br>The level type.</td>
  </tr>
  <tr>
    <td><code>id</code></td>
    <td><strong>integer</strong><br>The level ID.</td>
  </tr>
  <tr>
    <td><code>downloads</code><br>(optional)</td>
    <td><strong>integer</strong><br>Downloads count</td>
  </tr>
  <tr>
    <td><code>author</code></td>
    <td><strong>string</strong><br>The creator of the level.</td>
  </tr>
  <tr>
    <td><code>name</code></td>
    <td><strong>string</strong></td>
  </tr>
</tbody>
</thead>
</table>


# `Mod` Domain
Manages a mod.

## Methods
### Mod.`disableMod`
Disables a specific mod.
It won't actually be disabled until you [restart](domain:Game) the game.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>mod</code></td>
    <td><strong>string</strong><br>The mod ID</td>
  </tr>
</tbody>
</table>

### Mod.`enableMod`
Enables a specific mod.
It won't actually be enabled until you [restart](domain:Game) the game.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>mod</code></td>
    <td><strong>string</strong><br>The mod ID</td>
  </tr>
</tbody>
</table>

### Mod.`getSettingsItems`
Get non-custom(?) settings values

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>mod</code></td>
    <td><strong>string</strong><br>The mod ID</td>
  </tr>
  <tr>
    <td><code>keys</code><br>(optional)</td>
    <td><strong>array</strong><br>Limits return values to these keys</td>
  </tr>
</tbody>
</table>

### Mod.`setSettings`
Set non-custom(?) settings values

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>mod</code></td>
    <td><strong>string</strong><br>The mod ID</td>
  </tr>
  <tr>
    <td><code>data</code></td>
    <td><strong>object</strong><br>Settings</td>
  </tr>
</tbody>
</table>


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

### Game.`getWindowBounds`
Get position and size of the game window.

### Game.`getWindowBounds`
Set position and/or size of the game window.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>bounds</code></td>
    <td><strong><a href="#gamebounds">Bounds</a></strong></td>
  </tr>
</tbody>
</table>

## Types
### Game.`Bounds`
Game window bounds information

<table>
<thead>
<tr>
<th colspan="2">Properties</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>left</code></td>
    <td><strong>integer</strong></td>
  </tr>
  <tr>
    <td><code>top</code></td>
    <td><strong>integer</strong></td>
  </tr>
  <tr>
    <td><code>width</code></td>
    <td><strong>integer</strong></td>
  </tr>
  <tr>
    <td><code>height</code></td>
    <td><strong>integer</strong></td>
  </tr>
  <tr>
    <td><code>windowState</code></td>
    <td><strong>string</strong></td>
  </tr>
</tbody>
</thead>
</table>


# `Input` Domain
User inputs.

## Methods
### Input.`dispatchMouseEvent`
Dispatches a mouse event to the game.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>timestamp</code></td>
    <td><strong>integer</strong><br>Time at which the event occured.</td>
  </tr>
  <tr>
    <td><code>type</code></td>
    <td><strong>string</strong><br>Mouse event type.</td>
  </tr>
  <tr>
    <td><code>x</code></td>
    <td><strong>number</strong><br>Mouse X position</td>
  </tr>
  <tr>
    <td><code>y</code></td>
    <td><strong>number</strong><br>Mouse Y position</td>
  </tr>
  <tr>
    <td><code>deltaX</code></td>
    <td><strong>number</strong><br>Mouse horizontal scroll delta</td>
  </tr>
  <tr>
    <td><code>deltaY</code></td>
    <td><strong>number</strong><br>Mouse vertical scroll delta</td>
  </tr>
  <tr>
    <td><code>button</code></td>
    <td><strong>string</strong><br>Mouse button</td>
  </tr>
</tbody>
</table>

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
    <td><code>timestamp</code></td>
    <td><strong>integer</strong><br>Time at which the event occured.</td>
  </tr>
  <tr>
    <td><code>type</code></td>
    <td><strong>string</strong></td>
  </tr>
  <tr>
    <td><code>modifiers</code></td>
    <td><strong>integer</strong></td>
  </tr>
  <tr>
    <td><code>code</code></td>
    <td><strong>string</strong></td>
  </tr>
</tbody>
</table>

### Input.`setIgnoreInputEvents`


<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>ignore</code></td>
    <td><strong>boolean</strong></td>
  </tr>
</tbody>
</table>


# `Network` Domain
Network domain allows tracking network activities of the page. It exposes information about http, file, data and other requests and responses, their headers, bodies, timing, etc.

## Methods
### Network.`disable`
Disables Network agent.

### Network.`enable`
Enables Network agent.

### Network.`getRequestPostData`


<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>requestId</code></td>
    <td><strong>integer</strong></td>
  </tr>
</tbody>
</table>


# `Runtime` Domain
javascriptin

## Methods
### Runtime.`evaluate`
Evaluate the given JavaScript expression.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>expression</code></td>
    <td><strong>string</strong></td>
  </tr>
</tbody>
</table>


