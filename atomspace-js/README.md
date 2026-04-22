# Archived
There is nothing here, except for this README file.

# atomspace-js
JavaScript bindings to the AtomSpace

## Status
Version 0.0.0 - there is nothing here yet.

## Motivation and General Plan
There is a recurring desire to access the
[AtomSpace](https://wiki.opencog.org/w/AtomSpace) contents from a
web-browser. There are three different ways in which this can be done,
listed below. The first way is the worst, the last way is the best.

* Create a RESTful or RPC or JSON-RPC AtomSpace server, and send selected
  AtomSpace data to the browser. This is a bad design for two reasons:
  (1) it places a large CPU burden on the server to create the formats
  that the browser wants. (2) It forces the browser to remember a large
  number of Atoms, effectively re-creating the concept of the AtomSpace,
  but badly, inside the browser.

* Use the existing [CogServer](https://github.com/opencog/cogserver) for
  network communications. This solves problem (1) above: it's fast, it's
  easy, it works, it's maintained.  It does NOT solve (2).

* Create JavaScript interfaces to the AtomSpace. This avoids problem (1)
  and solves problem (2).  The browser now has complete access to a full
  AtomSpace, with all the bells and whistles and features and functions.
  In order to access remote AtomSpaces, a browser or a JS app can use the
  [CogStorageNode](https://wiki.opencog.org/w/CogStorageNode) to connect
  to other AtomSpaces over the network.

## The API
What should the JavaScript API to the AtomSpace look like? There are two
parts to the API: a representation for Atoms and Values, and a small
handful of **ad hoc** function calls.

Recall that AtomSpace [Values](https://wiki.opencog.org/w/Value) are a
base class for AtomSpace [Atoms](https://wiki.opencog.org/w/Atom). The
Atom class just has some methods that the Value class does not. The
API should resemble the existing C++ API for these two.

Values could look like this:
```
{
	type: function(),      // wrapper around the C++ Value::get_type() method.
	is_type: function(),   // wrapper around Value::is_type()
	is_atom: function(),   // wrapper around Value::is_atom()
	is_node: function(),   // wrapper around Value::is_node()
	is_link: function(),   // wrapper around Value::is_link()
}
```
Atoms:
```
{
	name: function(),      // wrapper around Atom::getName()
	value: function(),     // wrapper around Atom::getValue()
	tv: function(),        // wrapper around Atom::getTruthValue()
	keys: function(),      // wrapper around Atom::getKeys()
	incoming: function(),  // wrapper around Atom::getIncomingSet()
	outgoing: function(),  // wrapper around Atom::getOutgoingSet()
}
```

That's pretty much it, for now.

## Tools
Possible solutions:
* Use [nbind](https://github.com/charto/nbind), which provides JS
  wrappers for C++ that can be used in both browsers and desktop JS apps.
* Use the [node.js](https://nodejs.org/) FFI interfaces.
* Use [emscripten](https://github.com/kripken/emscripten/) to compile
  a small subset of the AtomSpace into WebAssembly.  That subset would
  consist of the Types nameserver (its just one c++ file), the AtomSpace
  itself (just four small/medium C++ files), and the base Value and Atom
  defintions (a handful of small C++ files). This would be enough to
  provide pretty much all of the needed AtomSpace access that a
  javascript app or webapp would need. Except for one thing: the
  `CogStorageNode` is still needed. This would need to be a pure custom
  all-new javascript object, that opens a socket and does the I/O over
  it.  It should not be that hard, but its not trivial.
* Use the cogserver JSON shell. See the [JSON shell README](https://github.com/opencog/atomspace/blob/master/opencog/persist/json/README.md)
  for how to use this. This is a solution of the second form, not the
  third.  It still requires the cogserver to serialize the data.

## The End
If you want to work on this project, contact the opencog mailing list.
Right now, no one is working on this.
