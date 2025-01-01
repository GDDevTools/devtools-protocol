GD DevTools Protocol Domains Reference (INCOMPLETE)
---

Except from some derivations (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

This .md file is generated from protocols.json so it's guaranteed to match with the protocols list returned.

> **Note** that this reference ==only documents what's currently available in the mod itself==, so don't expect every features of CDP available (especially on the early releases of the mod)
> 
> **Note 2** most of this docs will sound similar to the docs from the aforementioned protocol and it's intended. Don't ask anything about that.


<style type="text/css">
  tbody>tr>td:first-child {text-align: right}
</style>

# `DOM` Domain
This domain exposes DOM read/write operations. Each DOM Node is represented with its mirror object that has an `id`. This `id` can be used to get additional information on the Node, and more. It is important that client receives DOM events only for the nodes that are known to the client. Backend keeps track of the nodes that were sent to the client and never sends the same node twice. It is client's responsibility to collect information about the nodes that were sent to the client.

## Methods
### DOM.`click`
Try sending a click to the node without actually clicking. Only works for `button` nodes.

First it tries to cast the node to `CCMenuItem` and clicks it. If it isn't then it will send a `DispatchEvent("puppeteer/nodeClicked")` event to the game so nodes implmenting that can handle as it needs.

If you want real click events then you can use [Input.`dispatchMouseEvent`](#inputdispatchmouseevent) instead.

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
    <td><strong>Node</strong><br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0.</td>
  </tr>
</tbody>

<thead>
<tr>
<th colspan="2">Return Values</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>node</code></td>
    <td><strong><a href="#domnode">Node</a></strong><br>Node description.</td>
  </tr>
</tbody>
</table>

### DOM.`disable`
Disables DOM agent.

### DOM.`enable`
Enables DOM agent.


# `Log` Domain
Get events whenever a log is pushed

## Methods
### Log.`disable`
Disables the domain.

### Log.`enable`
Enables the domain.


# `Log` Domain
Get events whenever a log is pushed

## Methods
### Log.`disable`
Disables the domain.

### Log.`enable`
Enables the domain.

## Events
### Log.`entryAdded`
Issued when new message was logged.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>entry</code></td>
    <td><strong><a href="#loglogentry">LogEntry</a></strong><br>The log entry.</td>
  </tr>
</tbody>
</table>


# `Mods` Domain
Manages a mod.

## Methods
### Mods.`disableMod`
Disables a specific mod.  
It won't actually be disabled until you [restart](#game-domain) the game.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>mod</code></td>
    <td><strong>string</strong><br>The mod id<br>Allowed Values: <code></code></td>
  </tr>
</tbody>
</table>

### Mods.`enableMod`
Enables a specific mod.  
It won't actually be rnabled until you [restart](#game-domain) the game.

<table>
<thead>
<tr>
<th colspan="2">Parameters</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>mod</code></td>
    <td><strong>string</strong><br>The mod id<br>Allowed Values: <code></code></td>
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
    <td><strong>string</strong><br>The mod id<br>Allowed Values: <code></code></td>
  </tr>
  <tr>
    <td><code>keys</code></td>
    <td><strong>array</strong><br>Limits return values to these keys</td>
  </tr>
</tbody>

<thead>
<tr>
<th colspan="2">Return Values</th>
</tr>
</thead>
<tbody>
  <tr>
    <td><code>data</code></td>
    <td><strong>object</strong></td>
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
    <td><strong>string</strong><br>The mod id<br>Allowed Values: <code></code></td>
  </tr>
  <tr>
    <td><code>settings</code></td>
    <td><strong>object</strong></td>
  </tr>
</tbody>
</table>


