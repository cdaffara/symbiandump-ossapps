var __prevOpenedBookmarkControl = null;
var __previousSortEle = null;
var __bookmarkCount= 0;
var __dragendFlag = false;
var __dogearSelected = false;
var __timerValueLeft = 0;
var __timerId = "";
var __longPressEvent = false;
// Set to the LongPress object that is currently waiting for activation,
// ie. it has gotten a mouse-down event and has its timer running.
var __currentLongPress;
var __cancelLinkOpening = false;

function _enableSorting(ele) {
    // This is messy, why do we even need to do this sort/no-sort stuff???
    $(ele).removeClass('no-sort');
    $(ele).addClass('sort');
		
	var evt = document.createEvent("MouseEvents");
	evt.initMouseEvent("mousedown", true, true, window,
	  0, 0, 0, gInitialX, gInitialY, false, false, false, false, 0, null);
	var canceled = !ele.dispatchEvent(evt);
    
  	views.WebView.gesturesEnabled = false;

    $(ele).find(".dogEarBox").addClass('selected');
    $(ele).find(".dogEar").addClass('selected');

  __sortingEnabled = true;
  __sortEleOffsetTop = ele.offsetTop;
  __previousSortEle = ele;
    $(ele).toggleClass('highlight');

  }

function _disableSorting(ele) {
//    app.debug('disablesorting:'); printProp(ele);

    views.WebView.gesturesEnabled = true;
    
    $(ele).removeClass('sort');
    $(ele).addClass('no-sort');
    $(ele).find('.dogEarBox').removeClass('selected').addClass('closed');
    $(ele).find('.dogEar').removeClass('selected').addClass('closed');

    __sortingEnabled = false;
    __sortEleOffsetTop = 0;
    __previousSortEle = null;
}

function _dragInit()
{
    $("#bookmarkListTree").sortable({
        placeholder: 'ui-state-highlight',
        axis:'y',
      cancel : 'li.no-sort',
      scrollSensitivity : 40,
      scrollSpeed : 40,
        start: function(event, ui) {
            __dragendFlag = true;
            },
      update: function(event, ui) {
                var sortedBookmark = ui.item[0];
                _disableSorting(sortedBookmark); // Unselect it
                // If there's only one bookmark in the list, do nothing
                if ($('#bookmarkListTree').children().length == 0)
                    return;
                var nextBookmark = $(sortedBookmark).next()[0];
                var bm = window.bookmarksController.findBookmark(sortedBookmark.id);
                // If it wasn't moved to the end, change the sortIndex to the next bookmark's sortIndex and the rest will propagate down
                if (nextBookmark) {
                    var nextbm = window.bookmarksController.findBookmark(nextBookmark.id);
                    window.bookmarksController.reorderBookmark(sortedBookmark.id, nextbm.sortIndex);
                }
                // It was moved to the end, so change the sortIndex to the prev bookmark's sortIndex + 1
                else {
                    var prevBookmark = $(sortedBookmark).prev()[0];
                    var prevbm = window.bookmarksController.findBookmark(prevBookmark.id);
                    window.bookmarksController.reorderBookmark(sortedBookmark.id, prevbm.sortIndex+1);
                }
            }
    });

    $("#bookmarkListTree").sortable('enable');
    $("#bookmarkListTree").disableSelection();
}

function _longPress(ele)
{
   __longPressEvent = true;
   if(__timerId != "")
   {
       window.clearTimeout(__timerId);
       __timerId = "";
   }
  if (__sortingEnabled)
  {
            _disableSorting(__previousSortEle)
            return false;
  }

  if(__prevOpenedBookmarkControl)
  	_bookmarkHideControl(__prevOpenedBookmarkControl);
  	
    if(!__dogearSelected)
     _enableSorting(ele);
}

function _timeLeft()
{
   __timerValueLeft = 1;
}
function _longPressStarted(lp)
{
    __timerValueLeft = 0;
    __longPressEvent = false;
    if(!window.views.WebView.bedrockTiledBackingStoreEnabled())
    {
        __timerId = window.setTimeout("_timeLeft()",250);
    }
    else
    {
        __timerId = window.setTimeout("_timeLeft()",550);
    }
    // Remember the current LongPress object so we can cancel it if scrolling
    // starts.
    __currentLongPress = lp;
    __cancelLinkOpening = false;
}

function _handlePanStarted(type)
{
		__cancelLinkOpening = true;
		
    // Scrolling started, cancel the LongPress.
      if (__currentLongPress != undefined) {
        __currentLongPress.cancel();
        __currentLongPress = undefined;
    }
}


