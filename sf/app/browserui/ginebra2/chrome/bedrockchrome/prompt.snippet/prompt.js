/*!
  \file prompt.js This module contains the promptDialog class which
  displays messages.
*/

function promptDialog() {
    var onChromeComplete = function()
    {
        // Get external mouse events.
        snippets.PromptChromeId.externalMouseEvent.connect(this.handleExternalMouseEvent.bind(this));

        // Watch for page load errors.
                
        window.pageController.databaseQuotaExceeded.connect(
        	function() {
                update();
                showPrompt();
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
        document.getElementById("promptTextUrl2Id").innerHTML = pageController.promptReserved;

        // Set error message.
        if(pageController.promptMsg != undefined && pageController.promptMsg != "") {
            document.getElementById("promptTextMsgId").innerHTML = pageController.promptMsg;
        }
        else {
            document.getElementById("promptTextMsgId").innerHTML = window.localeDelegate.translateText("txt_browser_error_generic_error_msg");
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
            this.hidePrompt();
        }
    }
    
    //! Show the dialog and shaded overlay.
    var showPrompt = function(){

        var snippet = snippets.PromptChromeId;
        snippet.updateOwnerArea();
        snippet.show(true);
        snippet.zValue = 1;
        snippet.repaint();
    
    }

    //! Hide the dialog and shaded overlay.
    this.hidePrompt = function(){
        snippets.PromptChromeId.hide();
    }

    //! Create the DOM elements for the dialog.
    this.setupPage = function(){    
        var html =
            '<div class="promptBox">' +
                '<ul>' +
                    '<li>' +
                        '<img src="prompt.snippet/icons/icon_dialog_error.png"/>&nbsp;&nbsp;' +
                        '<span class="promptText">' +
                            window.localeDelegate.translateText("txt_browser_error_database_quota") +
                        '</span>' +
                    '</li>' +
                    '<li id="promptTextUrlParent">' +
                        '<span class="promptText" id="promptTextUrl1Id"></span><br/>' +
                        '<div class="promptText2" id="promptTextUrl2Id"></div>' +
                    '</li>' +
                    '<li>' +
                        '<span class="promptText" id="promptTextMsgId"/>' +
                    '</li>' +
                    '<li>' +
                        '<center><img id="prompt_okId" class="promptOkButton"/></center>' +
                    '</li>' +
                '</ul>' +
            '</div>';
        document.write(html);
        new SimpleButton("prompt_okId",
                         "prompt.snippet/icons/button_dialog_ok_wait.png",
                         "prompt.snippet/icons/button_dialog_ok_press.png",
                         "",
                         this.onOkPressed.bind(this));
    }

    this.onOkPressed = function() {
        this.hidePrompt();
    }

    this.setupPage();

    chrome.chromeComplete.connect(onChromeComplete.bind(this));
}
