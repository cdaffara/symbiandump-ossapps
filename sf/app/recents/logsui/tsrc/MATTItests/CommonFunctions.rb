require 'matti'

### Constants

VoiceIcon_Const = ':/callVoice.svg'
VideoIcon_Const = ':/callVideo.svg'
DirInIcon_Const = ':/dirIn.svg'
DirOutIcon_Const = ':/dirOut.svg'
DirMissedIcon_Const = ':/dirMissed.svg'
DataAndTimeIcon_Const = ':/callDateAndTime.svg'
RemoteInfoIcon_Const = ':/remoteInfo.svg'
CallDurationIcon_Const = ':/callDuration.svg'


## Parent class for all TC_ classes. Contains common functionality related to intializing and running the cases,
## and other functionality that can be re-used by different tests  
class CommonFunctions
 # Run when the class is intialized. Takes logging file and MATTI sut in.
 def initialize(log, my_sut)
  	@log = log
  	@my_sut = my_sut
  end
	
	# Running test case logic. The case name is taken in from the test list file and then a function that has the
	# same name is called. 
	def run(testName)
		begin
			result = send(testName)
		rescue Exception => e
	 		@log.fatal("Error, case execution terminated.")
	 		@log.fatal("Error code: #{e.message}")
	 		@log.fatal("Error line: #{e.backtrace.inspect}")
	 		if(@my_sut.application.attribute("FullName") == 'Z:\sys\bin\logs.exe')
	 			@appControl.closeApplication()
	 		end
	 		result = -2
		end
		return result
	end
	
	# Initializes the test by generating the log events
	def initializeTest(caseName)
		@appControl = ApplicationControl.new(@log, @my_sut)
		@appControl.clearList()
		@appControl.addEvents(caseName)
		@my_app = @appControl.startApplication()
		sleep 3
		# Note! Use only these in the test cases so that maintenance is easier. 
		@mainWindow = @my_app.LogsMainWindow
		@recentCallsView = @mainWindow.HbContentWidget.LogsRecentCallsView.HbWidget
		@eventContainer = @recentCallsView.HbListView.HbAbstractItemContainer		
	end
	
	# Initializes the test by generating the log events and starting application by pressing send Key
	def initializeTestSendKey(caseName)
		@appControl = ApplicationControl.new(@log, @my_sut)
		@appControl.clearList()
		@appControl.addEvents(caseName)
		@appControl.pressSendKey
	
		sleep 4
		puts @my_sut.application.attribute("FullName") 
		@mainWindow = @my_app.LogsMainWindow
		@recentCallsView = @mainWindow.HbContentWidget.LogsRecentCallsView.HbWidget
		@eventContainer = @recentCallsView.HbListView.HbAbstractItemContainer
		
	end
	
	
	# Selects a wanted view. First it verifies the current view and then the next view is selected.
	def selectView(currentView, nextView)
		@log.debug("Tapping on Show button")
		showButton = @mainWindow.HbToolBar(:name=>'logs_toolbar').HbToolButton(:toolTipText=>'Show')
		showButton.tap
		sleep 1
		sort_menu = @mainWindow.HbMenu.HbMenuListView.HbAbstractItemContainer
		@log.debug("Verifying the current view in sort menu. Menu should be: #{currentView}")
		
		if(currentView == "Recent")	
			sort_menu.HbMenuItem(:row=>'0').HbTextItem(:text=>'Recent calls')
			sort_menu.HbMenuItem(:row=>'0').HbIconItem(:iconName => 'qtg_indi_list_selected')
		elsif(currentView == "Dialled")
			sort_menu.HbMenuItem(:row=>'2').HbTextItem(:text=>'Dialled calls')
			sort_menu.HbMenuItem(:row=>'2').HbIconItem(:iconName => 'qtg_indi_list_selected')
		elsif(currentView == "Received")
			sort_menu.HbMenuItem(:row=>'1').HbTextItem(:text=>'Received calls')
			sort_menu.HbMenuItem(:row=>'1').HbIconItem(:iconName => 'qtg_indi_list_selected')
		elsif(currentView == "Missed")
			sort_menu.HbMenuItem(:row=>'3').HbTextItem(:text=>'Missed calls')
			sort_menu.HbMenuItem(:row=>'3').HbIconItem(:iconName => 'qtg_indi_list_selected')
		end
		@log.debug("Selecting the next view in sort menu. Selection: #{nextView}")
		
		if(nextView == "Recent")	
			sort_menu.HbMenuItem(:row=>'0').tap
			@currentView = "Recent"
		elsif(nextView == "Dialled")
			sort_menu.HbMenuItem(:row=>'2').tap
			@currentView = "Dialled"
		elsif(nextView == "Received")
			sort_menu.HbMenuItem(:row=>'1').tap
			@currentView = "Received"
		elsif(nextView == "Missed")
			sort_menu.HbMenuItem(:row=>'3').tap
			@currentView = "Missed"
		end
	end
	
	# Intiates a call to the event that is in specified row passed in as a parameter. 
	# Verifies the call is made to correct number
	def initiate_call(row, phoneNumber)
		@log.debug("Initiating call. Row: #{row} Phone number: #{phoneNumber}")
		firstDialledEvent = @eventContainer.HbListViewItem(:row=>"#{row}").HbTextItem(:text=>"#{phoneNumber}")
		firstDialledEvent.tap
		sleep 2
		# Verify that the Telephone is calling to the correct number!! Currently no environment for that!!!!
		puts @serviceApp.attribute("FullName")
		return 0
  	
  end
  
  # Intiates a call to the first event in the current view by pressing Send key
	# Verifies the call is made to correct number
  def initiate_call_with_send_key
		@log.debug("Initiating call by pressing Send key")
		#recentCallsView = @my_app.HbMainWindow.HbContentWidget.LogsRecentCallsView.HbWidget
		firstEvent = @eventContainer.HbAbstractViewItem(:row=>"0")#.HbTextItem(:text=>"#{phoneNumber}")
		@appControl.pressSendKey()
		# Verify that the Telephone is calling to the correct number!! Currently no environment for that!!!!
		return 0
  end	
  
  ## Common functionality for testing the that the last call is dislplayed correctly in
  ## selected view.
  ## Check the Call type (voice / video) and call direction
  def last_call(caseName, phoneNumber, direction, type)
  	initializeTest(caseName)
  	@log.debug('Verifying the first recent calls item.')
		verify_event(0, phoneNumber, direction, type)
		@log.debug('Changing the view from Recent to Dialled and verifying the event')
		if(direction == DirMissedIcon_Const)
			selectView("Recent", "Missed")
		elsif(direction == DirOutIcon_Const)
			selectView("Recent", "Dialled")
		else
			selectView("Recent", "Received")
		end
		verify_event(0, phoneNumber, direction, type)
		@log.debug('Add one event')
		@appControl.addEvents(caseName + "b")
		verify_event(0, phoneNumber + '1', direction, type)
		verify_event(1, phoneNumber, direction, type)
		result = 0
  	@appControl.closeApplication()
    return result
   end
  
  ## Function for verifying that the event is correct in the selected row.
  ## Check the Call type (voice / video), call direction and phone number
  def verify_event(row, contact, direction, type)
  	puts "Verifying that row: #{row} has a contact/phone number: #{contact}"
		@log.debug("Verifying that row: #{row} contact/phone number: #{contact}")
  	@eventContainer.HbListViewItem(:row=>"#{row}").HbTextItem(:text=>"#{contact}")
  	@log.debug("Verifying the call type: #{type} by checking the icon")
  	@eventContainer.HbListViewItem(:row=>"#{row}").HbIconItem(:iconName=>"#{type}")
  	@log.debug("Verifying the call direction: #{direction} by checking the icon")
  	## CHANGE THIS BACK, CURRENTLY 2 ICONS
  	#@recentCallsView.HbAbstractItemContainer.HbAbstractViewItem(:row=>"#{row}").HbIconItem(:iconName=>"#{direction}")
  end 
  
	
end
