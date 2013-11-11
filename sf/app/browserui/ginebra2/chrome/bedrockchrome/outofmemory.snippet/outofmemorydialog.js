//outofmemorydialog.js

function outOfMemoryDialog()
{
	window.pageController.lowMemory.connect(showOutOfMemoryDialog);
	window.pageController.outOfMemory.connect(showOutOfMemoryDialog);
	  
    this.write = writeOutOfMemoryDialog;
    // do setup
    this.write();
    
}

function writeOutOfMemoryDialog()
{
    var title = window.localeDelegate.translateText("txt_browser_error_dialog_device_low");
    var message = window.localeDelegate.translateText("txt_browser_error_dialog_close_some");
    var html =
        '<div><img src="outofmemory.snippet/icons/icon_dialog_error.png"/>&nbsp;&nbsp;<span class="title">' + title + '</span></div>' +
        '<div class="textLabel">' + message +'</div>' +
        '<div class="controls">' +
            '<div type="button" onmouseup="hideOutOfMemoryDialog();" class="outOfMemoryDialogButton outOfMemoryDoneButton"></div>'+ 
        '</div>'
    document.write(html);
}

function showOutOfMemoryDialog() {
    window.snippets.OutOfMemoryDialogId.show(false);
} 

function hideOutOfMemoryDialog(){
    window.snippets.OutOfMemoryDialogId.hide();
}