function _bookmarkHideControl(ele){
    $(ele).find('.dogEarBox').addClass('closed');
    $(ele).find('.dogEar').addClass('closed');
    $(ele).find('.bookmarkItem').css('width', (parseInt(window.innerWidth)-70)+"px");
}

function _bookmarkToggleControls(event){
    var ele = event.target;
    try {
        if(__timerId != "")
        {
        	window.clearTimeout(__timerId);
        	__timerId = "";
        }
        if (__sortingEnabled && ele.parentNode.parentNode!=__previousSortEle)
        {
            _disableSorting(__previousSortEle)
            return false;
        }
        else if (__sortingEnabled && (ele.parentNode.parentNode==__previousSortEle))
        {
            return false;
        }
        if(__timerValueLeft == 1 || __longPressEvent == true)
        {
            __timerValueLeft = 0;
            __longPressEvent = false;
            return false;
        }
        __timerValueLeft = 0;

        var li = $(ele).parents('li');
        var dogEarBox = li.find('.dogEarBox');
        dogEarBox.toggleClass('closed');
        dogEarBox.find('.dogEar').toggleClass('closed');
        // It's too bad we can't do this via margin-right because then we could just do it in the .css file coz it would be a static value
        if (dogEarBox.hasClass('closed')) {
            li.find('.bookmarkItem').css('width', (parseInt(window.innerWidth)-70)+"px");
        }
        else {
            li.find('.bookmarkItem').css('width', (parseInt(window.innerWidth)-220)+"px");
        }
        if (__prevOpenedBookmarkControl != null && __prevOpenedBookmarkControl != ele.parentNode.parentNode)
            _bookmarkHideControl(__prevOpenedBookmarkControl);

    }catch(e) { alert(e); }

    __prevOpenedBookmarkControl = ele.parentNode.parentNode;
}

