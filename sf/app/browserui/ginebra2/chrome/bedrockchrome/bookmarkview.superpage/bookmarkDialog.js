var _dailogFlag = 0;
var _OriginalTitle = 0;

document.getElementsByClassName = function(class_name) {
    var docList = this.all || this.getElementsByTagName('*');
    var matchArray = new Array();

    /*Create a regular expression object for class*/
    var re = new RegExp("(?:^|\\s)"+class_name+"(?:\\s|$)");
    for (var i = 0; i < docList.length; i++) {
        if (re.test(docList[i].className) ) {
            matchArray[matchArray.length] = docList[i];
        }
    }

	return matchArray;
}

function bookmarkDialog()
{
    this.write = writeBookmarkDialog;

    // do setup
    this.write();

    if (app.serviceFramework() == "mobility_service_framework")
    {
        document.getElementsByClassName("bookmarkCheckboxTextLabel")[0].style.display = "inline";
    }
    
}

function writeBookmarkDialog()
{
    var html =
               '<div class="top"></div>'+
                '<div class="body">'+
                   '<div class="bookmarkTextLabel" id="bookmarkDialogTitle">Add Bookmark</div>'+
                   '<div class="GinebraSnippet" id="BookmarkDialogTitleId" data-GinebraNativeClass="TextEditSnippet" data-GinebraVisible="true"></div>'+ 
                   '<div class="GinebraSnippet" id="BookmarkDialogUrlId" data-GinebraNativeClass="TextEditSnippet" data-GinebraVisible="true"></div>'+
                   '<div class="bookmarkCheckboxTextLabel"><input type="checkbox" id="bookmarkCheckboxId"/>Add shortcut to home screen</div>'+
                   '<div><input type="hidden" id="BookmarkDialogBookmarkId" name="BookmarkDialogBookmarkId" value=""/></div>'+
                   '<div class="controls">' +
                      '<div type="button"  onmouseup="bookmarkOperation();" class="bookmarkDoneButton"></div>'+     
                       '<div type="button" onmouseup="bookmarkDialogIdHide();" class="bookmarkCancelButton"></div>'+  
                    '</div>'+ /*controls*/
                '</div>'+  /*body*/
                '<div class="bottom"></div>';
  document.write(html);
}

function bookmarkDialogIdHide(){
    window.snippets.BookmarkDialogId.hide();
    snippets.BookmarkViewToolbarId.enabled = true;
    snippets.WebViewToolbarId.enabled = true;
    
    if (app.serviceFramework() == "mobility_service_framework") 
    {
        if (document.getElementById("bookmarkCheckboxId").checked)
        {
            document.getElementById("bookmarkCheckboxId").checked = false;
        }
    }    
}

function launchBookmarkDialog(bmtitle, bmurl, bmid, dialogFlag)
{
    try{
        snippets.BookmarkViewToolbarId.enabled = false;
        snippets.WebViewToolbarId.enabled = false;

        if (dialogFlag == 0) {
            if (app.serviceFramework() == "mobility_service_framework")
            {
                document.getElementsByClassName("bookmarkCheckboxTextLabel")[0].style.display = "inline"; 
            }
            var dlgTitle = document.getElementById("bookmarkDialogTitle");
            dlgTitle.firstChild.nodeValue= window.localeDelegate.translateText("txt_browser_input_dial_add_bm");
            var chkboxTitle = document.getElementsByClassName("bookmarkCheckboxTextLabel")[0];
            chkboxTitle.firstChild.nodeValue= window.localeDelegate.translateText("txt_browser_bookmarks_also_add_to_home_screen");
        }
        else if (dialogFlag == 1) {
             document.getElementsByClassName("bookmarkCheckboxTextLabel")[0].style.display = "none";
             var dlgTitle = document.getElementById("bookmarkDialogTitle");
             dlgTitle.firstChild.nodeValue= window.localeDelegate.translateText("txt_browser_input_dial_edit_bm");
        }
       
        _dailogFlag = dialogFlag;
        _OriginalTitle = bmtitle;
        window.snippets.BookmarkDialogTitleId.lostFocus.connect(titleFieldLostFocus);
        window.snippets.BookmarkDialogUrlId.lostFocus.connect(urlFieldLostFocus);
        window.snippets.BookmarkDialogTitleId.gainedFocus.connect(titleFieldGainedFocus);
        window.snippets.BookmarkDialogUrlId.gainedFocus.connect(urlFieldGainedFocus);
        // set max text length
        // window.snippets.BookmarkDialogTitleId.setMaxTextLength(30);
        var hints = window.snippets.BookmarkDialogUrlId.getTextOptions();
        hints |= 2; // Qt::ImhNoAutoUppercase 0x2
        window.snippets.BookmarkDialogUrlId.setTextOptions(hints);
        if (bmtitle == "")
            window.snippets.BookmarkDialogTitleId.text = "Title";
        else
            window.snippets.BookmarkDialogTitleId.text = bmtitle;    
  
        if (bmurl == "")
            window.snippets.BookmarkDialogUrlId.text = "Url";
        else
           window.snippets.BookmarkDialogUrlId.text = bmurl;
        document.getElementById('BookmarkDialogBookmarkId').value = bmid;

        window.snippets.BookmarkDialogId.show(false);

        window.snippets.BookmarkDialogTitleId.selectAll();
        
       }catch(e){ alert(e); }

}

function bookmarkOperation()
{
    snippets.BookmarkViewToolbarId.enabled = true;
    snippets.WebViewToolbarId.enabled = true;
    //get title and url from the dialog
    var bmtitle = window.snippets.BookmarkDialogTitleId.text;
    var bmurl = window.snippets.BookmarkDialogUrlId.text;
    //Hide the dialog
    window.snippets.BookmarkDialogId.hide();
    //Update the database
    var errCode = 0;

    if (_dailogFlag == 0) {
        var bmid = window.bookmarksController.addBookmark(bmtitle,bmurl);
        if (bmid < 0) {
            alert("Unknown error adding bookmark");
            return;
        }
        if (app.serviceFramework() == "mobility_service_framework") 
        {
            if (document.getElementById("bookmarkCheckboxId").checked)
            {
                errCode = window.hsBookmarkPublishClient.addWidget(bmtitle, bmurl);
                document.getElementById("bookmarkCheckboxId").checked = false;
            }
        } 
    }
    else if (_dailogFlag == 1) {
        var bmid = document.getElementById('BookmarkDialogBookmarkId').value;
        errCode = window.bookmarksController.modifyBookmark(bmid,bmtitle,bmurl);
    }
    
    if (errCode == -3){
	     alert("Bookmark Url Is Empty");
	     return;
    }
    else if (errCode != -0){
	       alert("General Error");
	       return;
    }
     
	
}

function titleFieldLostFocus()
{
    var bmtitle = window.snippets.BookmarkDialogTitleId.text;
    if (bmtitle == "")
       window.snippets.BookmarkDialogTitleId.text = "Title";
}

function urlFieldLostFocus()
{
    var bmurl= window.snippets.BookmarkDialogUrlId.text;
    if (bmurl == "")
       window.snippets.BookmarkDialogUrlId.text = "Url";
}


function titleFieldGainedFocus()
{
    window.snippets.BookmarkDialogUrlId.unselect();
}

function urlFieldGainedFocus()
{
    window.snippets.BookmarkDialogTitleId.unselect();
}


function showBookmarkEditDialog(bmtitle,bmurl,id) {
    launchBookmarkDialog(bmtitle,bmurl,id,1);
}
