require 'CommonFunctions.rb'

class TC_DialledCalls < CommonFunctions
	
	## Initializes the Dialled view. Log events are created and application is started.
	## Calls are sorted by Dialled option
	def initializeDialledView(caseName)
		initializeTest(caseName)
		selectView("Recent", "Dialled")
	end
	
	## Initializes the Dialled view. Log events are created and application is started by pressing Send key
	## Calls are sorted by Dialled option
	def intializeDialledViewSendKey(caseName)
		initializeTestSendKey(caseName)
		selectView("Recent", "Dialled")
	end
	
	## Test that last dialled CS call is shown in dialled view
  def last_dialled_CS_call_in_dialled_view
  	result = last_call("case1", "+35812345790", DirOutIcon_Const, VoiceIcon_Const)
  	return result
  end
  
  ## Test that last dialled video call is shown in dialled view
  def last_dialled_video_call_in_dialled_view
  	result = last_call("case2", "+358123457918888", DirOutIcon_Const, VideoIcon_Const)
  	return result
  end
	
	## Test initiating a quick CS callback in dialled calls view
	def initiate_CS_callback_in_dialled_view
		intializeDialledViewSendKey("case7")
		# call to function in CommonFunctions.rb
  	initiate_call_with_send_key()
	end
	
	## Test initiating a quick video callback in dialled calls view
	def initiate_video_callback_in_dialled_view
		intializeDialledViewSendKey("case9")
		# call to function in CommonFunctions.rb
  	initiate_call_with_send_key()
	end
	
	## Test initiating a quick video callback in dialled calls view once the 
	## list is scrolled down
	def initiate_video_callback_in_dialled_view_scrolling
		intializeDialledViewSendKey("case9")
		# call to function in CommonFunctions.rb
		# Scroll down the list!! IMPLEMENT THIS
  	initiate_call_with_send_key()
	end
	
	## Test initiating CS call to first event in dialled calls view
	def initiate_CS_call_to_first_event_in_dialled_view
		initializeDialledView("case8")
		# call to function in CommonFunctions.rb
  	initiate_call("0", "+3580123456789")
	end
	
	## Test initiating CS call to last event in dialled calls view
	def initiate_CS_call_to_last_event_in_dialled_view
		initializeDialledView("case8")
		# call to function in CommonFunctions.rb
  	initiate_call("1", "+358012345678910")
	end
	
	## Test that contact name is displyed correctly in dialled view
	def matching_contact_name_dialled_CS_call
		initializeDialledView("case10")
		#match_conctact("2", "Test")
		verify_event(1, "Test", DirOutIcon_Const, VoiceIcon_Const)
		@appControl.closeApplication()
		return 0
	end
	
	## Test that contact name is displyed correctly in dialled view
	def matching_contact_name_dialled_video_call
		initializeDialledView("case10")
		verify_event(0, "Testing The Test", DirOutIcon_Const, VideoIcon_Const)
		@appControl.closeApplication()
		return 0
	end

	
	## Test pressing Send key in Idle mode and verify that dialled calls view is opened
	def pressing_send_key_in_idle_mode
		intializeDialledViewSendKey("case8")
		sleep 5
		#my_app = @my_sut.application.attribute("FullName")
		#puts my_app
		#@my_sut.application.close
		#my_app = @my_sut.application.attribute("FullName")
		#puts my_app
		# Verfiy that the application is logs and the menu is dialled
		# case is not finished
		# Check the UI components that must be there. If not there, case fails
		#dialledCallsView = my_app.HbMainWindow.HbContentWidget.LogsDialledCallsView.HbWidget
		#@log.debug('Verifying that the view name is "Recent calls"')
	  #dialledCallsView.HbLabel(:text=>'Dialed calls')
	end
end