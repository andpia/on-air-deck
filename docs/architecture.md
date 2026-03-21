# Architecture

OnAirDeck currently has a deliberately small native shell around an embedded web frontend.

## Runtime Structure

The application starts from a JUCE `JUCEApplication` subclass that creates a single desktop window.

```text
OnAirDeckApplication
  -> MainWindow
    -> MainComponent
      -> WebUIComponent
        -> Embedded Web UI
```

## Native Entry Point

The app entry point lives in `src/Main.cpp`.

- `OnAirDeckApplication` owns the application lifecycle.
- `MainWindow` derives from `juce::DocumentWindow`.
- The main window owns `MainComponent` as its content component.

This keeps the desktop host thin and pushes most UI concerns into the embedded frontend.

## Main Component

`MainComponent` is a JUCE `Component` whose only current responsibility is layout.

- It creates one `WebUIComponent`.
- It sizes that browser component to fill the full client area.
- It leaves background painting minimal because the browser covers the window.

## Web UI Host

`WebUIComponent` derives from `juce::WebBrowserComponent` and is the main integration boundary between native code and the frontend.

Responsibilities:

- Choose the startup URL depending on build mode.
- Load the Vite server in Debug.
- Load bundled static files in Release.
- Intercept `myapp://` URLs before the browser navigates.
- Dispatch frontend actions into JUCE-side handlers.

## Debug vs Release Loading

### Debug

In Debug builds the browser loads the URL from `WEBUI_DEV_SERVER_URL`.

Default:

```text
http://localhost:5173
```

This is intended for frontend development with Vite hot reload.

### Release

In Release builds the browser loads `index.html` from packaged frontend assets.

- macOS: inside `Contents/Resources/WebUI`
- Windows/Linux: in a `WebUI` directory next to the executable

If the packaged files are missing, the app falls back to an inline HTML error page.

## Bridge Boundary

Frontend to native communication currently uses a URL-based bridge.

```text
myapp://action?key=value
```

The bridge is handled inside `pageAboutToLoad()`:

1. detect `myapp://`
2. parse action and query string
3. decode values
4. call `handleBridgeAction()`
5. cancel navigation

Current actions are intentionally small:

- `setVolume`
- `play`
- `stop`

These actions currently log intent with `DBG` and mark the places where the future audio engine integration should connect.

## Current Design Tradeoff

This structure optimizes iteration speed on the frontend while keeping the native shell straightforward.

Benefits:

- fast UI iteration in Debug
- minimal native layout code
- clear single integration point for browser/native communication

Current limitations:

- no dedicated audio engine layer is wired yet
- no typed bridge contract beyond URL conventions
- browser error reporting is still basic