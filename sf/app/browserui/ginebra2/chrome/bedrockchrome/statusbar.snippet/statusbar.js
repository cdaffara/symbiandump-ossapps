/*!
  \file statusbar.js This module contains the StatusBar class.
*/

/*!
  Class to handle displaying and updating the status bar. Only 1 status bar
  should be created for the browser. This class is not designed to be code
  space efficient for creating multiple status bar objects.
*/
function StatusBar()
{
    // Private Classes

    /*!
      Class to handle updating the status bar title. Only 1 title should be
      created for the browser status bar. This class is not designed to be code
      space efficient for creating multiple title objects.
    */
    function Title()
    {
        // Private Methods

        //! Changes the current title to the specified title.
        /*!
          \param title new title to display
          \param doRepaint force immediate repaint if true
        */
        function setTitle(title, doRepaint)
        {
            document.getElementById('title').innerHTML = title;
            if (doRepaint)
                window.snippets.StatusBarChromeId.repaint();
        }

        // Public Methods

        //! Handles title change signal.
        /*!
          \param title new title
        */
        this.handleTitleChange = function(title)
        {
             if ((!window.views.WebView.currentPageIsSuperPage()) &&
                 window.views.current().type == "webView") {
                if (title != "")
                    setTitle(title, true);
            }
        }

        //! Handles title partial URL change signal.
        /*!
          \param partialUrl new title
        */
        this.handlePartialUrlChange = function(partialUrl)
        {
            if ((!window.views.WebView.currentPageIsSuperPage()) &&
                 window.views.current().type == "webView") {
                setTitle(partialUrl, true);
            }
        }

        //! Handles title update in response to current view change signal.
        this.handleCurrentViewChange = function()
        {
            if (window.views.WebView.currentPageIsSuperPage()) {
                   //alert(window.views.WebView.currentSuperPageName());
                 if (window.views.WebView.currentSuperPageName() == "BookmarkTreeView") {
                    setTitle(window.localeDelegate.translateText("txt_browser_bookmarks_bookmarks"), false);
                  }
                else if (window.views.WebView.currentSuperPageName() == "BookmarkHistoryView") {
                         setTitle(window.localeDelegate.translateText("txt_browser_history_history"), false);
                       }
                else if (window.views.WebView.currentSuperPageName() == "SettingsView") {
                         setTitle(window.localeDelegate.translateText("txt_browser_settings_settings"), false);
                }
           }
           else {
                 if (window.views.current().type == "webView")  {
                  
                 //enable the double click for the content view page
                 if (!window.views.WebView.bedrockTiledBackingStoreEnabled())
                     window.views.WebView.touchNav.doubleClickEnabled = true;                 
                   
                      /* For new windows, show title as 'New Window' */
                    if ((window.pageController.currentDocTitle == "") && (window.pageController.currentRequestedUrl == "")) {
                        setTitle(window.localeDelegate.translateText("txt_browser_content_view_new_window"), false);
                    }
                    else if (window.pageController.currentDocTitle == "") {
                        if (window.pageController.currentDocUrl == "")  {
                            setTitle(window.pageController.currentPartialReqUrl, false);
                        }
                        else {
                            setTitle(window.pageController.currentPartialUrl, false);
                        }
                    }
                    else {
                        setTitle(window.pageController.currentDocTitle, false);
                    }
                 }
                else {
                    if (window.views.current().type == "WindowView")
                        setTitle(window.localeDelegate.translateText("txt_browser_windows_windows"), false);
                }
            }
         }

    }

    /*!
      Class to handle updating the lock status (show/remove lock icon). Only 1
      LockStatus object should be created for the browser status bar. This class
      is not designed to be code space efficient for creating multiple objects.
    */
    function LockStatus()
    {
        // Private Member Variables
        var secureIconSrc = "<img src=\"statusbar.snippet/icons/lock.png\">";
        var noIconSrc =  "&nbsp;";

        // Public Methods

        //! Shows lock icon in status bar if in webView.
        this.showLockIcon = function()
        {
            if (window.views.current().type == "webView") {
                document.getElementById('lock').innerHTML = "<img src=\"statusbar.snippet/icons/lock.png\">";
                //window.snippets.StatusBarChromeId.repaint();
            }
        }

        //! Removes lock icon from status bar.
        this.removeLockIcon = function()
        {
             document.getElementById('lock').innerHTML = "&nbsp;";
          }

        //! Handles lock status update in response to current view change signal.
        this.handleCurrentViewChange = function()
        {
              if (window.views.WebView.currentPageIsSuperPage()) {
                this.removeLockIcon();
              }
              else{
                    if (window.views.current().type == "webView")  {
                        /* Secure icon */
                        if (window.pageController.secureState)
                            this.showLockIcon();
                        else {
                            this.removeLockIcon();
                        }
                    }
                    else {
                        this.removeLockIcon();
                    }
            }
        }
    }

    /*!
      Class to handle updating the clock time. Only 1 Clock object should be
      created for the browser status bar. This class is not designed to be code
      space efficient for creating multiple objects.
    */
    function Clock()
    {
        // Public Methods
        //! Updates the time displayed on the status bar.
        this.showtime = function()
        {
            var now = new Date();
            var hours = now.getHours();
            var minutes = now.getMinutes();
            var timeValue = "" + ((hours > 12) ? hours - 12 : hours);
            timeValue += ((minutes < 10) ? ":0" : ":") + minutes;
            timeValue += (hours >=12) ? " pm": " am";
            document.getElementById('clock').innerHTML = timeValue;
        }
    }

    /*!
      Class to handle updating the network status. Only 1 NetworkStatus object
      should be created for the browser status bar. This class is not designed
      to be code space efficient for creating multiple objects.
    */
    function NetworkStatus()
    {
        // Private Member Variables
        var networkIconSrc = new Array(
            "<img src=\"statusbar.snippet/icons/signal/signal0.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/signal/signal0.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/signal/signal25.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/signal/signal50.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/signal/signal75.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/signal/signal100.png\" alt=\"\">");
        var enumNetworkStrengths = new Object();
        var currentState; // last known signal state - see enumNetworkStrengths

        enumNetworkStrengths.state = {Offline:0, NoSignal:1, Signal1:2,
            Signal2:3, Signal3:4, Signal4:5}

        currentState = enumNetworkStrengths.state.Offline;

        //! Encodes the specified string for display in HTML format.
        /*!
          \param str string to encode
        */
        // Note: this function doesn't handle partial or double encoding.
        function htmlEncode(str)
        {
            var s; // function return

            // Encode special HTML characters (&, ", <, >, and ').
            s = str.replace(/&/g, '&amp;');
            s = s.replace(/\"/g, '&quot;');
            s = s.replace(/</g, '&lt;');
            s = s.replace(/>/g, '&gt;');
            s = s.replace(/'/g, '&apos;');

            return (s);
        }

        //! Updates the field width for the network provider name.
        /*!
          \param s network provider name
        */
        function updateFieldWidth(s)
        {
            if (document.getElementById) {
                var rulerSpan = document.getElementById('sbruler');
                var fieldWidth;

                rulerSpan.innerHTML = s;
                fieldWidth = rulerSpan.offsetWidth + 10; // add 10 pixel padding
                document.getElementById('provider').width = fieldWidth + "px";
            }
        }

        //! Converts the specified strength using a scale of -1 to 100 to the
        //! appropriate signal level state.
        /*!
          \param strength signal strength to convert
        */
        function convertStrengthToState(strength)
        {
            var state;

            if (strength < 0) // unknown network mode or error
                state = enumNetworkStrengths.state.Offline;
            else if (strength == 0) // no signal
                state = enumNetworkStrengths.state.NoSignal;
            else if (strength < 40) // less than 40/100
                state = enumNetworkStrengths.state.Signal1;
            else if (strength < 65) // less than 65/100
                state = enumNetworkStrengths.state.Signal2;
            else if (strength < 90) // less than 90/100
                state = enumNetworkStrengths.state.Signal3;
            else // 90/100 or higher - full signal
                state = enumNetworkStrengths.state.Signal4;

            return (state);
        }

        //! Changes the displayed network provider name.
        /*!
          \param networkName New network provider name to display
        */
        function changeName(networkName)
        {
            // truncate name if it's too long
            if (networkName.length > NetworkStatus.MAX_NAME_LEN)
                networkName = networkName.substring(0, NetworkStatus.MAX_NAME_LEN);
            // set field width to the appropriate width and change the name
            updateFieldWidth(htmlEncode(networkName));
            document.getElementById('provider').innerHTML = htmlEncode(networkName);

            // repaint if status bar exists
            if (window.snippets.StatusBarChromeId)
                window.snippets.StatusBarChromeId.repaint();
        }

        //! Gets the appropriate image tag HTML string for the current network
        //! signal strength.
        this.getInitialStrengthImgTag = function()
        {
           var strength = window.networkDelegate.networkSignalStrength;

           currentState = convertStrengthToState(strength);
           return (networkIconSrc[currentState]);
        }

        //! Displays the initial network name.
        this.showInitialNetworkName = function()
        {
            // if we went offline, set the provider name to "offline"
            if (currentState == enumNetworkStrengths.state.Offline)
                changeName(window.localeDelegate.translateText("txt_browser_offline"));
            else
                changeName(window.networkDelegate.networkName);
        }

        //! Handles the signal strength change signal.
        /*!
          \param strength new signal strength
        */
        this.handleSignalStrengthChange = function(strength)
        {
            var state = convertStrengthToState(strength);

            // only interested in state changes
            if (currentState != state) {
                lastState = currentState; // save former state
                // update current state and network icon
                currentState = state;
                document.getElementById('strength').innerHTML =
                    networkIconSrc[currentState];

                // if we went offline, change the provider name to "offline"
                if (currentState == enumNetworkStrengths.state.Offline)
                    changeName(window.localeDelegate.translateText("txt_browser_offline"));
                // if we just came online, get and update provider name
                else if (lastState == enumNetworkStrengths.state.Offline)
                    changeName(window.networkDelegate.networkName);
            }
        }

        //! Handles the network name change signal.
        /*!
          \param networkName new network name
        */
        this.handleNameChange = function(networkName)
        {
            // Offline network name is hard coded.
            if (currentState != enumNetworkStrengths.state.Offline)
                changeName(networkName);
        }
    }

    // class property (i.e. property of the class constructor function)
    NetworkStatus.MAX_NAME_LEN = 20; // max length of provider name

    /*!
      Class to handle updating the battery level. Only 1 BatteryStatus object
      should be created for the browser status bar. This class is not designed
      to be code space efficient for creating multiple objects.
    */
    function BatteryStatus()
    {
        // Private Member Variables
        var batteryIconSrc = new Array(
            "<img src=\"statusbar.snippet/icons/battery/batt10.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt20.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt30.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt40.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt50.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt60.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt70.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt80.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt90.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt100.png\" alt=\"\">",
            "<img src=\"statusbar.snippet/icons/battery/batt100_charging.png\" alt=\"\">");
        var enumBatteryLevels = new Object();

        enumBatteryLevels.state = {Level10:0, Level20:1, Level30:2, Level40:3,
            Level50:4, Level60:5, Level70:6, Level80:7, Level90:8, Level100:9,
            LevelCharging:10}

        //! Converts the specified battery level (1 to 100) to a battery state.
        /*!
          \param level battery level (1 to 100)
        */
        function convertLevelToState(level)
        {
            var state;

            // Don't report battery level as being any higher than it actually is.
            // Unless it is under 10% in which case user story specifies one bar be displayed.
            if (window.deviceDelegate.batteryCharging)
                state = enumBatteryLevels.state.LevelCharging;
            else if (level < 20) // less than 20% full
                state = enumBatteryLevels.state.Level10;
            else if (level < 30) // less than 30% full
                state = enumBatteryLevels.state.Level20;
            else if (level < 40) // less than 40% full
                state = enumBatteryLevels.state.Level30;
            else if (level < 50) // less than 50% full
                state = enumBatteryLevels.state.Level40;
            else if (level < 60) // less than 60% full
                state = enumBatteryLevels.state.Level50;
            else if (level < 70) // less than 70% full
                state = enumBatteryLevels.state.Level60;
            else if (level < 80) // less than 80% full
                state = enumBatteryLevels.state.Level70;
            else if (level < 90) // less than 90% full
                state = enumBatteryLevels.state.Level80;
            else if (level < 100) // less than 100% full
                state = enumBatteryLevels.state.Level90;
            else // 100% full
                state = enumBatteryLevels.state.Level100;

            return (state);
        }

        //! Gets the initial battery level image tag HTML string.
        this.getInitialLevelImgTag = function()
        {
            return (batteryIconSrc[convertLevelToState(
                window.deviceDelegate.batteryLevel)]);
        }

        //! Handles battery level change signal.
        /*!
          \param level new battery level
        */
        this.handleLevelChange = function(level)
        {
            document.getElementById('battery').innerHTML =
                batteryIconSrc[convertLevelToState(level)];
        }
    }

    /*!
      Class to handle updating the download indicator.
    */
    function DownloadStatus()
    {
        function _enableDownloadIndicator()
        {
            var src = 'statusbar.snippet/icons/download_statusbar_icon.png';
            var tag = '<img id="downloadImage" src="' + src + '" alt="">';
            document.getElementById('download').innerHTML = tag;
            window.snippets.StatusBarChromeId.repaint();
        }

        function _disableDownloadIndicator()
        {
            document.getElementById('download').innerHTML = "";
            window.snippets.StatusBarChromeId.repaint();
        }

        function _connectDownloadSignals()
        {
            window.downloads.downloadCreated.connect(_enableDownloadIndicator);
            window.downloads.downloadsCleared.connect(_disableDownloadIndicator);
        }

        if (window.downloads != null) {
            window.chrome.chromeComplete.connect(_connectDownloadSignals);
        }
    }

    // Private Member Variables
    var sbTitle = new Title(); //!< status bar title
    var sbLockStatus = new LockStatus(); //!< status bar lock status
    var sbClock = new Clock(); //!< status bar clock
    var sbNetworkStatus = new NetworkStatus(); //!< status bar network status
    var sbBatteryStatus = new BatteryStatus(); //!< status bar battery status
    var sbDownloadStatus = new DownloadStatus(); //!< status bar download status

    // Private Methods
    //! Write status bar HTML code to document.
    function _statusbar_write()
    {
        var downloadHTML = '';
        if (window.downloads != null) {
            downloadHTML = '<td class="rightalign" id="download"></td>';
        }

        var html = ''+
            '<table>'+
              '<tr>'+
              '<td class="leftalign" id="strength">' + sbNetworkStatus.getInitialStrengthImgTag() + '</td>'+
              '<td class="centeralign" id="provider">&nbsp;</td>'+
              '<td class="leftalign"><div id="title">Bedrock Browser</div></td>'+
              '<td class="rightalign" id ="lock">&nbsp;</td>'+
              downloadHTML +
              '<td class="centeralign" id="clock">time: &nbsp;</td>' +
              '<td class="rightalign" id="battery">' + sbBatteryStatus.getInitialLevelImgTag() + '</td>'+
            '</tr>'+
            '</table>'+
            // ruler span used for getting the width of network name
            '<span id="sbruler"></span>';
        document.write(html);
    }

    // StatusBar Constructor
    _statusbar_write(); // write status bar HTML code to document
    sbClock.showtime(); // display current time on status bar
    // Update displayed time every 30 seconds.
    setInterval(function() {sbClock.showtime();}, 30000);

    // Note that in the slots below the "this" object is never used directly.
    // This is because they don't have access to "this" as they are invoked
    // as functions rather than as methods.

    // On chromeComplete signal, show initial network name.
    window.chrome.chromeComplete.connect(
        function() {sbNetworkStatus.showInitialNetworkName();});
    
    // Connect page controller signals to slots.
    window.pageController.titleChanged.connect(
        function(title) {sbTitle.handleTitleChange(title);});
    window.pageController.partialUrlChanged.connect(
        function(partialUrl) {sbTitle.handlePartialUrlChange(partialUrl);});
    window.pageController.hideSecureIcon.connect(
        function() {sbLockStatus.removeLockIcon();});
    window.pageController.showSecureIcon.connect(
        function() {sbLockStatus.showLockIcon();});

    // Connect view manager signals to slots.
    window.views.currentViewChanged.connect(
        function() {
            sbTitle.handleCurrentViewChange();
            sbLockStatus.handleCurrentViewChange();
        }
    );

    // Connect device delegate signals to slots.
    window.deviceDelegate.batteryLevelChanged.connect(
       function(level) {sbBatteryStatus.handleLevelChange(level);});
    window.networkDelegate.networkSignalStrengthChanged.connect(
        function(strength) {sbNetworkStatus.handleSignalStrengthChange(strength);});
    window.networkDelegate.networkNameChanged.connect(
        function(networkName) {sbNetworkStatus.handleNameChange(networkName);});

    var mydiv = document.getElementById("StatusBarChromeId");
    mydiv.addEventListener("mouseup", handleMouseUp, true);
    mydiv.addEventListener("keyup", handleMouseUp, true);

    function handleMouseUp() {
        if (window.views.current().type == "webView" &&
            !window.views.WebView.currentPageIsSuperPage()) {
            window.chrome.slideView(100);
            window.views.WebView.scrollTo(0,0);
        }
    }

}
