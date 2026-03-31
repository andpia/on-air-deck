#include "WebUIComponent.hpp"

#include <unordered_map>

// WebView2 runtime detection – only available on Windows when the SDK is present.
// The static loader (WebView2LoaderStatic.lib) exposes
// GetAvailableCoreWebView2BrowserVersionString which returns S_OK when the
// WebView2 Runtime (Edge) is installed on the machine.
#if JUCE_WINDOWS && JUCE_WEB_BROWSER_RESOURCE_PROVIDER_AVAILABLE
 #include <WebView2.h>

static bool isWebView2RuntimeAvailable()
{
    LPWSTR versionInfo = nullptr;
    const HRESULT hr   = GetAvailableCoreWebView2BrowserVersionString (nullptr, &versionInfo);
    const bool available = SUCCEEDED (hr) && versionInfo != nullptr;
    if (versionInfo != nullptr)
        CoTaskMemFree (versionInfo);
    return available;
}
#endif

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
    {
        DBG ("WebUIComponent: resource provider root missing -> " + webUIDir.getFullPathName());
        return std::nullopt;
    }

    auto normalized = path.upToFirstOccurrenceOf ("?", false, false);

    if (normalized.isEmpty() || normalized == "/")
        normalized = "/index.html";

    while (normalized.startsWithChar ('/'))
        normalized = normalized.substring (1);

    if (normalized.contains (".."))
    {
        DBG ("WebUIComponent: blocked suspicious resource path -> " + path);
        return std::nullopt;
    }

    const auto requestedFile = webUIDir.getChildFile (normalized);

    if (! requestedFile.existsAsFile())
    {
        DBG ("WebUIComponent: missing resource -> " + requestedFile.getFullPathName());
        return std::nullopt;
    }

    juce::MemoryBlock block;
    if (! requestedFile.loadFileAsData (block))
    {
        DBG ("WebUIComponent: failed to read resource -> " + requestedFile.getFullPathName());
        return std::nullopt;
    }

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
    auto options = juce::WebBrowserComponent::Options {};

   // withResourceProvider is only available on macOS/iOS/Linux and Windows+WebView2.
   // On Windows without WebView2 (IE backend) the macro evaluates to 0 and the API
   // does not exist, so we fall back to loading the files via a file:// URL instead.
   #if JUCE_WEB_BROWSER_RESOURCE_PROVIDER_AVAILABLE
    options = options.withResourceProvider (
        [] (const juce::String& path)
        {
            return provideWebUIResource (path);
        });
   #endif

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
   // On Windows the resource-provider URL (https://juce.backend/) is only
   // reachable when the WebView2 Runtime (Edge) is installed.  Without it
   // JUCE silently falls back to the legacy MSHTML/IE engine, which cannot
   // resolve that origin and shows an opaque "Navigation cancelled" page.
   // Also, when the WebView2 SDK was not compiled in at all, JUCE always uses
   // the IE backend, which cannot run modern ES-module JavaScript regardless
   // of the URL used.  In either case, detect the situation early and show a
   // clear, actionable message instead of a broken blank page.
   #if JUCE_WINDOWS
    #if JUCE_WEB_BROWSER_RESOURCE_PROVIDER_AVAILABLE
    const bool needsWebView2Alert = ! isWebView2RuntimeAvailable();
    #else
    // WebView2 SDK was not compiled in → IE backend is always used.
    const bool needsWebView2Alert = true;
    #endif
    if (needsWebView2Alert)
    {
        juce::MessageManager::callAsync ([]
        {
            juce::AlertWindow::showMessageBoxAsync (
                juce::MessageBoxIconType::WarningIcon,
                "Microsoft Edge WebView2 Runtime Required",
                "On Air Deck requires the Microsoft Edge WebView2 Runtime "
                "to display its user interface.\n\n"
                "Please download and install it from:\n"
                "  https://go.microsoft.com/fwlink/p/?LinkId=2124703\n\n"
                "Restart the application after installation.",
                "OK");
        });
        return; // Do not navigate; leave the browser pane blank.
    }
   #endif

    startupURL = getStartupURL();
    DBG ("WebUIComponent: startup URL -> " + startupURL);
    goToURL (startupURL);
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
    mainPageLoaded = true;
    DBG ("WebUIComponent: page loaded -> " + url);
}

