require 'ApplicationControl.rb'
require 'CommonFunctions.rb'

class TC_Toolbar < CommonFunctions
	
	## Test that log is sorted to contain only dialled calls.  
	def sort_call_log_dialled_calls
		return sort_call_log("Dialled", DirOutIcon_Const)
  end
  
  ## Test that log is sorted to contain only received calls. 
  def sort_call_log_received_calls
		return sort_call_log("Received", DirInIcon_Const)
  end
  
  ## Test that log is sorted to contain only missed calls. 
  def sort_call_log_missed_calls
		return sort_call_log("Missed", DirMissedIcon_Const)
  end
  
  ## Common functionality for sorting the calls
  ## It sorts the log according to criteria passed in and verfies that the list is sorted
  ## Finally it switches back to Recent view and verifies the list again
  def sort_call_log(criteria, icon)
  	initializeTest("case8")
		selectView("Recent", criteria)

		@log.debug("Verify that only #{criteria} calls are displayed and they are in correct order")
		
		if(criteria == "Dialled")
			verify_event("1", "+358012345678910", DirOutIcon_Const, VoiceIcon_Const)
			verify_event("0", "+3580123456789", DirOutIcon_Const, VoiceIcon_Const)
		elsif(criteria == "Received")
			verify_event("1", "88012345678910", DirInIcon_Const, VoiceIcon_Const)
			verify_event("0", "880123456789", DirInIcon_Const, VoiceIcon_Const)
		else
			verify_event("1", "088012345678910", DirMissedIcon_Const, VoiceIcon_Const)
			verify_event("0", "088012345678911", DirMissedIcon_Const, VoiceIcon_Const)
		end
				
		@log.debug("Change back to Recent view and verify that all events are displayed")
		selectView(criteria, "Recent")
		
		verify_event("5", "+358012345678910", DirOutIcon_Const, VoiceIcon_Const)
		verify_event("4", "+3580123456789", DirOutIcon_Const, VoiceIcon_Const)
		verify_event("3", "88012345678910", DirInIcon_Const, VoiceIcon_Const)
		verify_event("2", "880123456789", DirInIcon_Const, VoiceIcon_Const)
		verify_event("1", "088012345678910", DirMissedIcon_Const, VoiceIcon_Const)
		verify_event("0", "088012345678911", DirMissedIcon_Const, VoiceIcon_Const)
		
		result = 0
  	@appControl.closeApplication()
    return result
    
  end
end