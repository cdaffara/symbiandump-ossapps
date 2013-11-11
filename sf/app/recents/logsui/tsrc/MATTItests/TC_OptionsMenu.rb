require 'CommonFunctions.rb'

class TC_OptionsMenu < CommonFunctions
	
	## Initializes the Application. Log events are created and application is started.
	def initializeOptionsMenu(caseName)
		initializeTest(caseName)
	end
	
	## Test that event list can be cleaned in recent view
  def clear_event_list_recent_view
  	initializeOptionsMenu("case8")
		@log.debug('Tapping the options menu')
		@mainWindow.HbTitlePane.tap
		@log.debug('Selecting "Clear list" options menu')
		@optionsMenu = @mainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer
		@optionsMenu.HbMenuItem(:row=>'1').HbTextItem(:text=>'Clear list').tap
		begin
			@log.debug('Verify that there are no event items anymore left')
			@eventContainer.HbListViewItem(:row=>"0")
  	rescue
  		#Expected, case passed as no items left anymore	
  		result = 0
	  	@appControl.closeApplication()
	    return result
  	end
  	@log.fatal('There are still event items left!')
  	return -2
  end
  
  ## Test that event list can be cleaned in recent view
  def clear_event_list_missed_view
  	return clear_event_list("Missed")
  end
  
  ## Test that event list can be cleaned in recent view
  def clear_event_list_dialled_view
  	return clear_event_list("Dialled")
  end
  
  ## Test that event list can be cleaned in recent view
  def clear_event_list_received_view
  	return clear_event_list("Received")
  end
  
  def clear_event_list(view)
  	initializeOptionsMenu("case8")
  	selectView("Recent", view)
		@log.debug('Tapping the options menu')
		@mainWindow.HbTitlePane.tap
		@log.debug('Selecting "Clear list" options menu')
		@optionsMenu = @mainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer
		@optionsMenu.HbMenuItem(:row=>'1').HbTextItem(:text=>'Clear list').tap
		begin
			@log.debug('Verify that there are no event items anymore left')
  		@eventContainer.HbListViewItem(:row=>"0")
  	rescue
  		#Expected, no items left in selected view view
  		@log.debug("No event items anymore left in #{view} view")
  		@log.debug('Verify Recent view, all other event items should be still there')
  		if(view == "Missed")
  			selectView("Missed", "Recent")
  			verify_event("0", "880123456789", DirInIcon_Const, VoiceIcon_Const)
  			verify_event("3", "+358012345678910", DirOutIcon_Const, VoiceIcon_Const)
  		elsif(view == "Dialled")
  			selectView("Dialled", "Recent")
  			verify_event("0", "088012345678911", DirMissedIcon_Const, VoiceIcon_Const)
  			verify_event("3", "88012345678910", DirInIcon_Const, VoiceIcon_Const)
  		else
 				selectView("Received", "Recent")
 				verify_event("0", "088012345678911", DirMissedIcon_Const, VoiceIcon_Const)
 				verify_event("3", "+358012345678910", DirOutIcon_Const, VoiceIcon_Const)
  		end
  		result = 0
  		@appControl.closeApplication()
    	return result
  	end
  	@log.fatal('There are still event items left in #{view} view!')
  	return -2
  end
  
 	## Test that logs application is closed when tapping on back
  def exit_logs_by_selecting_back		
  	initializeOptionsMenu("case8")
		@log.debug('Tapping the Back button')
		@my_app.LogsMainWindow.HbSoftKey({:visibleOnScreen => 'true'}).tap 
  	if(@my_sut.application.attribute("FullName") == "Z:\sys\bin\logs.exe")
  	  log.fatal("Error in closing logs application from Exit options menu. Logs still running!") 
  	  appControl.closeApplication()
  	  result = -2
  	else
  		result = 0 
  	end
    return result
  end
  
  ## Test that logs application is closed when selecting exit from options menu
  def exit_logs_by_selecting_exit_from_options_menu
  	initializeOptionsMenu("case8")
		@log.debug('Tapping the options menu')
		@my_app.LogsMainWindow.HbTitlePane.tap
		@log.debug('Selecting "Exit" options menu')
		@my_app.LogsMainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer.HbMenuItem(:row=>'3').HbTextItem(:text=>'Exit').tap
  	if(@my_sut.application.attribute("FullName") == "Z:\sys\bin\logs.exe")
  	  log.fatal("Error in closing logs application from Exit options menu. Logs still running!") 
  	  appControl.closeApplication()
  	  result = -2
  	else
  		result = 0 
  	end
    return result
  end
  


end


 	#@my_sut.capture_screen(:Filename => 'c:/temp/live.png')
		#puts @my_sut.application.attribute("FullName")
		#f = File.new('c:\temp\decorator.xml', "w")
		#xml = @my_sut.get_ui_dump()
		#f.puts xml