bool WebUIComponent::pageLoadHadNetworkError (const juce::String& errorInfo)
{
    DBG ("WebUIComponent: network error callback -> " + errorInfo);

    // This callback may also be triggered by secondary resources (favicon,
    // source maps, etc.). Once the main page is up, keep the UI alive.
    if (mainPageLoaded)
    {
        DBG ("WebUIComponent: non-fatal subresource load error -> " + errorInfo);
        return false;
    }

    if (fatalLoadErrorShown)
        return false;

    fatalLoadErrorShown = true;

    const auto webUIDir = getBundledWebUIDirectory();
    const auto indexPath = webUIDir.getChildFile ("index.html").getFullPathName();
    DBG ("WebUIComponent: fatal load error; startup URL -> " + startupURL);
    DBG ("WebUIComponent: fatal load error; WebUI dir -> " + webUIDir.getFullPathName());
    DBG ("WebUIComponent: fatal load error; index path -> " + indexPath);

    // Build a self-contained error page.  We intentionally avoid a data: URI
    // because the legacy IE/MSHTML backend may block navigation to data: URLs
    // depending on the active security zone.  Writing a physical temp file and
    // using a file:// URL lets even the IE fallback render the error.
    //
    // Use a unique filename to prevent symlink-based race conditions.
    const juce::File errFile = juce::File::createTempFile ("_onairdeck_load_error.html");

    // Escape all HTML-significant characters before embedding in the page.
    const juce::String safeError = errorInfo
        .replace ("&",  "&amp;")
        .replace ("<",  "&lt;")
        .replace (">",  "&gt;")
        .replace ("\"", "&quot;")
        .replace ("'",  "&#x27;");

    const juce::String html =
        "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:24px'>"
        "<h2>Web UI load failed</h2>"
        "<p>The embedded browser could not load one or more resources.</p>"
        "<pre style='white-space:pre-wrap'>"
        + safeError
        + "</pre></body></html>";

    if (errFile.replaceWithText (html))
    {
        goToURL (juce::URL (errFile).toString (false));
    }
    else
    {
        // Last-resort: try the data: URI anyway (works with WebView2 / WebKit).
        DBG ("WebUIComponent: could not write temp error file at "
             + errFile.getFullPathName() + "; falling back to data: URI");
        const auto escapedError = juce::URL::addEscapeChars (errorInfo, false);
        goToURL ("data:text/html;charset=UTF-8,"
                 "<html><body style='font-family:sans-serif;padding:24px'>"
                 "<h2>Web UI load failed</h2>"
                 "<p>The embedded browser could not load one or more resources.</p>"
                 "<pre style='white-space:pre-wrap'>" + escapedError + "</pre>"
                 "</body></html>");
    }

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
    const auto overrideURL = juce::SystemStats::getEnvironmentVariable ("ONAIRDECK_WEBUI_URL", {});
    if (overrideURL.isNotEmpty())
    {
        DBG ("WebUIComponent: using ONAIRDECK_WEBUI_URL override -> " + overrideURL);
        return overrideURL;
    }

    const auto webUIDir = getBundledWebUIDirectory();
    const juce::File indexFile = webUIDir.getChildFile ("index.html");
    DBG ("WebUIComponent: probing bundled index -> " + indexFile.getFullPathName());

    if (indexFile.existsAsFile())
    {
       // On platforms with resource provider support the files are served through
       // JUCE's internal backend origin (juce:// or https://juce.backend/).
       // On Windows with the IE backend (no WebView2) we fall back to the file:// URL.
       #if JUCE_WEB_BROWSER_RESOURCE_PROVIDER_AVAILABLE
        return juce::WebBrowserComponent::getResourceProviderRoot();
       #else
        return juce::URL(indexFile).toString(false);
       #endif
    }

#if JUCE_DEBUG
    // Development fallback: use the Vite dev server for hot-reload support.
    DBG ("WebUIComponent: bundled index missing; using debug dev server -> " + juce::String (WEBUI_DEV_SERVER_URL));
    return WEBUI_DEV_SERVER_URL;
#else
    // Fallback: inline error page shown when the WebUI folder is missing.
    DBG ("WebUIComponent: bundled index missing in release; showing inline error page");
    return "data:text/html,<h2>Web UI not found</h2>"
           "<p>Expected index.html at: " + indexFile.getFullPathName() + "</p>"
           "<p>Run <code>npm run build</code> in the on-air-deck-figma repo "
           "and pass <code>-DWEBUI_DIST_PATH=/path/to/dist</code> to CMake.</p>";
#endif
}