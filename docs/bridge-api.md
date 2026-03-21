# Bridge API

The frontend communicates with JUCE by navigating to a custom URL scheme handled inside `WebUIComponent::pageAboutToLoad()`.

## URL Format

```text
myapp://<action>?key=value&key=value
```

JUCE intercepts the navigation, parses the query string, dispatches the action, and cancels the browser navigation so the page remains loaded.

## Current Actions

### setVolume

```javascript
window.location = 'myapp://setVolume?value=0.75';
```

- Expected range: `0.0` to `1.0`
- Invalid values are ignored

### play

```javascript
window.location = 'myapp://play';
```

### stop

```javascript
window.location = 'myapp://stop';
```

## Adding a New Action

1. Trigger a new `myapp://...` URL from the frontend.
2. Add a new action branch in `WebUIComponent::handleBridgeAction()`.
3. Parse any expected query parameters from the `juce::StringPairArray`.
4. Forward the action to the relevant JUCE subsystem.