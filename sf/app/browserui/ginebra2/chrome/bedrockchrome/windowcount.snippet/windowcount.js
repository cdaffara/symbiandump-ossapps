/* This snippet is used to display the circles indicating the number of windows open.
 * Current window is indicated using filled in circle and others by an empty circle.
 * In portrait mode, the snippet is placed below the status bar while in landscape
 * mode, it is overlaid on the status bar
 * Note: When overlaying the snippet there were some issues repainting when the background was set to none
 * and hence a table with three cells are being used - the width of first and last cells match the
 * area we need to see from the status bar and their backgrounds are set to none. The middle cell is
 * used to display the circles and its bakground changes between black and white based
 * on the display mode.
 */

//
// INIT the WindowCount snippet
//
function WindowCountBar(id)
{

    this.id = id;

    this.begin =
        '<table id="wcrow" align=center   >'+
        '<tr>'+
        '<td id="wcfirst" style="background-color: transparent;">'+
        '</td>'+
        '<td  id="wccontent">';

    this.filledimg =
        '<img class="wcicon" src="windowcount.snippet/icons/filledcircle.png" >' ;

    this.emptyimg =
        '<img class="wcicon" src="windowcount.snippet/icons/emptycircle.png" >' ;

    this.end =
        '</td>'+
        '<td id="wclast" style="background-color: transparent;">'+
        '</td>'+
        '</tr>'+
        '</table>' ;

    this.InitWCBar = function()  {
        this.wcOneWindow();
        this.setId();
    }

    /* To identify the class. This class can also be identified through the
     * global variable wcbar which saves the instance when initialized
     * in chrome.html
     */
    this.setId = function() {
        el = document.getElementById(this.id);
        if (el.wcChrome) {
            delete el.wcChrome;
        }
        el.wcChrome = this;

    }

    this.wcUpdateWindowHtml = function() {

        //window.app.debug("WC: wcUpdateWindowHtml "+ window.pageController.pageCount());
        //If we are in webview get page count from pageController
        if (window.views.current().type == "webView" ) {
            count = window.pageController.pageCount();
        }
        else  if (window.views.current().type == "WindowView" ){
            count = window.views.WindowView.pageCount();
        }

        //app.debug("wcUpdateWindowHtml: View " + window.views.current().type  + " Count : " + count);
        /* Generate html based on the number of windows open */
        switch (count) {
            case 1:
                this.wcOneWindow();
                break;
            case 2:
                this.wcTwoWindows();
                break;
            case 3:
                this.wcThreeWindows();
                break;
            case 4:
                this.wcFourWindows();
                break;
            case 5:
                this.wcFiveWindows();
                break;
            default :
                break;
        }
        this.setProps();

   }

    this.wcOneWindow = function() {
        var htmlText = '' +
                this.begin + this.filledimg + this.end;

        var el = document.getElementById("wcrow");
        if (el) {
            el.innerHTML = htmlText;
        }
        else {
            document.write(htmlText);
        }
    }

    this.wcTwoWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.end;

        this.setHtmlText(htmlText)


    }

    this.wcThreeWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.emptyimg+ this.end;
        this.setHtmlText(htmlText)
    }

    this.wcFourWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.emptyimg + this.emptyimg +  this.end;

        this.setHtmlText(htmlText)
    }

    this.wcFiveWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.emptyimg + this.emptyimg + this.emptyimg + this.end;

        this.setHtmlText(htmlText)
    }

    this.setHtmlText = function(htmlText) {

        var el = document.getElementById("wcrow");
        if (el) {
            el.innerHTML = htmlText;
        }
        else {
            document.write(htmlText);
        }
        this.setCurrentIndex();
    }

    this.setCurrentIndex  = function(){
        var el = document.getElementById("wcrow");
        Icons = el.getElementsByTagName("img");

        if (window.views.current().type == "webView" ) {
            index = window.pageController.currentPageIndex();
        }
        else  if (window.views.current().type == "WindowView" ){
            index = window.views.WindowView.currentPageIndex();
        }

        //app.debug("setCurrentIndex: View " + window.views.current().type  + " Index : " + index);
        Icons[index].setAttribute('src', "windowcount.snippet/icons/filledcircle.png");
    }


    this.setSnippetPosition = function() {

        if (window.snippets.WindowCountBarId ) {
            mode = window.chrome.displayMode;
            if (mode == "portrait") {
                window.snippets.WindowCountBarId.setPosition(0,27);
            }
            else if (mode == "landscape") {
                window.snippets.WindowCountBarId.setPosition(0,0);
            }

        }
    }


    /* Private method that sets the properties:
     * set the widht of first and last cells,
     * set the background of middle cell based on
     * display mode
     */
    this.setProps = function() {

        var leftW = document.getElementById("strength").offsetWidth +
                    document.getElementById("provider").offsetWidth +
                    document.getElementById("title").offsetWidth;

        var rightW = document.getElementById("clock").offsetWidth +
                    document.getElementById("battery").offsetWidth;

        if (window.snippets.WindowCountBarId) {
            if (window.chrome.displayMode == "portrait" ) {
                // for some reason setting width to 0 doesn't work
                document.getElementById("wcfirst").width = "1px";
                document.getElementById("wclast").width = "1px";
            }
            else {
                document.getElementById("wcfirst").width = leftW + "px";
                document.getElementById("wclast").width = rightW + "px";
            }
        }
    }

    function updateWCSnippet(mode) {
        el = document.getElementById('WindowCountBarId').wcChrome;
        el.setSnippetPosition();
        el.setProps();
    }

    function onPageAdded() {

        //app.debug("onPageAdded: Count " + window.views.WindowView.pageCount());
        document.getElementById('WindowCountBarId').wcChrome.wcUpdateWindowHtml();
        //app.debug("Leave onPageAdded ");
    }

    /* Initialize */
    this.InitWCBar();

    /* Slots */
    window.chrome.chromeComplete.connect(
        function() {
          el = document.getElementById('WindowCountBarId').wcChrome;
          el.setSnippetPosition();
          //el.wcUpdateWindowHtml();
          window.chrome.aspectChanged.connect(updateWCSnippet);
          window.views.WindowView.pageAdded.connect(onPageAdded);
        }
    );


    window.ViewStack.pageChanged.connect(
        function() {
          document.getElementById('WindowCountBarId').wcChrome.wcUpdateWindowHtml();
        }
    );

    window.ViewStack.activateWindowView.connect(
        function() {
          updateWCSnippet();
          window.snippets.WindowCountBarId.toggleVisibility(false);
        }
    );


    window.ViewStack.deActivateWindowView.connect(
        function() {
            window.snippets.WindowCountBarId.toggleVisibility(false);
        }
    );
}
