
// This file contains functions to be used for testing and logging.

var __loadStartTime = new Date().getTime();
var __loadCount = 0;
var __average = 0;

// Setup callback for loadStarted signal.
// Called when a new page has started loading.
//
window.pageController.loadStarted.connect(
    function() {
        __loadStartTime = new Date().getTime();
        window.chrome.log("loadStarted:");
    }
);

// Setup callback for URL changed signal.
// Called when a new page has started loading.
//
//window.pageController.currentPageUrlChanged.connect(
//    function(url) {
//        window.chrome.log("currentPageUrlChanged: " + url);
//    }
//);

// Setup callback for loadFinished signal.
// Called when the current page has finished loading.
//   'ok' parameter is false if there was an error (not very reliable).
//
window.pageController.loadFinished.connect(
    function(ok) {
        __loadCount++;
        var loadTime = new Date().getTime() - __loadStartTime;
        app.debug("+++ loadtime=" + loadTime);
        if (__average == 0) {
            __average = loadTime;
        }
        else {
            __average += (loadTime - __average) / __loadCount;
        }
        app.debug("== count=" + __loadCount + " average=" + __average);
        window.chrome.log("loadFinished: " +
            "load count: " + __loadCount + " " +
            "load time: " + loadTime + "ms " +
            "average: " + __average + "ms " +
            "ok=" + ok + " " + window.pageController.currentDocUrl);

        if (__loadCount > 1) {
            // Load another document.  This will effectively keep loading the same document forever.
            //setTimeout("window.pageController.currentLoad('http://doc.trolltech.com/4.6/functions.html')",2000);
            setTimeout("window.pageController.currentLoad('http://www.google.com/search?q=help')",1000);
            //setTimeout("window.pageController.currentLoad('http://apple.com')",2000);
        }
    }
);

// Setup callback for loadProgress signal.
// Called periodically during the load process.
//    'percent' parameter indicates how much of the document has been loaded (0-100).
//
//window.pageController.loadProgress.connect(
//    function(percent) {
//        window.chrome.log("loadProgress: percent=" + percent);
//        window.chrome.alert("loadProgress: percent=" + percent);
//    }
//);

// Setup callback for the chrome's loadComplete signal.
// Called when the chrome finishes loading.
//
window.chrome.chromeComplete.connect(
    function() {
        window.chrome.log("chrome.loadComplete");
    }
);


