/*!
  \file zoombar.js This module contains the ZoomBar class.
*/

/*!
  Class to handle displaying the zoom bar. The zoom bar is displayed when the
  user presses the zoom button on the toolbar. It provides access to the zoom
  in and out functions. It is hidden when the main toolbar is hidden on user inactivity
  and on timer maintained by native code
*/
function ZoomBar()
{
    var enabled = true; // zooming enabled flag - initially enabled

    // Private Methods
    //! Write zoom bar HTML code to document.
    function _zoombar_write() {
        var html = ''+
            '<div id="zoomBarDiv">'+
            '<div class = "GinebraSnippet zoomBarBtn" id="zoomBarBtnIn" data-GinebraVisible="false" data-GinebraHidesContent="true"   data-GinebraNativeClass="ActionButton"> </div>'+
            '<div class = "GinebraSnippet zoomBarBtn" id="zoomBarBtnOut" data-GinebraVisible="false" data-GinebraHidesContent="true"   data-GinebraNativeClass="ActionButton"> </div>'+

            '</div>';
        document.write(html);
    }

    //! Create zoom in & out buttons.
    function _setActions() {
        snippets.zoomBarBtnIn.connectAction("zoomIn", "WebView", true, true);
        snippets.zoomBarBtnOut.connectAction("zoomOut", "WebView",  true, true);
        snippets.zoomBarBtnIn.setIcon(":/chrome/bedrockchrome/zoombar.snippet/icons/icon_zoom+.png");
        snippets.zoomBarBtnIn.setActiveIcon(":/chrome/bedrockchrome/zoombar.snippet/icons/icon_zoom+_pressed.png");
        snippets.zoomBarBtnIn.setDisabledIcon(":/chrome/bedrockchrome/zoombar.snippet/icons/icon_zoom+_disabled.png");
        snippets.zoomBarBtnOut.setIcon(":/chrome/bedrockchrome/zoombar.snippet/icons/icon_zoom-.png");
        snippets.zoomBarBtnOut.setActiveIcon(":/chrome/bedrockchrome/zoombar.snippet/icons/icon_zoom-_pressed.png");
        snippets.zoomBarBtnOut.setDisabledIcon(":/chrome/bedrockchrome/zoombar.snippet/icons/icon_zoom-_disabled.png");

        // Connect to each action's "changed" signal.
        window.views.WebView.actions.zoomIn.changed.connect(
            createDelegate(this, handleChange));
        window.views.WebView.actions.zoomOut.changed.connect(
            createDelegate(this, handleChange));

    }

    //! Handle action object changes. In particular we are interested in
    //! changes to the enabled state of the object.
    function handleChange() {
        var saveEnabled = enabled;

        // enable zoom button if either zoom-in or zoom-out action enabled
        if (window.views.WebView.actions.zoomIn.enabled
            || window.views.WebView.actions.zoomOut.enabled) {
            enabled = true;
        } else {
            enabled = false;
        }

        // if state changed update web toolbar zoom button state
        if (saveEnabled != enabled) {
            // enabled state changed
            window.snippets.ZoomButtonSnippet.setEnabled(enabled);
        }
    }

    //! After chrome is loaded, create zoombar buttons and setup show/hide
    //! handlers.
    function _chromeLoadComplete() {
        _setActions();
    }

    _zoombar_write();

    window.chrome.chromeComplete.connect(_chromeLoadComplete);
}




