#include "WebUIComponent.h"

WebUIComponent::WebUIComponent()
{
    goToURL (getStartupURL());
}

bool WebUIComponent::pageAboutToLoad (const juce::String& newURL)
{
    // Intercept the custom "myapp://" URL scheme for the JS to JUCE bridge.
    if (newURL.startsWith ("myapp://"))
    {
        // Format: myapp://<action>[?key=value&key=value...]
        const juce::String withoutScheme = newURL.substring (8); // strip "myapp://"
        const juce::String action        = withoutScheme.upToFirstOccurrenceOf ("?", false, false);
        const juce::String queryString   = withoutScheme.fromFirstOccurrenceOf ("?", false, false);

        juce::StringPairArray params;

        if (queryString.isNotEmpty())
        {
            for (const auto& pair : juce::StringArray::fromTokens (queryString, "&", ""))
            {
                const auto key   = juce::URL::removeEscapeChars (pair.upToFirstOccurrenceOf ("=", false, false));
                const auto value = juce::URL::removeEscapeChars (pair.fromFirstOccurrenceOf ("=", false, false));
                params.set (key, value);
            }
        }

        handleBridgeAction (action, params);
        return false; // Prevent the browser from navigating away
    }

    return true; // Allow all other URLs
}

void WebUIComponent::pageFinishedLoading (const juce::String& url)
{
    DBG ("WebUIComponent: page loaded -> " + url);
}

void WebUIComponent::handleBridgeAction (const juce::String& action,
                                         const juce::StringPairArray& params)
{
    DBG ("WebUIComponent: bridge action -> " + action);

    if (action == "setVolume")
    {
        const float volume = params["value"].getFloatValue();
        if (volume < 0.0f || volume > 1.0f)
        {
            DBG ("WebUIComponent: setVolume -> value out of range [0,1]: " + juce::String (volume));
            return;
        }
        DBG ("WebUIComponent: setVolume -> " + juce::String (volume));
        // TODO: forward to the audio engine
    }
    else if (action == "play")
    {
        DBG ("WebUIComponent: play");
        // TODO: forward to the audio engine
    }
    else if (action == "stop")
    {
        DBG ("WebUIComponent: stop");
        // TODO: forward to the audio engine
    }
    else
    {
        DBG ("WebUIComponent: unknown action -> " + action);
    }
}

juce::String WebUIComponent::getStartupURL()
{
#if JUCE_DEBUG
    // Development: connect to the Vite dev server for hot-reload support.
    return WEBUI_DEV_SERVER_URL;
#else
    // Production: load the bundled index.html from the WebUI/ folder.
    juce::File webUIDir;

  #if JUCE_MAC
    // On macOS, look inside the .app bundle's Resources folder.
    webUIDir = juce::File::getSpecialLocation (juce::File::currentApplicationFile)
                   .getChildFile ("Contents/Resources/WebUI");
  #else
    // On Windows and Linux, look next to the executable.
    webUIDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile)
                   .getSiblingFile ("WebUI");
  #endif

    const juce::File indexFile = webUIDir.getChildFile ("index.html");

    if (indexFile.existsAsFile())
        return juce::URL (indexFile).toString (false);

    // Fallback: inline error page shown when the WebUI folder is missing.
    return "data:text/html,<h2>Web UI not found</h2>"
           "<p>Expected index.html at: " + indexFile.getFullPathName() + "</p>"
           "<p>Run <code>npm run build</code> in the on-air-deck-figma repo "
           "and pass <code>-DWEBUI_DIST_PATH=/path/to/dist</code> to CMake.</p>";
#endif
}