function _addNewBookmark(bmtitle,bmurl,bmid)
{
    // bookmark title/url may have been altered by the controller's add method (i.e. to include http://) so reload it
    var bm = window.bookmarksController.findBookmark(bmid);
    bmtitle = bm.title;
    bmurl = bm.url;
    if(__timerId != "")
    {
        window.clearTimeout(__timerId);
        __timerId = "";
    }
	if(__prevOpenedBookmarkControl)
  		_bookmarkHideControl(__prevOpenedBookmarkControl);

    if(__previousSortEle    != null)
        _disableSorting(__previousSortEle)

    __bookmarkCount += 1;
    //create element and add it to bookmark view
    var ul=document.getElementById('bookmarkListTree');
    var dbgTitle = bmtitle.replace(/'/g, "&#39");
    dbgTitle = dbgTitle.replace(/"/g, "&#34");
    var li = _createBookmarkElement(dbgTitle,bmurl,bmid);
	$(li).addClass('no-sort');
	ul.insertBefore(li, ul.firstChild);
    ul.childNodes[ul.childNodes.length-1].focus();
    
    if (!window.views.WebView.bedrockTiledBackingStoreEnabled())
        new LongPress(li.id, _longPress, _longPressStarted, 250);
    else
        new LongPress(li.id, _longPress, _longPressStarted, 550);

    $('html, body').animate({
                        scrollTop: 0}, 1000);
}

function _editBookmark(bmtitle,bmurl,bmid)
{
    if(__prevOpenedBookmarkControl)
        _bookmarkHideControl(__prevOpenedBookmarkControl);

    // bookmark title/url may have been altered by the controller's edit method (i.e. to include http://) so reload it
    var bm = window.bookmarksController.findBookmark(bmid);

    $('#'+bmid).find('.aTitle').text(bm.title);
    $('#'+bmid).find('.aUrl').text(bm.url);
}

function _launchEditBookmark(r,bmtitle,bmurl,id)
{
		// bookmark title/url may have been altered by the controller's edit method, so reload it
    var bm = window.bookmarksController.findBookmark(id);
    window.bookmarksController.showBookmarkEditDialog(bm.title,bm.url,id);
}

function _deleteBookmark(r,bmid)
{ 
  window.bookmarksController.deleteBookmark(bmid);
  $('#'+bmid).remove();
}


function _openUrl(ele, bmid) {
    if(__timerId != "")
    {
       window.clearTimeout(__timerId);
        __timerId = "";
    }
    // DragStart & DragEnd listeners are defined at bottom
    if (__sortingEnabled && (ele.parentNode!=__previousSortEle))
    {
        _disableSorting(__previousSortEle)
        return false;
    }
    else if (__sortingEnabled && (ele.parentNode==__previousSortEle))
        {
            return false;
        }
    else if (__dragendFlag)
    {
        __dragendFlag = false;
        return false;
    }
    else if(__cancelLinkOpening)
    {
    	__cancelLinkOpening = false;
    	return false;
    }

    window.views.WebView.showNormalPage();
    window.ViewStack.switchView( "WebView","BookmarkTreeView");

    // Laod a page to chrome view
    // bookmark title/url may have been altered by the controller's edit method, so reload it
    var bm = window.bookmarksController.findBookmark(bmid);
    window.views.WebView.loadUrlToCurrentPage(bm.url);
    views.WebView.gesturesEnabled = true;
}

function _updateBookmarkViewGoemetry(displayMode)
{
    // It's too bad we couldn't use margin-right instead of width coz then we could just put a static value in the .css file and not even need to do anything
    $('.dogEarBox.closed').next('.bookmarkItem').css('width', (parseInt(window.innerWidth)-70)+"px");
    $('.dogEarBox').not('.closed').next('.bookmarkItem').css('width', (parseInt(window.innerWidth)-220)+"px");
}

function _setDogear()
{
	__dogearSelected = true;
}

function _unsetDogear()
{
	__dogearSelected = false;
}

function _createBookmarkElement(bmtitle,bmfullurl,idValue)
{
        var _width = parseInt(window.innerWidth)-70;
        var escapedTitle = bmtitle.replace(/&#39/g, "\\'"); 
        var li=document.createElement('li');
        li.id = idValue;
        li.innerHTML =
        '<div class="dogEarBox closed">'+ // bookmarkBox
          '<div class="dogEar closed"></div>'+
          '<img class="bookmarkEditBtn" src="icons/edit_btn.png" width="56" height="56" vspace="7" hspace="5">'+
          '<img class="bookmarkDeleteBtn" src="icons/delete_btn.png" width="56" height="56" vspace="7" hspace="5" >'+
        '</div>'+
        '<div class="bookmarkItem" style="width:'+_width+'px;">'+
          '<span class="aTitle">'+bmtitle+'</span>'+
          '<br/>'+
          '<span class="aUrl">'+bmfullurl+'</span>'+
        '</div>';
        $(li).find(".dogEar")
               .click(_bookmarkToggleControls)
               .mouseover(_setDogear)
               .mouseout(_unsetDogear);
        $(li).find(".bookmarkItem").
               click(function (event) {_openUrl(event.target, idValue);});
        $(li).find(".bookmarkEditBtn").
               click(function (event) {_launchEditBookmark(event.target, bmtitle, bmfullurl, idValue);});
        $(li).find(".bookmarkDeleteBtn").
               click(function (event) {_deleteBookmark(event.target, idValue);});

        return li;
}

function _createBookmarkView()
{
        try
        {
            __currentLongPress = undefined;
            //Get bookmarks data from database
            window.bookmarksController.findAllBookmarks();
            var ul=document.getElementById('bookmarkListTree');
            while (window.bookmarksController.hasMoreBookmarks())
            {
                var bm = window.bookmarksController.nextBookmark();
                var bmurl = bm.url;
                var bmtitle = bm.title;
                var id = bm.id;

                //create element and add it to bookmark view
                var li = _createBookmarkElement(bmtitle,bmurl,id);
                $(li).addClass('no-sort');
                ul.appendChild(li);
                ul.childNodes[0].focus();
                if (!window.views.WebView.bedrockTiledBackingStoreEnabled())
                    new LongPress(li.id, _longPress, _longPressStarted, 250);
                else
                    new LongPress(li.id, _longPress, _longPressStarted, 550);
                		
                __bookmarkCount++;
            }
        } catch(E) { alert(E); }

        _dragInit();
}

function launchBookmarkView()
{
    try{
        window.chrome.aspectChanged.connect(_updateBookmarkViewGoemetry);
        window.bookmarksController.bookmarkAdded.connect(_addNewBookmark);
        window.bookmarksController.bookmarkModified.connect(_editBookmark);

        // Get Bookmarks from the database
        _createBookmarkView();

        // Watch for the start of scrolling in the view so we can cancel the
        // long presses to prevent them from interfere with scrolling.  Otherwise
        // the long press callback will fire and trigger sortable mode.
        views.WebView.startingPanGesture.connect(_handlePanStarted);

    }catch(e){ alert(e); }
}