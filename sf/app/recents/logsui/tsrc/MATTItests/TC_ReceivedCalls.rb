require 'CommonFunctions.rb'

class TC_ReceivedCalls < CommonFunctions
	
	## Initializes the Received view. Log events are created and application is started.
	## Calls are sorted by Received option
	def initializeReceivedView(caseName)
		initializeTest(caseName)
		selectView("Recent", "Received")
	end
	
	## Initializes the Received view. Log events are created and application is started by pressing Send key
	## Calls are sorted by Received option
	def initializeReceivedViewSendKey(caseName)
		initializeTestSendKey(caseName)
		selectView("Recent", "Received")
	end
	
  ## Test that last received CS call is shown in received view
  def last_received_CS_call_in_received_view
  	result = last_call("case3", "358123457918889", DirInIcon_Const, VoiceIcon_Const)
  	return result
  end
  
  ## Test that last received video call is shown in received view
  def last_received_video_call_in_received_view
  	result = last_call("case4", "358123457918810", DirInIcon_Const, VideoIcon_Const)
  	return result
  end
	
	## Test initiating CS call to first event in received calls view
	def initiate_CS_call_to_first_event_in_received_view
		initializeReceivedView("case8")
		# call to function in CommonFunctions.rb
  	initiate_call("0", "880123456789")
	end
	
	## Test initiating CS call to last event in received calls view
	def initiate_CS_call_to_last_event_in_received_view
		initializeReceivedView("case8")
		# call to function in CommonFunctions.rb
  	initiate_call("1", "88012345678910")
	end
	
	## Test initiating a quick CS callback in received calls view
	def initiate_CS_callback_in_received_view
		initializeReceivedViewSendKey("case7")
		# call to function in CommonFunctions.rb
  	initiate_call_with_send_key()
	end
	
	## Test that contact name is displyed correctly in received view
	def matching_contact_name_received_CS_call
		initializeReceivedView("case10")
		verify_event(0, "Testing1 050", DirInIcon_Const, VoiceIcon_Const)
		@appControl.closeApplication()
		return 0
	end
	
end