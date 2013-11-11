require 'CommonFunctions.rb'

class TC_MissedCalls < CommonFunctions
	
	## Initializes the Missed view. Log events are created and application is started.
	## Calls are sorted by Missed option
	def initializeMissedView(caseName)
		initializeTest(caseName)
		selectView("Recent", "Missed")
	end
	
	## Initializes the Missed view. Log events are created and application is started by pressing Send ky
	## Calls are sorted by Missed option	
	def initializeMissedViewSendKey(caseName)
		initializeTestSendKey(caseName)
		selectView("Recent", "Missed")
	end
	
	## Test that last missed CS call is shown in missed view
  def last_missed_CS_call_in_missed_view
  	result = last_call("case5", "358123461", DirMissedIcon_Const, VoiceIcon_Const)
  	return result
  end
  
  ## Test that last missed video call is shown in missed view
  def last_missed_video_call_in_missed_view
  	result = last_call("case6", "558123461", DirMissedIcon_Const, VideoIcon_Const)
  	return result
  end
	
	## Test initiating CS call to first event in missed calls view
	def initiate_CS_call_to_first_event_in_missed_view
		initializeMissedView("case8")
		# call to function in CommonFunctions.rb
  	initiate_call("0", "088012345678911")
	end
	
	## Test initiating CS call to last event in missed calls view
	def initiate_CS_call_to_last_event_in_missed_view
		initializeMissedView("case8")
		# call to function in CommonFunctions.rb
  	initiate_call("1", "088012345678910")
	end
	
	## Test initiating a quick CS callback in missed calls view
	def initiate_CS_callback_in_missed_view
		initializeMissedViewSendKey("case7")
		# call to function in CommonFunctions.rb
  	initiate_call_with_send_key()
	end
	
	## Test that contact name is displyed correctly in missed view
	def matching_contact_name_missed_CS_call
		initializeMissedView("case10")
		verify_event(0, "Keke Rosberg", DirMissedIcon_Const, VoiceIcon_Const)
		@appControl.closeApplication()
		return 0
	end

end