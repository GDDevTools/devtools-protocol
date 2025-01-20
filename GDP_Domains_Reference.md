GD DevTools Protocol Domains Reference (INCOMPLETE)
---

Except from some changes (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

This .md file is generated from protocols.json so it's guaranteed to match with the protocols list returned.

> **Note 2** most of this docs will sound similar to the docs from the aforementioned protocol and it's intended. Don't ask anything about that.


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
    <td><strong><a href="#domnodeid">NodeID</a></strong><br>The node id. Must be a button node</td>
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


# `Levels` Domain
Manages the levels list.

## Methods
### Levels.`disable`
Disables the Levels agent.

### Levels.`enable`
Enables the Levels agent.

### Levels.`getLocalList`
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
    <td><strong>integer</strong><br>The list ID, can be seen in the game using Editor List ID API (installed as required).</td>
  </tr>
  <tr>
    <td><code>includeLevels</code><br>(optional)</td>
    <td><strong>boolean</strong><br>Bundle the levels with the response. Defaults to false.</td>
  </tr>
</tbody>
</table>

### Levels.`getSavedList`
Get a saved online list.

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
    <td><code>includeLevels</code><br>(optional)</td>
    <td><strong>boolean</strong><br>Bundle the levels with the response. Defaults to false.</td>
  </tr>
</tbody>
</table>

### Levels.`getLocalLevel`
Get a local level.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>id</code></td>
    <td><strong>integer</strong><br>The level ID, can be seen in the game using Editor Level ID API (installed as required)</td>
  </tr>
</tbody>
</table>

### Levels.`getSavedLevel`
Get a saved online level.

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
</tbody>
</table>

### Levels.`getSavedLevels`
Get some online level.

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

## Events
### Levels.`listCreated`
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
    <td><strong><a href="#levelslevellist">LevelList</a></strong></td>
  </tr>
</tbody>
</table>

### Levels.`listDeleted`
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
    <td><strong><a href="#levelslevellist">LevelList</a></strong></td>
  </tr>
</tbody>
</table>

### Levels.`levelCreated`
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
    <td><strong><a href="#levelslevel">Level</a></strong></td>
  </tr>
</tbody>
</table>

### Levels.`levelDeleted`
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
    <td><strong><a href="#levelslevel">Level</a></strong></td>
  </tr>
</tbody>
</table>

## Types
### Levels.`LevelList`
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

### Levels.`Level`
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
    <td><strong>string</strong><br>The level type. This indicate which getLevel function to use.</td>
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


# `Mods` Domain
Manages a mod.

## Methods
### Mods.`disableMod`
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

### Mods.`enableMod`
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

### Mods.`getSettingsItems`
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

### Mods.`setSettings`
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


