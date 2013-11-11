//clearhistorydialog.js

function clearHistoryDialog()
{
	window.historyManager.confirmHistoryClear.connect(showClearHistoryDialog);
	  
    this.write = writeClearHistoryDialog;
    // do setup
    this.write();
    
}

function writeClearHistoryDialog()
{
    var message = window.localeDelegate.translateText("txt_browser_history_delete_are_you_sure");
    var html =
        '<div class="textLabel">'+message +'</div>' +
        '<div class="controls">' +
            '<div type="button" onmouseup="clearAllHistoryDialog();" class="historyDialogButton clearDoneButton"></div>'+ 
            '<div type="button" onmouseup="clearHistoryDialogIdHide();" class="historyDialogButton clearCancelButton"></div>'+         
        '</div>' +
        '<div id="hiddenDialogOK"></div>' +
        '<div id="hiddenDialogCancel"></div>';
  document.write(html);
}

function showClearHistoryDialog() {
  try{
      window.snippets.ClearHistoryDialogId.show(false);
      window.snippets.RecentUrlViewToolbarId.enabled = false;
  }catch(e){ alert(e); }
} 

function clearAllHistoryDialog()
{
    window.historyManager.clearHistory();
    window.views.WebView.reload();    
    clearHistoryDialogIdHide();
}

function clearHistoryDialogIdHide(){
    window.snippets.ClearHistoryDialogId.hide();
    window.snippets.RecentUrlViewToolbarId.enabled = true;
}
