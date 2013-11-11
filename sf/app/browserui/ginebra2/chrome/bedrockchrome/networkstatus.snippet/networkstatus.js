/*!
  \file networkstatus.js This module contains the NetworkStatusDialog class which
  displays network/http errors.
*/

function NetworkStatusDialog() {
    var onChromeComplete = function()
    {
        // Get external mouse events.
        snippets.NetworkStatusChromeId.externalMouseEvent.connect(this.handleExternalMouseEvent.bind(this));

        // Watch for page load errors.
        window.pageController.pageLoadFailed.connect(
          function() {
             if (!window.pageController.loadCanceled ) {
                if (window.pageController.errorUrlMatches) {
                   update();
                   showNetworkStatus();
                }
             }
          }
        );        
    }
    
    //! Add text to the given DOM element, truncate it at the given width (in pixels).
    //! Returns the remainder of the string.
    function truncateTextToWidth(element, text, width)
    {
       element.innerHTML = '<span style="white-space:nowrap;">' + text + '</span>';
       if(element.offsetWidth > width)
       {
          var i = 1;
          element.innerHTML = '';
          while(element.offsetWidth < (width) && i < text.length)
          {
             element.innerHTML = text.substr(0,i);
             i++;
          }
          return text.substr(i-1);
       }
       return "";
    } 
    
    //! Update text elements with error info.
    var update = function()
    {
        // Set URL.  Two lines of it are displayed, the first wraps to the second and the second
        // is truncated with an ellipsis appended by CSS.
        var parentWidth = document.getElementById("networkStatusTextUrlParent").offsetWidth;
        var restOfUrl = truncateTextToWidth(document.getElementById("networkStatusTextUrl1Id"), '"' + pageController.networkErrorUrl + '"', parentWidth);
        document.getElementById("networkStatusTextUrl2Id").innerHTML = restOfUrl;

        // Set error message.
        if(pageController.networkErrorMsg != undefined && pageController.networkErrorMsg != "") {
            document.getElementById("networkStatusTextMsgId").innerHTML = pageController.networkErrorMsg;
        }
        else {
            document.getElementById("networkStatusTextMsgId").innerHTML = window.localeDelegate.translateText("txt_browser_error_generic_error_msg");
        }
    }
    
    //! Handles external mouse events - dismisses status dialog.
    /*!
      \param type the type of event
      \param name the name of event
      \param description event description
    */
    this.handleExternalMouseEvent = function(type, name, description)
    {
        if (name == "QGraphicsSceneMousePressEvent") {
            this.hideNetworkStatus();
        }
    }
    
    //! Show the network status dialog and shaded overlay.
    var showNetworkStatus = function(){

        var snippet = snippets.NetworkStatusChromeId;
        snippet.updateOwnerArea();
        snippet.show(true);
        snippet.zValue = 1;
        snippet.repaint();
    
        //networkTimeoutId = setTimeout(hideNetworkStatus, 2000);
    }

    //! Hide the network status dialog and shaded overlay.
    this.hideNetworkStatus = function(){
        snippets.NetworkStatusChromeId.hide();
    }

    //! Create the DOM elements for the dialog.
    this.setupPage = function(){    
        var html =
            '<div class="networkStatusBox">' +
                '<ul>' +
                    '<li>' +
                        '<img src="networkstatus.snippet/icons/icon_dialog_error.png"/>&nbsp;&nbsp;' +
                        '<span class="networkStatusText">' +
                            window.localeDelegate.translateText("txt_browser_error_page_load_failed") +
                        '</span>' +
                    '</li>' +
                    '<li id="networkStatusTextUrlParent">' +
                        '<span class="networkStatusText" id="networkStatusTextUrl1Id"></span><br/>' +
                        '<div class="networkStatusText2" id="networkStatusTextUrl2Id"></div>' +
                    '</li>' +
                    '<li>' +
                        '<p class="networkErrorMessage" id="networkStatusTextMsgId"/>' +
                    '</li>' +
                    '<li>' +
                        '<center><img id="networkStatus_okId" class="networkStatusOkButton"/></center>' +
                    '</li>' +
                '</ul>' +
            '</div>';
        document.write(html);
        new SimpleButton("networkStatus_okId",
                         "networkstatus.snippet/icons/button_dialog_ok_wait.png",
                         "networkstatus.snippet/icons/button_dialog_ok_press.png",
                         "",
                         this.onOkPressed.bind(this));
    }

    this.onOkPressed = function() {
        this.hideNetworkStatus();
    }

    this.setupPage();

    chrome.chromeComplete.connect(onChromeComplete.bind(this));
}
