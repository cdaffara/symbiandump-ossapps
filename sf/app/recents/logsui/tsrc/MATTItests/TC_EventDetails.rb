require 'CommonFunctions.rb'

class TC_EventDetails < CommonFunctions

  ## Test that dialled call event can be deleted from event specifc view
  def deleting_dialled_call_event_in_the_event_specific_view
  	initializeTest("case8")
  	delete_event_from_event_specific_view(4, "Dialled", "+358012345678910", DirOutIcon_Const)
  end
  
   ## Test that missed call event can be deleted from event specifc view
  def deleting_missed_call_event_in_the_event_specific_view
  	initializeTest("case8")
  	delete_event_from_event_specific_view(0, "Missed", "088012345678910", DirMissedIcon_Const)
  end
  
  ## Test that dialled call event can be deleted from event specifc view
  def deleting_received_call_event_in_the_event_specific_view
  	initializeTest("case8")
  	delete_event_from_event_specific_view(2, "Received", "88012345678910", DirInIcon_Const)
  end
  
  ## Test that event specific data is displayed correclty for dialled call. Contact has contact name
  def displaying_event_specific_data_dialled_CS_call_contact_name_assigned
  	initializeTest("case11")
  	@log.debug('Long tapping the dialled event')
  	@eventContainer.HbListViewItem(:row=>"7").long_tap
  	@mainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer.HbMenuItem(:row=>'2').tap
  	sleep 2
  	
  
  end
  
  
  
	
	def delete_event_from_event_specific_view(row, view, verificationPhoneNumber, direction)
		@log.debug('Long tapping the dialled event')
  	@eventContainer.HbListViewItem(:row=>"#{row}").long_tap
  	@mainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer.HbMenuItem(:row=>'2').tap
  	sleep 2
  	puts "tapping finished"
  	@log.debug('Tapping the options menu')
  	@mainWindow.HbTitlePane.tap
  	@log.debug('Selecting Delete Event from the options menu')
		@mainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer.HbMenuItem(:row=>'4').HbTextItem(:text=>'Delete event').tap
		
		verify_event("#{row}", verificationPhoneNumber, direction, VoiceIcon_Const)
		selectView("Recent", view)
		verify_event("0", verificationPhoneNumber, direction, VoiceIcon_Const)
		begin
			@eventContainer.HbListViewItem(:row=>"1")
		rescue 
			#Expected, as there should be only one event left
			@appControl.closeApplication()
			return 0
		end
		@appControl.closeApplication()
		return -2		
	end
	
	

end