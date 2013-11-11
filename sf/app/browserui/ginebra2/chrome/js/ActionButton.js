// Call ActionButton to wire an HTML button (typically an <img> tag) to an action (QAction).
// Params:
//   id - the id of the button.  Ex:  <img id="buttonId" .../>
//   upImg - the path to the "up" image for the button.
//   downImg - the path to the "down" image for the button.  Note: not yet working...
//   disabledImg - the path to the "disabled" image for the button.
//   action - the targeted action
//   triggerOnMouseUp- if true the action will be triggered on mouse-up, defaults to true.
//   triggerOnMouseDown - if true the action will be triggered on mouse-down, defaults to false.
//   triggerOnMouseOut - if true the action will be triggered on mouse-out, defaults to false.
//
function ActionButton(id, upImg, downImg, disabledImg, action, triggerOnMouseUp, triggerOnMouseDown, triggerOnMouseOut) {
    this.id = id;
    this.upImg = upImg;
    this.downImg = downImg;
    this.disabledImg = disabledImg;
    this.action = action;
    this.isDown = false;
    this.triggerOnMouseUp= triggerOnMouseUp == undefined ? true : triggerOnMouseUp;
    this.triggerOnMouseDown = triggerOnMouseDown == undefined ? false : triggerOnMouseDown;
    this.triggerOnMouseOut = triggerOnMouseOut == undefined ? false : triggerOnMouseOut;

    // window.chrome.alert("ActionButton");

    // attach this object to the item as "button" for later access
    // delete/cleanup existing button (if any) and attach this as button
    dom = document.getElementById(this.id);
    if (dom.button) {
        delete dom.button;
    }
    dom.button = this;

    this.element = function() {
        return document.getElementById(this.id);
    }

    this.updateButton = function() {
        // window.chrome.alert("ActionButton::updateButton " + this);
        if (this.action.enabled) {
            if (this.isDown) {
                this.element().src = this.downImg;
            } else {
                this.element().src = this.upImg;
            }
        } else {
            this.element().src = this.disabledImg;
        }
    }

    // Make sure we can find the element.
    if (!this.element()) {
        alert("ActionButton: element not found, " + id);
        return;
    }

    // --------------------------------

    // Callback for changes in the action's state.
    onActionChanged = function() {
//          window.chrome.alert("ActionButton::on action changed " +
//                              " this=" + this +
//                              " action=" + this.action     +
//                              " id=" + this.id     +
//                              " enabled=" + this.action.enabled +
//                              " src=" + this.element().src);
        this.updateButton();
    }

// Commented out because onclick is not reliable.  Slight movements between mouse down
// and mouse up seem to cancel the onclick.
//    this.onClick = function() {
//        window.chrome.alert("ActionButton::onClick: " + this);
//        if (this.action.enabled) {
//            this.action.trigger();
//        }
//        else {
//            window.chrome.alert("ActionButton::onClick: not enabled");
//        }
//    }

    this.onMouseDown = function() {
        // window.chrome.alert("ActionButton::onMouseDown " + this);
        if (!this.isDown) {
            this.isDown = true;
            this.updateButton.call(this);
            if (this.triggerOnMouseDown) {
                this.action.trigger();
            }
        }
    }

    this.onMouseUp = function() {
        // window.chrome.alert("ActionButton::onMouseUp " + this);
        if (this.isDown) {
            this.isDown = false;
            this.updateButton.call(this);

            if (this.triggerOnMouseUp) {
                // Trigger the action.
                this.action.trigger();
            }
        }
    }

    this.onMouseOut = function() {
        // window.chrome.alert("ActionButton::onMouseOut " + this);
        if (this.isDown) {
            this.isDown = false;
            this.updateButton.call(this);

            if (this.triggerOnMouseOut) {
                // Trigger the action.
                this.action.trigger();
            }
        }
    }


    // Connect to the action's "changed" signal.  Note: don't use bind here, causes crash in
    // scope chain code (but only for first document that is loaded...).
    this.action.changed.connect(createDelegate(this, onActionChanged));

    // Set up element event handlers.
    this.element().onmousedown = this.onMouseDown.bind(this);
    this.element().onmouseup = this.onMouseUp.bind(this);
    this.element().onmouseout = this.onMouseOut.bind(this);
//    this.element().onclick = this.onClick.bind(this);

    // Set the initial state of the button.
    this.updateButton();
}

