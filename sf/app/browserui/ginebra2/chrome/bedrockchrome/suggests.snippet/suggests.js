/*!
  \file suggests.js This module contains the Suggests class.
*/

/*!
  Class to handle displaying URL suggestions from history and bookmarks.
*/
function Suggests()
{
    // "Private" member variables
    var suggestsXOffset = 20;
    var inputTimeoutId = null;
    var inputTimeoutDelay = _getTimeoutDelaySetting();
    var maxHeight = 0; // maximum height of suggest list
    var urlSearchHeight = 0;
    var urlSnippetId;
    var urlHasFoucus = false; // URL edit field focus flag

    // Orbit UI has a different URL snippet.
    if (app.layoutType() == "tenone") {
        urlSnippetId = "TitleUrlId";
    }
    else {
        urlSnippetId = "UrlSearchChromeId";
    }
    
    // "Private" methods

    //! Calculates the maximum height for the suggest list.
    //! If not all suggest items can fit in list, only half an item should be
    //! visible the end when scrolled to the top. This is to indicate more items.
    function _setMaxHeight()
    {
        // Calculate height of available space for suggest list.
        var statusbarSz = snippets.StatusBarChromeId.getGeometry();
        // The Orbit UI doesn't have a status bar.
        var statusbarHeight = (app.ui() == "orbit_ui") ? 0 : statusbarSz.height;
        var urlSearchSz = snippets[urlSnippetId].getGeometry();
        var toolbarSz = snippets.WebViewToolbarId.getGeometry();
        // leave some space between suggest and toolbar (~10% of display height)
        var bufferHeight = Math.ceil(chrome.displaySize.height / 10);
        var availableHeight = chrome.displaySize.height -
            (statusbarHeight + urlSearchSz.height + toolbarSz.height + bufferHeight);
        // Calculate number of elements that can fit in available space.
        var elementHeight = 70; // set in suggests.css
        var numElements = Math.floor(availableHeight / elementHeight);

        // in order to make room for 1/2 entry at end of list we may
        // need to sacrifice 1 full element.
        if ((availableHeight % elementHeight) < (elementHeight / 2)) {
            numElements -= 1;
        }
        numElements += 0.5; // half of final visible entry
        // max suggest list height in pixels
        maxHeight = Math.floor(numElements * elementHeight);
    }

    //! Temporary substitute for settings function to get the delay for
    //! displaying the URL suggests list.
    function _getTimeoutDelaySetting()
    {
        // no setting stored for this currently
        return (1000); // 1 second
    }

    //! Displays and updates suggest list.
    function _showSuggests()
    {
        // make sure the input is the latest
        var input = snippets[urlSnippetId].url;

        // only display suggestions if there is input
        if (input.length != 0) {
            _updateSuggestList(input);
            this.updateSuggestsSize();

            if (!snippets.SuggestsChromeId.visible && pageController.editMode) {
                window.scrollTo(0, 0);
                // Disable the content view, leave the URL serach bar and status bar enabled.
                views.WebView.enabled = false;
                views.WebView.freeze();
                snippets.SuggestsChromeId.show(false);
            }
        } else {
            // no user input so hide suggestions
            _hideSuggests();
        }
    }

    //! Updates the suggestion list based on the specified input.
    /*!
      \param input the current URL box text
    */
    function _updateSuggestList(input)
    {
        var recenturl;
        var recenttitle = window.localeDelegate.translateText(
            "txt_browser_chrome_suggests_search_for");
        var snippetId = document.getElementById('SuggestsId');
        var suggests = window.bookmarksController.suggestSimilar(input);
        var suggestUL = document.createElement('ul');
        var suggestLI = document.createElement('li');
        var pattern = new RegExp(input, "ig");

        snippetId.innerHTML = ""; // clear previous results
        suggestUL.id = 'suggestUlId';
        suggestLI.id = "searchLiId";

        // always provide an option to search for user entry
        recenttitle = recenttitle.replace(/%1/, "<b>" + input + "</b>");

        suggestLI.innerHTML = '<a href="#" onclick="searchSuggests.searchUrlValue(\''+input+'\');'+
            'return false;">'+
            '<div class="SuggestElement">'+'<span class="aTitle">'+recenttitle+'</span>'+'</div></a>';
        suggestUL.appendChild(suggestLI);

        // add each search suggestion to unordered list
        for (i=0; i < suggests.length; i++) {
            recenturl = suggests[i].url1;
            recenttitle = suggests[i].title1;
            suggestLI = document.createElement('li');
            suggestLI.id = "suggestsLiId";

            // bold search text
            recenttitle = recenttitle.replace(pattern, "<b>$&</b>");
            recenturl = recenturl.replace(pattern, "<b>$&</b>");

            suggestLI.innerHTML = '<a href="#" onclick="searchSuggests.gotoUrl(\''+suggests[i].url1+'\');' +
                ' return false;">'+
                '<div class="SuggestElement">'+
                '<span class="aTitle">'+recenttitle+'</span><br/>'+
                '<span class="aUrl">'+recenturl+'</span></div></a>';
            suggestUL.appendChild(suggestLI);
        }

        snippetId.appendChild(suggestUL);
    }

    //! Hides suggests list and support items.
    function _hideSuggests()
    {
        clearTimeout(inputTimeoutId);
        window.snippets.SuggestsChromeId.hide(false);
        views.WebView.enabled = true;
        views.WebView.unfreeze();
    }

    // Public Functions

    //! Handler for onload javascript event.
    this.loadComplete = function()
    {
        var urlSearchSz = snippets[urlSnippetId].getGeometry();

        urlSearchHeight = urlSearchSz.height;
        snippets.SuggestsChromeId.anchorTo(urlSnippetId, suggestsXOffset, urlSearchHeight);
        snippets.SuggestsChromeId.zValue = 10;

        _setMaxHeight(); // calculate max suggest list height

        // hide suggests on external mouse events
        snippets.SuggestsChromeId.externalMouseEvent.connect(this.handleExternalMouseEvent);
        // need to resize suggest page snippet on geometry change
        chrome.prepareForGeometryChange.connect(createDelegate(this, this.handleGeometryChange));
    }

    //! Updates the size of the suggests window depending on the size of the
    //! main DIV -- which changes as the number of items changes.
    this.updateSuggestsSize = function()
    {
        if (snippets.SuggestsChromeId != undefined) {
            var totalW = chrome.displaySize.width;
            var divHeight = document.getElementById("SuggestsId").clientHeight + 2;
            var displayHeight = Math.min(maxHeight, divHeight);
            var displayWidth = totalW - (2 * suggestsXOffset);

            // reset snippet height
            snippets.SuggestsChromeId.setSize(displayWidth, displayHeight);
        }
    }

    //! Handles the geometry change signal. Need to re-calculate max height
    //! and then update suggest page height.
    this.handleGeometryChange = function()
    {
        _setMaxHeight(); // max height is based on display height
        this.updateSuggestsSize();
    }

    //! Loads google search page for search string.
    /*!
      \param input value to search for
    */
    this.searchUrlValue = function(input)
    {
        var searchurl = window.pageController.searchUrl(input);

        _hideSuggests();

        window.pageController.currentLoad(searchurl);
        window.pageController.urlTextChanged(searchurl);
    }

    //! Hides suggests list and loads specified page.
    /*!
      \param url address of page to load
    */
    this.gotoUrl = function(url)
    {
        _hideSuggests();
        url = window.pageController.guessUrlFromString(url);
        window.pageController.currentLoad(url);
        window.pageController.urlTextChanged(url);
    }

    //! Handles external mouse events - dismisses suggests list.
    /*!
      \param type the type of event
      \param name the name of event
      \param description event description
    */
    this.handleExternalMouseEvent = function(type, name, description)
    {
        // external mouse event received on VKB mouse clicks and 
        // suggest list mouse clicks both of which should be ignored
        if ((name == "MouseClick") && !urlHasFoucus 
            && !snippets.SuggestsChromeId.hasFocus) {
            _hideSuggests();
        }
    }

    //! Updates the user input for suggestion list.
    /*!
      \param input the current user input
    */
    this.updateUserInput = function()
    {
        // user still editing URL - don't show/update suggests yet
        clearTimeout(inputTimeoutId);
        inputTimeoutId = setTimeout(_showSuggests.bind(this), inputTimeoutDelay);
    }

    //! Called when load state changes. Suggests should only be called when
    //! the load state is editing.
    this.updateLoadState = function()
    {
        if (!pageController.editMode) {
            // not in editing mode - suggests not allowed
            _hideSuggests(); // ensure suggests are hidden
        }
    }

    //! Called when URL search bar focus changes. The external mouse event
    //! handler deals with most cases where the suggestion list should be
    //! dismissed but we don't get those events when the list isn't visible
    //! so this handler is needed to cancel the timer in those cases.
    this.urlSearchFocusChanged = function(focusIn)
    {
        urlHasFoucus = focusIn;
        // if visible user may be scrolling suggestion page so ignore focus change
        if (!focusIn && !snippets.SuggestsChromeId.visible) {
            // prevent suggestion list from being displayed until URL edited again
            clearTimeout(inputTimeoutId);
        }
    }

    //! Sets the user input URL suggest delay.
    /*!
      \param to the new delay time.
    */
    this.setSuggestTimeout = function(to)
    {
        inputTimeoutDelay = to;
    }
}

