require 'CommonFunctions.rb'

class TC_RecentCalls < CommonFunctions
	
	## Initializes the Recent view. Log events are created and application is started
	def initializeRecentView(caseName)
		initializeTest(caseName)
		@log.debug('Verifying that the view name is "Recent calls"')
	  @recentCallsView.HbLabel(:text=>'Recent calls')
	  #@appControl.startCallService()
	end

  ## Test initiating CS call to first dialled event in recent calls view
  def initiate_CS_call_to_first_dialled_event_in_recent_view
  	# call to function in CommonFunctions.rb
  	initializeRecentView("case8")
  	initiate_call("4", "+3580123456789")
  end
  
  ## Test initiating CS call to first missed event in recent calls view
  def initiate_CS_call_to_first_missed_event_in_recent_view
  	# call to function in CommonFunctions.rb
  	initializeRecentView("case8")
  	initiate_call("0", "088012345678911")
  end
  
  ## Test initiating CS call to first received event in recent calls view
  def initiate_CS_call_to_first_received_event_in_recent_view
  	# call to function in CommonFunctions.rb
  	initializeRecentView("case8")
  	initiate_call("2", "880123456789")
  end
  
  ## Test initiating a quick video callback by pressing Send key in recent calls view
  def initiate_CS_callback_in_recent_view
		initializeRecentView("case9")
		# call to function in CommonFunctions.rb
  	initiate_call_with_send_key()
	end
	 
  
end
