var cm_TheContextMenu;

// Return true if the given element's className includes the given class.
function hasClass(ele,cls) {
    return ele.className.match(new RegExp('(\\s|^)'+cls+'(\\s|$)'));
}

// Remove a class from an element's className.
function removeClass(ele,cls) {
    if (hasClass(ele,cls)) {
        var reg = new RegExp('(\\s|^)'+cls+'(\\s|$)');
        ele.className=ele.className.replace(reg,' ');
    }
}

function ContextMenu(snippetId, contentView) {
    this.snippetId = snippetId;
    this.mainDiv = undefined;
    this.tailEl = undefined;
    this.contentView = contentView;
    this.showTimeoutId = 0;
    // Width of a tab with no text, just the icon.  Icons must all be the same width.
    // Update this if icon size or tab border width etc. changes -- or better yet, determine it dynamically.
    this.normalTabWidth = 64;
    // Height of the menu is the max possible height to be used when positioning the snippet
    this.menuHeight = 272;

    // ContextMenu is a singleton to avoid problems with scope-chaining in some of the
    // callbacks that it uses.  See handleTabActivate.
    if (cm_TheContextMenu != undefined) app.debug("ERROR: cm_TheContextMenu must be a singleton");
    cm_TheContextMenu = this;

    // Create tabs and their corresponding menus based on JSON data.
    this.createTabsElement = function(data) {
        this.mainDiv = document.createElement("div");
        this.mainDiv.setAttribute("class", "ContextMenuDiv");
        this.mainDiv.setAttribute("id", "cm_mainDivId");

        var tabsDiv = document.createElement("div");
        tabsDiv.setAttribute("class", "TabsDiv");
        tabsDiv.setAttribute("id", "cm_tabsDivId");

        var menuDiv = document.createElement("div");
        menuDiv.setAttribute("class", "MenuDiv");
        menuDiv.setAttribute("id", "cm_menuDivId");

        var tabsUl = document.createElement("ul");
        tabsUl.setAttribute("class", "TabsUl");
        tabsUl.setAttribute("id", "cm_tabsUlId");
        tabsDiv.appendChild(tabsUl);

        var currentTabFound = false;

        // Iterate through the list of tabs.
        for (var i=0; i < data.tabs.length; i++) {
            var tab = data.tabs[i];
            if (tab.visible != undefined) {
                if (!tab.visible()) {
                    continue;
                }
            }

            // Create the tab.
            var tabEl = document.createElement("li");
            tabsUl.appendChild(tabEl);
            
            var tabDiv = document.createElement("div");
            tabEl.appendChild(tabDiv);

            var iconEl = undefined;
            var iconHighlightedEl = undefined;
            
            // Create the tab's icons.
            if (tab.icon != undefined) {
                iconEl = document.createElement("img");
                iconEl.setAttribute("id", "icon");
                iconEl.setAttribute("src", tab.icon);
                tabDiv.appendChild(iconEl);
            }
            if (tab.iconHighlighted != undefined) {
                iconHighlightedEl = document.createElement("img");
                iconHighlightedEl.setAttribute("id", "iconHighlighted");
                iconHighlightedEl.setAttribute("src", tab.iconHighlighted);
                tabDiv.appendChild(iconHighlightedEl);
            }

            // Create the tab's text.
            if (tab.text != undefined) {
                var anchorEl = document.createElement("a");
                tabDiv.appendChild(anchorEl);
                var textEl = document.createTextNode(tab.text);
                anchorEl.appendChild(textEl);
            }

            // Create the menu for this tab.
            var menuEl = this.createMenuElement(tab);
            menuDiv.appendChild(menuEl);

            var tabClassName;
            if (tab.current == "true") {
                // This is the current, or selected, tab.
                tabClassName = "ViewContext_HighlightedTab";
                tabDiv.className = "ViewContext_HighlightedTabDiv";
                if(iconEl != undefined) {
                    iconEl.setAttribute("style", "display: none;");
                }
                currentTabFound = true;
            }
            else {
                // Not selected.
                tabClassName = "ViewContext_NormalTab";
                tabDiv.className = "ViewContext_NormalTabDiv";
                if(iconHighlightedEl != undefined) {
                    iconHighlightedEl.setAttribute("style", "display: none;");
                }

                // Hide its menu.
                menuEl.style.display = "none";
            }

            if (tab.disabled == "true") {
                // The tab is disabled, add the appropriate CSS class to it.
                tabClassName += " ViewContext_DisabledTab";
            }

            tabEl.className = tabClassName;

            // Set up callback to show the menu that corresponds to this tab.
            tabEl.onmouseup = this.handleTabActivate;
            tabEl.cm_menu = menuEl;

            // Remember that this tab is disabled.
            tabEl.cm_disabled = tab.disabled == "true";
        }

        // If a "current" tab was not specified, highlight the first tab and display its menu.
        if (!currentTabFound) {
            var firstTabEl = tabsUl.firstChild;
            firstTabEl.className = "ViewContext_HighlightedTab";
            firstTabEl.firstChild.className = "ViewContext_HighlightedTabDiv";
            firstTabEl.cm_menu.style.display = "";
        }

//        this.tailEl = document.createElement("img");
//        this.mainDiv.appendChild(this.tailEl);
//        this.tailEl.setAttribute("id", "cm_tailId");
//        this.tailEl.setAttribute("src", "contextmenu.snippet/icons/menu_tail.png");

        this.mainDiv.appendChild(tabsDiv);
        this.mainDiv.appendChild(menuDiv);
        return this.mainDiv;
    }

    // Create a single menu based on the given data structure.
    this.createMenuElement = function(data) {
        // Create menu list.
        var menuUl = document.createElement("ul");
        menuUl.setAttribute("class", "MenuUl");

        for (var i=0; i < data.menuItems.length; i++) {
            var menuItem = data.menuItems[i];

            // Create the item.
            var itemLi = document.createElement("li");
            itemLi.setAttribute("class", "MenuLi");
            var itemSpan = document.createElement("div");

            // Is it a row if items? enumerate that as a ul inside of this outer li
            if(menuItem.menuRow != undefined) {
                var menuRowUl = document.createElement("ul");
                menuRowUl.setAttribute("class", "MenuRowUl");
                itemSpan.appendChild(menuRowUl);

                for(var j=0; j < menuItem.menuRow.length; j++)
                {
                    var menuRowItem = menuItem.menuRow[j];
                    
                    var rowItemLi = document.createElement("li");
                    rowItemLi.setAttribute("class", "MenuRowLi");
                    menuRowUl.appendChild(rowItemLi);
                    
                    // bind to mouseup
                    rowItemLi.onmouseup =  (function(handler) {
                        return function() {
                            if (handler != undefined)
                                handler();
                            this.hide();
                        }.bind(this)
                    }.bind(this))(menuRowItem.onclick);

                    if (menuRowItem.text != undefined) {
                        var textEl = document.createTextNode(menuRowItem.text);
                        rowItemLi.appendChild(textEl);
                    }
                }
            }
            else {
                itemLi.className += " RegularMenuLi";
                if (menuItem.disabled == "true" || data.disabled == "true") {
                    // Disabled item.
                    itemLi.className += " ViewContext_DisabledMenuItem";
                }
                else {
                    // Enabled item.  Set up the onmouseup handler.
                    itemLi.onmouseup = (function(handler) {
                        return function() {
                            if (handler != undefined)
                                handler();
                            this.hide();
                        }.bind(this)
                    }.bind(this))(menuItem.onclick);
                }

                itemLi.onmouseover = function() {
                    this.className += " MouseOverItem";
                }.bind(itemLi)

                itemLi.onmouseout = function() {
                    removeClass(this, "MouseOverItem");
                }.bind(itemLi)

                // Create the item's icon.
                if (menuItem.icon != undefined) {
                    var iconEl = document.createElement("img");
                    itemSpan.appendChild(iconEl);
                    iconEl.setAttribute("src", menuItem.icon);
                }

                // Create the item's text.
                if (menuItem.text != undefined) {
                    var anchorEl = document.createElement("a");
                    itemSpan.appendChild(anchorEl);
                    var textEl = document.createTextNode(menuItem.text);
                    anchorEl.appendChild(textEl);
                }
            }

            menuUl.appendChild(itemLi);
            itemLi.appendChild(itemSpan);
        }
        return menuUl;
    }
    
    // Handle mouse clicks on tab elements.
    // Note: "this" refers to the element that was clicked on, ie. the tab item.
    this.handleTabActivate = function() {
        var tabsDivChildren = document.getElementById("cm_tabsUlId").childNodes;
        var otherTabsWidth = 0;
        // Set the class for each tab.
        for (var i = 0; i < tabsDivChildren.length; i++) {
            var tabEl = tabsDivChildren[i];
            var iconHighlighted = getChildById(tabEl, "iconHighlighted");
            var icon = getChildById(tabEl, "icon");
            if (tabEl == this) {
                // Activate the tab.
                tabEl.className = "ViewContext_HighlightedTab";
                //tabEl.firstChild.className = "ViewContext_HighlightedTabDiv";
                
                // Show the highlighted icon, if one exists.
                if(iconHighlighted != undefined) {
                    iconHighlighted.style.display = "";
                    
                    // Hide the normal icon.
                    if (icon != undefined) {
                        icon.style.display = "none";
                    }
                }
            }
            else {
                // Deactivate the tab.
                tabEl.className = "ViewContext_NormalTab";
                //tabEl.firstChildclassName = "ViewContext_NormalTabDiv";
                
                // If a highlighted icon exists, switch to the normal one, otherwise leave
                // the normal one alone.
                if(iconHighlighted != undefined) {
                    iconHighlighted.style.display = "none";
                    if (icon != undefined) {
                        icon.style.display = "";
                    }
                }
            }
            if (tabEl.cm_disabled) {
                tabEl.className += " ViewContext_DisabledTab";
                //tabEl.firstClassName += " ViewContext_DisabledTabDiv";
            }
        }

        // Show the menu of the tab that was just clicked.
        var menuDivChildren = document.getElementById("cm_menuDivId").childNodes;
        for (var i = 0; i < menuDivChildren.length; i++) {
            var menuEl = menuDivChildren[i];
            if (menuEl == this.cm_menu) {
                menuDivChildren[i].style.display =  "";
            }
            else {
                menuDivChildren[i].style.display =  "none";
            }
        }
        //cm_TheContextMenu.positionTail();
        //document.getElementById(this.snippetId).clientHeight = this.mainDiv.clientHeight;
        //document.getElementById("ContextMenuId").setAttribute("style", "height: " + document.getElementById("cm_mainDivId").clientHeight + "px;")

        cm_TheContextMenu.updateTabSizes();
        snippets[cm_TheContextMenu.snippetId].updateOwnerArea();
    }

    // Return the width of the "non-content" part of the element box, ie. thickness of
    // the padding, the border and the margin.
    this.getNonContentWidth = function(element) {
        var tabStyle = document.defaultView.getComputedStyle(element, null);
        return parseInt(tabStyle["margin-left"]) + parseInt(tabStyle["margin-right"]) +
               parseInt(tabStyle["padding-left"]) + parseInt(tabStyle["padding-right"]) +
               parseInt(tabStyle["border-left-width"]) + parseInt(tabStyle["border-right-width"]);
    }

    // Update the tab widths.  Expand the highlighted tab to its maximum width and shrink the
    // normal tabs to their minimum widths.  Note: it would be preferable to have this done
    // by CSS.
    this.updateTabSizes = function() {
        var tabsUl = document.getElementById("cm_tabsUlId");
        var tabsDivChildren = tabsUl.childNodes;
        var otherTabsWidth = 0;
        var highlightedTab;
        for (var i = 0; i < tabsDivChildren.length; i++) {
            var tabEl = tabsDivChildren[i];
            if (tabEl.className.indexOf("ViewContext_HighlightedTab") != -1) {
                highlightedTab = tabEl;
            }
            else {
                var newTabWidth = cm_TheContextMenu.normalTabWidth - cm_TheContextMenu.getNonContentWidth(tabEl);
                tabEl.style.width = newTabWidth;
                otherTabsWidth += tabEl.offsetWidth;
            }
        }
        if (highlightedTab != undefined) {
            var newWidth = tabsUl.offsetWidth - otherTabsWidth;
            newWidth -= cm_TheContextMenu.getNonContentWidth(highlightedTab) + 2;
            highlightedTab.style.width = newWidth;
        }
    }

    this.positionTail = function() {
        // Move the "tail" into position.
        var tailEl = document.getElementById("cm_tailId");
        if (tailEl != undefined) {
            //var mainDiv = document.getElementById("cm_mainDivId");
            var tailX = (this.mainDiv.clientWidth - tailEl.clientWidth) / 2;
            var tailY = this.mainDiv.clientHeight;
            tailEl.setAttribute("style", "position:absolute; top: " + tailY + "px; left: " + tailX);
        }
    }

    // Create all the DOM elements of the window.
    this.create = function(menuData) {
        var snippetEl = document.getElementById(this.snippetId);
        var el = this.createTabsElement(menuData);
        snippetEl.appendChild(el);
    }

    // Show the content menu.  The menuData must contain an object tree describing the structure of the
    // tabs and sub-menus.
    //
    // Example menu data in JSON format:
    //        var MenuData = {
    //            "tabs": [ {
    //                     "text": "Tab 1",
    //                     "icon": "tab1.png",
    //                     "current": "true",
    //                     "menuItems": [ {
    //                             "text": Menu item 1,
    //                             "onclick": handleMenu1,
    //                             "icon": "menu1.png",
    //                           },
    //                           {
    //                             "text": Menu item 2,
    //                             "onclick": function() { alert("hello"); },
    //                             "icon": "menu2.png",
    //                           },
    //                         ],
    //                 },
    //                 {
    //                     "text": "Tab 2",
    //                     "icon": "tab2.png",
    //                     "menuItems": [ {
    //                             "text": Menu item 1,
    //                             "onclick": handleMenu21,
    //                           },
    //                           {
    //                             "text": Menu item 2,
    //                             "onclick": handleMenu22,
    //                           },
    //                         ],
    //                 },
    //                ]
    //        };
    //
    
    this.cancel = function() {
        //app.debug("CM: cancel " + this.showTimeoutId);
        clearTimeout(this.showTimeoutId);
        this.showTimeoutId = 0;
        this.cleanUp();
    }

    this.cleanUp = function() {
        // Remove elements from DOM to save memory.
        var oldEl = document.getElementById("cm_mainDivId");
        if (oldEl) {
            var snippetEl = document.getElementById(cm_TheContextMenu.snippetId);
            snippetEl.removeChild(oldEl);
        }
    }

    // Hide this window.
    this.hide = function() {
        snippets[cm_TheContextMenu.snippetId].hide();
    }

    this.onHide = function() {
        this.cleanUp();
    }

    this.show = function(menuData) {
        this.cleanUp();
        this.create(menuData);

        cm_TheContextMenu.updateTabSizes();
        // Use a timer to actually show the window to allow the page re-layout
        // to finish.  We don't know when this really happens but 50ms seems to
        // be enough on the N97.  Without this delay the bottom of the window
        // often gets clipped.
        setTimeout("cm_TheContextMenu.showIt2()", 50);
    }

    this.showIt2 = function() {

        var snippet = snippets[cm_TheContextMenu.snippetId];
        snippet.updateOwnerArea();
        snippet.setZValue(2);

        this.centerSnippet();

//        if (showTail) {
//            cm_TheContextMenu.positionTail();
//        }

        snippet.show();
    }


    this.centerSnippet = function() {

        
        var statusBarHeight = snippets.StatusBarChromeId.geometry.height;
        
        var snippet = snippets[cm_TheContextMenu.snippetId];
        var x = (chrome.displaySize.width - snippet.geometry.width) / 2;
        
        // Center the menu in the space between status bar and tool bar
        var y = (chrome.displaySize.height - statusBarHeight - snippets.WebViewToolbarId.geometry.height - cm_TheContextMenu.menuHeight)/2;
        snippet.setPosition(x, (y+statusBarHeight));
    
    }
    chrome.chromeComplete.connect(createDelegate(this,
        function() {
            var snippet = snippets[cm_TheContextMenu.snippetId];

            chrome.aspectChanged.connect(createDelegate(this,
                    function(a) {
                        this.centerSnippet();
                    }));

            snippet.hidden.connect(createDelegate(this, this.onHide));

        }));
}  // End ContextMenu class
