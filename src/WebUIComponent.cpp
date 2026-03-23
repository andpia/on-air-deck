#include "WebUIComponent.h"

#include <unordered_map>

juce::File WebUIComponent::getBundledWebUIDirectory()
{
   #if JUCE_MAC
    return juce::File::getSpecialLocation (juce::File::currentApplicationFile)
        .getChildFile ("Contents/Resources/WebUI");
   #else
    return juce::File::getSpecialLocation (juce::File::currentExecutableFile)
        .getSiblingFile ("WebUI");
   #endif
}

std::optional<juce::WebBrowserComponent::Resource> WebUIComponent::provideWebUIResource (const juce::String& path)
{
    const auto webUIDir = getBundledWebUIDirectory();

    if (! webUIDir.isDirectory())
        return std::nullopt;

    auto normalized = path.upToFirstOccurrenceOf ("?", false, false);

    if (normalized.isEmpty() || normalized == "/")
        normalized = "/index.html";

    while (normalized.startsWithChar ('/'))
        normalized = normalized.substring (1);

    if (normalized.contains (".."))
        return std::nullopt;

    const auto requestedFile = webUIDir.getChildFile (normalized);

    if (! requestedFile.existsAsFile())
        return std::nullopt;

    juce::MemoryBlock block;
    if (! requestedFile.loadFileAsData (block))
        return std::nullopt;

    static const std::unordered_map<std::string, juce::String> mimeByExtension {
        { ".html", "text/html; charset=utf-8" },
        { ".js", "text/javascript; charset=utf-8" },
        { ".mjs", "text/javascript; charset=utf-8" },
        { ".css", "text/css; charset=utf-8" },
        { ".json", "application/json; charset=utf-8" },
        { ".map", "application/json; charset=utf-8" },
        { ".svg", "image/svg+xml" },
        { ".png", "image/png" },
        { ".jpg", "image/jpeg" },
        { ".jpeg", "image/jpeg" },
        { ".gif", "image/gif" },
        { ".webp", "image/webp" },
        { ".ico", "image/x-icon" },
        { ".woff", "font/woff" },
        { ".woff2", "font/woff2" },
        { ".ttf", "font/ttf" },
        { ".otf", "font/otf" }
    };

    const auto extension = requestedFile.getFileExtension().toLowerCase().toStdString();

    const auto mimeIt = mimeByExtension.find (extension);
    const auto mime = (mimeIt != mimeByExtension.end()) ? mimeIt->second : juce::String ("application/octet-stream");

    const auto* data = static_cast<const std::byte*> (block.getData());

    juce::WebBrowserComponent::Resource resource;
    resource.mimeType = mime;
    resource.data.assign (data, data + block.getSize());

    return resource;
}

juce::WebBrowserComponent::Options WebUIComponent::createBrowserOptions()
{
    auto options = juce::WebBrowserComponent::Options {}
        .withResourceProvider (
            [] (const juce::String& path)
            {
                return provideWebUIResource (path);
            });

   #if JUCE_MAC || JUCE_IOS
    options = options.withAppleWkWebViewOptions (
        juce::WebBrowserComponent::Options::AppleWkWebView {}
            .withAllowAccessToEnclosingDirectory (true));
   #endif

    return options;
}

WebUIComponent::WebUIComponent()
    : juce::WebBrowserComponent (createBrowserOptions())
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

bool WebUIComponent::pageLoadHadNetworkError (const juce::String& errorInfo)
{
    const auto escapedError = juce::URL::addEscapeChars (errorInfo, false);

    goToURL ("data:text/html;charset=UTF-8,"
             "<html><body style='font-family:sans-serif;padding:24px'>"
             "<h2>Web UI load failed</h2>"
             "<p>The embedded browser could not load one or more resources.</p>"
             "<pre style='white-space:pre-wrap'>" + escapedError + "</pre>"
             "</body></html>");

    return false;
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
    // Prefer bundled resources served through JUCE's internal backend origin.
    const auto webUIDir = getBundledWebUIDirectory();
    const juce::File indexFile = webUIDir.getChildFile ("index.html");

    if (indexFile.existsAsFile())
        return juce::WebBrowserComponent::getResourceProviderRoot();

#if JUCE_DEBUG
    // Development fallback: use the Vite dev server for hot-reload support.
    return WEBUI_DEV_SERVER_URL;
#else
    // Fallback: inline error page shown when the WebUI folder is missing.
    return "data:text/html,<h2>Web UI not found</h2>"
           "<p>Expected index.html at: " + indexFile.getFullPathName() + "</p>"
           "<p>Run <code>npm run build</code> in the on-air-deck-figma repo "
           "and pass <code>-DWEBUI_DIST_PATH=/path/to/dist</code> to CMake.</p>";
#endif
}
