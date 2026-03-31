#pragma once

#include <JuceHeader.h>

/**
    @class WebUIComponent
    @brief Embeds the On Air Deck Web UI using JUCE's WebBrowserComponent.

    Behaviour:
    - Debug builds: first tries bundled index.html from WebUI/ and falls
      back to the Vite dev server at WEBUI_DEV_SERVER_URL
      (default: http://localhost:5173) for hot-reload.
    - Release builds: loads index.html from a WebUI/ folder placed next to
      the executable (or in Contents/Resources/WebUI/ on macOS .app bundles).
      Falls back to an inline error page when the file cannot be found.

    JS to JUCE bridge: JavaScript navigates to "myapp://<action>?<params>" to
    trigger JUCE-side handlers.  The bridge is intercepted in pageAboutToLoad()
    before any network request is made.  See handleBridgeAction() for details.
*/
class WebUIComponent : public juce::WebBrowserComponent
{
public:
    WebUIComponent();
    ~WebUIComponent() override = default;

    /** @copydoc juce::WebBrowserComponent::pageAboutToLoad */
    bool pageAboutToLoad (const juce::String& newURL) override;

    /** @copydoc juce::WebBrowserComponent::pageFinishedLoading */
    void pageFinishedLoading (const juce::String& url) override;

    /** @copydoc juce::WebBrowserComponent::pageLoadHadNetworkError */
    bool pageLoadHadNetworkError (const juce::String& errorInfo) override;

private:
  // Last startup URL requested by the component.
  juce::String startupURL;

  // Becomes true after the first successful top-level page load callback.
  bool mainPageLoaded = false;

  // Prevent repeatedly replacing the page with the same error UI.
  bool fatalLoadErrorShown = false;

    /** Returns the expected bundled WebUI directory for the current platform. */
    static juce::File getBundledWebUIDirectory();

    /** Serves WebUI assets through JUCE's ResourceProvider. */
    static std::optional<juce::WebBrowserComponent::Resource> provideWebUIResource (const juce::String& path);

    /** Builds browser options suitable for bundled local assets. */
    static juce::WebBrowserComponent::Options createBrowserOptions();

    /** Dispatches an action received from the JavaScript bridge.
        @param action  The action name (host component of the myapp:// URL).
        @param params  Key-value pairs parsed from the URL query string.
    */
    void handleBridgeAction (const juce::String& action,
                             const juce::StringPairArray& params);

    /** Builds the URL to load on start-up. */
    static juce::String getStartupURL();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WebUIComponent)
};
