#!/usr/bin/env ruby
###################################################################
# Environment constants. Change these according to your environment
# Path to your script directory
@scriptdir = "C:/matti/script/QTLogs/"
# Path to your test case list
@testlist = "C:/matti/script/QTLogs/testlist.txt"
# Path to your log directory file
@logfile = "C:/matti/script/QTLogs/logs/log.txt"
# Path to test result file used by ATS3
@results = "C:/matti/script/QTLogs/logs/testresults.txt"
###################################################################

require 'logger'
require 'socket'
require 'ftools'
require 'matti'
require @scriptdir + 'TC_RecentCalls'
require @scriptdir + 'TC_DialledCalls'
require @scriptdir + 'TC_Toolbar'
require @scriptdir + 'TC_MissedCalls'
require @scriptdir + 'TC_ReceivedCalls'
require @scriptdir + 'TC_OptionsMenu'
require @scriptdir + 'TC_EventDetails'

# create Log file
#time = Time.now.strftime("%d-%m-%Y_%H-%M-%S")
#logname = time + ".log"
if(File.exists?(@logfile))
  File.delete(@logfile)
end
file = open(@logfile, File::WRONLY | File::CREAT)
log = Logger.new(file)
#log = Logger.new(@logdir + logname)
log.debug("Establishing HTI connection to the Phone")

#fileresults = open(@results, File::WRONLY | File::CREAT)
logResults = File.new(@results, File::WRONLY | File::CREAT)


begin
	@my_sut = MATTI.sut(:Id => 'sut_s60_qt')
	log.debug("Connection established")
rescue
	log.fatal("Error: HTI connection establishment failed")
	exit()
end


if(File.exists?(@testlist) == true)
	f = File.new(@testlist, "r")
	begin
		while (line = f.readline)
	  	line.chomp
	  	if(!line.include? "//")
		  	puts "Starting to run Test case: " + line[0..(line.index('-')-2)]
		  	log.debug("- - - - - - - - - - ")
		  	log.debug("- - - - - - - - - - ")
		    
		    #testname = line[(line.index('.')+1)..line.length].chomp
		    testname = line[(line.index('.')+1)..(line.index('-')-2)].chomp
		    logResults.puts("Title:[#{testname}]")
		    
		    if(line.include? "RecentCalls")
		    	log.debug("Set:[RecentCalls]")
		    	log.debug("Title:[#{testname}]")
		      testSet = TC_RecentCalls.new(log, @my_sut)
		    end
		    if(line.include? "Toolbar")
		    	log.debug("Set:[Toolbar]")
		    	log.debug("Title:[#{testname}]")
		      testSet = TC_Toolbar.new(log, @my_sut)
		    end
		    if(line.include? "DialledCalls")
		    	log.debug("Set:[DialledCalls]")
		    	log.debug("Title:[#{testname}]")
		      testSet = TC_DialledCalls.new(log, @my_sut)
		    end
		    if(line.include? "MissedCalls")
		    	log.debug("Set:[MissedCalls]")
		    	log.debug("Title:[#{testname}]")
		     	testSet = TC_MissedCalls.new(log, @my_sut)
		    end
		    if(line.include? "ReceivedCalls")
		    	log.debug("Set:[ReceivedCalls]")
		    	log.debug("Title:[#{testname}]")
		     	testSet = TC_ReceivedCalls.new(log, @my_sut)
		    end
		    if(line.include? "OptionsMenu")
		    	log.debug("Set:[OptionsMenu]")
		    	log.debug("Title:[#{testname}]")
		     	testSet = TC_OptionsMenu.new(log, @my_sut)
		    end
		    if(line.include? "EventDetails")
		    	log.debug("Set:[EventDetails]")
		    	log.debug("Title:[#{testname}]")
		     	testSet = TC_EventDetails.new(log, @my_sut)
		    end
		    result = testSet.run(testname)
		    
		    if result == 0
		    	puts 'Result: 0 [] ==> PASSED'
		    	log.debug('### Result: 0 [] ==> PASSED ###')
		    	logResults.puts('Result: 0 [] ==> PASSED')
		    else 
		    	puts "Result: #{result} [Case failed, check log] ==> FAILED"
		    	log.debug("### Result: #{result} [Case failed, check log] ==> FAILED ### ")
		    	logResults.puts("Result: #{result} [Case failed, check log] ==> FAILED")
		    end
		  end
		  
		end
	 rescue EOFError
	   f.close
	 end
else
	log.error('Test list not found')
end
	



		
	#@my_sut.capture_screen(:Filename => 'c:/temp/live.png')
	#puts @my_sut.application.attribute("FullName")
	#f = File.new('c:\temp\decorator.xml', "w")
	#xml = @my_sut.get_ui_dump()
	#f.puts xml