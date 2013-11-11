

class ApplicationControl
  # Run when the class is intialized. Takes logging file and MATTI sut in.
  def initialize(log, my_sut)
  	@log = log
  	@my_sut = my_sut
  end
	
	# Starts Logs application
	def startApplication()
		puts "starting application"
		@log.debug("Starting logs.exe application")
		@my_app = @my_sut.run(:name=>'logs.exe')
		sleep 4
	#		@my_sut.capture_screen(:Filename => 'c:/temp/live.png')
	#	puts @my_sut.application.attribute("FullName")
	#	f = File.new('c:\temp\decorator.xml', "w")
	#	xml = @my_sut.get_ui_dump()
	#	f.puts xml
		
		return @my_app
	end
	
	# Closes Logs application
	def closeApplication()
		puts "closing application"
		@log.debug("Closing logs.exe application")
		@my_app.close()
	end
	
	# Clears the database. 
	# An external test application is called that clears the database used by Logs application
	def clearList
		begin
  		@my_sut.run(:name=>'clearlisttest.exe')
  	rescue
			# Expected, as the exe does not stay running
  	end
  end
  
  # Adds events to database. 
	# An external test application is called that adds events to database used by Logs application.
	# The test application takes in an argument that specifies which events should be added, e.g. case1.
  def addEvents(caseName)
  	begin
  		puts caseName
  		@my_sut.run(:name=>"logclienttest.exe #{caseName}")	
  	rescue
  		# Expected, as the exe does not stay running
  	end	
	end
	
	# Starts the call service monitor test application
  def startCallService()
  	begin
  		puts caseName
  		@serviceApp = @my_sut.run(:name=>"servicepp2.exe")	
  	rescue
 			# Some crashes my occure during startup
  	end	
	end
	
	
  # Start application by pressing Send key. Currently a test application is used as the Send key does not work
  def pressSendKey()
  	@log.debug("running logsservicestester.exe 2")
  	## matti does not support pressing keys in QT side at the moment. IT will be added in maybe 0.6 version.
  	#MATTI.disconnect_sut(:Id => 'sut_s60_qt')
		#sut = MATTI.sut(:Id => 's60')
		#sut.press_key(:kEnd)
		#sleep 4
		sut.press_key(:kSend)		
  	#MATTI.disconnect_sut(:Id => 's60')
  	#@my_sut = MATTI.sut(:Id => 'sut_s60_qt')
  	#begin
  		#@my_sut.run(:name=>'logsservicestester.exe 2')
  	#rescue
  		# Expected, as the exe does not stay running
  	#end
	end
  
  
end
	