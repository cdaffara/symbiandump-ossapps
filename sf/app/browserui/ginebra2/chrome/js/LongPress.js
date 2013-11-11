//
// Use LongPress() to set a function callback for a long-press on a particular DOM element.
//   Parameters:
//      elementId: id string of the element
//      callback: the function to be called when the long-press fires
//      mouseDownCallback: the function to be called on mouse-down
//      timerValue : timeout value for long press
//   Example:
//        <javascript ...>
//          new LongPress("btnId", function(el) { alert("hello"); });
//        </javascript>
//        ...
//        <img id="btnId" ... />
//
var gInitialX = 0;
var gInitialY = 0;
    
function LongPress(elementId, callback, mouseDownCallback, timerValue) {
    this.el = document.getElementById(elementId);
    this.callback = callback;
    this.initialX = 0;
    this.initialY = 0;
    this.mouseDownCallback = mouseDownCallback;
    this.timerValue = timerValue;
		
    if (!this.el) {
        //window.app.debug("LongPress: element " + elementId + " not found");
        return;
    }

    this.onTimerFired = function() {
        //window.app.debug("onTimerFired");
        this.callback(this.el);
        this.cancelTimer();
        this.unregisterMouseMove();
    }

    this.startTimer = function() {
        this.cancelTimer();
        this.timer = window.setTimeout(createDelegate(this, this.onTimerFired), timerValue);
    }

    this.cancelTimer = function() {
        if (this.timer) {
            window.clearTimeout(this.timer);
            this.timer = null;
        }
    }

    // Stop tracking mouse movements.
    this.unregisterMouseMove= function() {
        this.el.onmousemove = null;
    }

    // Stop tracking mouse out.
    this.unregisterMouseOut= function() {
        this.el.onmouseout = null;
    }
    
    this.cancel = function() {
        //window.app.debug("cancel");
        this.cancelTimer();
        this.unregisterMouseOut();
        this.unregisterMouseMove();
    }

    // If the mouse has moved too much it's not considered a long press, so cancel.
    this.onMouseMove = function() {
        //window.app.debug("LongPress::onMouseMove " + this + " event=" + window.event +
        //                    " " + window.event.clientX + "," + window.event.clientY);
        if (Math.abs(this.initialX - window.event.clientX) > 16 ||
           Math.abs(this.initialY - window.event.clientY) > 16) {
            this.cancel();
        }
    }

    // Start tracking the mouse and save the initial mouse coords.
    this.onMouseDown = function() {
        //window.app.debug("LongPress::onMouseDown " + this);
        this.isDown = true;
        this.initialX = window.event.clientX;
        this.initialY = window.event.clientY;
				gInitialX = window.event.clientX;
				gInitialY = window.event.clientY;
    		this.el.onmouseout = this.cancel.bind(this);
        this.el.onmousemove = this.onMouseMove.bind(this);
        this.startTimer();
        if (this.mouseDownCallback != undefined)
            this.mouseDownCallback(this);
    }

    this.el.onmousedown = this.onMouseDown.bind(this);

    // Cancel tracking on mouse up and out events, ie. not a long press.
    this.el.onmouseup = this.cancel.bind(this);
}
