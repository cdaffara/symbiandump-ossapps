// Call SimpleButton to wire an HTML button (typically an <img> tag) to a JS callback
//
// (As opposed to ActionButton which is served all the way through QT Actions)
//
// Params:
//   id - the id of the button.  Ex:  <img id="buttonId" .../>
//   upImg - the path to the "up" image for the button.
//   downImg - the path to the "down" image for the button.  Note: not yet working...
//   disabledImg - the path to the "disabled" image for the button.
//   handler - the javascript callback to be called when triggered
//   triggerOnMouseUp- if true the handler will be called on mouse-up, defaults to true.
//   triggerOnMouseDown - if true the handler will be called on mouse-down, defaults to false.
//   triggerOnMouseOut - if true the handler will be called on mouse-out, defaults to false.
//
function SimpleButton(id, upImg, downImg, disabledImg, handler, triggerOnMouseUp, triggerOnMouseDown, triggerOnMouseOut) {
    this.id = id;
    this.upImg = upImg;
    this.downImg = downImg;
    this.disabledImg = disabledImg;
    this.handler = handler;
    this.isDown = false;
    this.enabled = true;
    this.triggerOnMouseUp= triggerOnMouseUp == undefined ? true : triggerOnMouseUp;
    this.triggerOnMouseDown = triggerOnMouseDown == undefined ? false : triggerOnMouseDown;
    this.triggerOnMouseOut = triggerOnMouseOut == undefined ? false : triggerOnMouseOut;

    // attach this class to the item as "button" for later access
    // delete/cleanup existing button (if any) and attach this as button
    dom = document.getElementById(this.id);
    if (dom.button) {
        delete dom.button;
    }
    dom.button = this;

    this.element = function() {
        return document.getElementById(this.id);
    }

    this.updateImages = function(upImg, downImg, disabledImg) {
        this.upImg = upImg;
        this.downImg = downImg;
        this.disableImg = disabledImg;
        this.updateButton();
    }

    this.updateButton = function() {
        if (this.enabled) {
            if (this.isDown) {
                this.element().src = this.downImg;
            }
            else {
                this.element().src = this.upImg;
            }
        }
        else {
            this.element().src = this.disabledImg;
        }
    }

    this.setEnabled = function(state) {
        this.enabled = state;
        this.updateButton();
    }

    // Make sure we can find the element.
    if (!this.element()) {
        alert("SimpleButton: element not found, " + id);
        return;
    }

// Commented out because onclick is not reliable.  Slight movements between mouse down
// and mouse up seem to cancel the onclick.
//    this.onClick = function() {
//        //window.chrome.alert("SimpleButton::onClick: " + this);
//        this.handler();
//    }

    this.onMouseDown = function() {
        this.isDown = true;
        this.updateButton.call(this);
        if (this.triggerOnMouseDown && this.enabled) {
            this.handler();
        }
    }

    this.onMouseUp = function() {
        this.isDown = false;
        this.updateButton.call(this);
        if (this.triggerOnMouseUp && this.enabled) {
            this.handler();
        }
    }

    this.onMouseOut = function() {
        if (this.isDown) {
            this.isDown = false;
            this.updateButton.call(this);

            if (this.triggerOnMouseOut && this.enabled) {
                this.handler();
            }
        }
    }

    // Set up element event handlers.
    this.element().onmousedown = this.onMouseDown.bind(this);
    this.element().onmouseup = this.onMouseUp.bind(this);
    this.element().onmouseout = this.onMouseOut.bind(this);
//    this.element().onclick = this.onClick.bind(this);

    // Set the initial state of the button.
    this.updateButton();
}

