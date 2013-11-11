#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
#!/usr/bin/perl

use File::Copy;
use Cwd;
use Getopt::Long;
use XML::Parser::Expat;
use Data::Dumper;

#---------------------------------------Initialization------------------------------------------------------#

$projectrootname = "/"; #is set to correct at run-time 
$projectdrive = "z:"; #is set to correct at run-time 

$unitTestRunner = "unittest_qt.pl";
$scriptLocation = "logsui/tsrc/unittestrunner";
$coverageResultsDirDefault = "logsui/tsrc/unittestrunner/qtresults/";
$testConfigDefault = "unittest_qt_config.txt";
$qtProFileDefault = "tsrc.pro";
$buildResults = "BuildResults.txt";
$coverageDat = "MON.dat";
$coverageSymbols = "MON.sym";
$coverageProfile = "profile.txt";

$coverageResultsFile = "CTCHTML/index.html";
$finalResultsFile = "AllResults.html";
$cssFile = "CTCHTML/ctc.css";
$cssLink = "<link rel=\"stylesheet\"";

$xmlDllLine = "TestCase";
$xmlResultLine = "Incident";
$xmlCaseFailed = "failed";
$xmlCasePassed = "pass";
$outputString = "";
$outputFileBodyStart = "<body";

$totalCount = 0;
$passedCount = 0;
$failedCount = 0;

$qtestOutputString = "";

my @testNamesAndLogFiles = ( [""],[""] ); # two dimensional array e.g. [name, log1, log2], [name2, log3, log5, log6]

# from command line
my ($param_noclean,
		$testConfig,
		$qtProFile,
		$coverageResultsDir);

#---------------------------------------Main Start----------------------------------------------------------#

# read command line parameters
my $result = GetOptions("noclean" 			  => \$param_noclean,
												"config:s"	    => \$testConfig,
												"pro:s"         => \$qtProFile,
												"results:s"       => \$coverageResultsDir,
												"help"						=> \$help);
												
$startdir = cwd;

if (defined $help){
	print_help();
	exit;
}

findProjectDriveAndRoot();

createResultsDir();

# set target for intrumentation result
$ENV{'CTC_DATA_PATH'}= "$coverageResultsDir";	

doClean();

parseConfigFile();

buildTests();

runTests();

chdir($startdir);
	
# create textual output
chdir("$coverageResultsDir");
doSystemCall("ctcpost $coverageSymbols $coverageDat -p $coverageProfile -T 70");
doSystemCall("ctc2html -i $coverageProfile -t 70");

# clear target for intrumentation result
$ENV{'CTC_DATA_PATH'}= "";	

combineResults();

chdir($startdir);

#---------------------------------------Main Ends-------------------------------------------------------------#

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub findProjectDriveAndRoot()
{
		$tempStartDir = $startdir;
		if ($tempStartDir =~ m/$scriptLocation/){
			# extract project root
			for ($tempStartDir) {
				 s/$scriptLocation+$//;
    	}
    	# divide it to drive and root
    	if ($tempStartDir =~ /^(.:)(.*)/){
    		$projectdrive = $1;
    		$projectrootname = $2;
			}
			print "project drive: $projectdrive \n";
			print "project root: $projectrootname \n";
		}
		else{
			print "cannot determine project drive and root, use defaults!\n";
		}
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub createResultsDir()
{
    # create directory for results
    if ( defined $coverageResultsDir ){
        if ($coverageResultsDir =~ /^(.:)/){
            print("Drive name given in results dir arg\n");
        }
        else{  
            $coverageResultsDir = "$projectdrive" . "/" . "$coverageResultsDir";
        }
    }
    else{
        $coverageResultsDir = "$projectdrive$projectrootname$coverageResultsDirDefault";
    }
    print ("Writing results to $coverageResultsDir \n\n");
    if (mkdir("$coverageResultsDir", 0755)){
        print("The results directory was created successfully! \n");
    } 
    else {
        print("The results directory already exists. \n");
    } 
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub doClean()
{
    if (!defined $param_noclean)
    {
    	# clear previous results
    	print("Cleaning previous results. \n");
    	unlink("$coverageResultsDir$buildResults");
    	unlink("$coverageResultsDir$coverageDat");
    	unlink("$coverageResultsDir$coverageSymbols");
    	unlink("$coverageResultsDir$coverageProfile");
    	unlink("$coverageResultsDir$finalResultsFile");
    }
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub parseConfigFile()
{
    if ( !defined $testConfig ){
        print("Config file not defined, using default. \n");
        $testConfig = $testConfigDefault;
    }
    
    open(CONFIG, $testConfig) or die("file $testConfig not found!\n");    

    @config_file_content = <CONFIG>;
    
  
    for ($j = 0; $j <= $#config_file_content; $j++)
	  {
	     # remove \n from each line
	    $currline = @config_file_content[$j];
	    for ($currline) {
	        s/\n+$//;
	    }
	    @{ $testNamesAndLogFiles [$j] } = split( ",", $currline );
	  }
	  
	  print Dumper( @testNamesAndLogFiles );
	  
    close(CONFIG);
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub buildTests()
{
    if ( !defined $qtProFile ){
        print("Pro file not defined, using default. \n");
        $qtProFile = $qtProFileDefault;
    }
    doSystemCall( "qmake $qtProFile" );
    doSystemCall( "sbs reallyclean" );
    doSystemCall( "qmake $qtProFile" );
    
    $exclude = "-C \"EXCLUDE+*\tsrc\*\" -C \"EXCLUDE+*/*/tsrc/*\" -C \"EXCLUDE+*/*/*/tsrc/*\" -C \"EXCLUDE+moc_*\"";
    
    doSystemCall( "call ctcwrap -i d -n $coverageResultsDir$coverageSymbols $exclude -2comp sbs.bat -c winscw_udeb" );
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub runTests()
{
  for $i ( 0 .. $#testNamesAndLogFiles ) {
    $testName = $testNamesAndLogFiles[$i][0];
    print("Running tests for: $testName\n");
    
    $testCall = "\\epoc32\\release\\winscw\\udeb\\" . $testName . " -xml"; 
    doSystemCall( $testCall );
    
    for $j ( 1 .. $#{ $testNamesAndLogFiles [$i] } ) {
        $logFile = $projectdrive . "\\epoc32\\winscw\\c\\" . $testNamesAndLogFiles[$i][$j];
        print("Copying log file: $logFile\n");
        copy( $logFile, "$coverageResultsDir" );
    }
  }
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub combineResults()
{
    open(COVERAGE, "$coverageResultsDir$coverageResultsFile") or die("file $coverageResultsFile not found!\n");
    @coverageFileContent = <COVERAGE>;
    
    # append coverage information after eunit results
    
    for ($j = 0; $j <= $#coverageFileContent; $j++){
    		$currentLine = @coverageFileContent[$j];
    		
    		if ($currentLine =~ /$cssLink/){
    			if ( open(CSSFILE,  "$coverageResultsDir$cssFile") ){
    				# append css styles to results html
    				$outputString .= "<style type=\"text/css\"> body {";
    				@cssFileContent = <CSSFILE>;
    				my($line);
    				foreach $line (@cssFileContent){ 
    					$outputString .= $line;
    				}
    				$outputString .= "}</style>\n";
    				close(CSSFILE);
    			}
    		}
    		else{
    			$outputString .= $currentLine;
    
    			if ($currentLine =~ /$outputFileBodyStart/){
    				$outputString .= parseXmlResults();
    				$outputString .= "<br><br>"
    			}
    		}	
    	}
    	
    open(NEWRESULTSFILE, "> $coverageResultsDir$finalResultsFile") or die "Cannot open final results file\n";
    print NEWRESULTSFILE "$outputString";
    close(NEWRESULTSFILE);
    close(OUTPUT);
    close(COVERAGE);
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub parseXmlResults()
{
    $qtestOutputString = "<span class=\"head1\">QTestLib Results</span><br><br>\n";
    
    
    for $i ( 0 .. $#testNamesAndLogFiles ) {
        $testName = $testNamesAndLogFiles[$i][0];
        
        for $j ( 1 .. $#{ $testNamesAndLogFiles [$i] } ) {
            $logFile = $testNamesAndLogFiles[$i][$j];
            if ( -e "$coverageResultsDir$logFile" ){
                print("Parsing: $logFile\n");
                open(TESTRESULTS,  "$coverageResultsDir$logFile");
                $parser = new XML::Parser::Expat;
                $parser->setHandlers('Start' => \&sh,
                        'End'   => \&eh,
                        'Char'  => \&ch);
            
                $totalCount = 0;
    	          $passedCount = 0;
    	          $failedCount = 0;
    	          eval{
                    ### try block
                    $parser->parse(*TESTRESULTS);
                };
                if ($@){
                    ### catch block
                    print("$logFile, parsing failed\n");
    	             $qtestOutputString .= "<span class=\"red\">$logFile parsing failed, testcase execution might have failed </span><br>\n";
                };
    	          close(TESTRESULTS);
                
            }
            else{

                print("$logFile, not found\n");
                $qtestOutputString .= "<span class=\"head3\">";
                $qtestOutputString .= "$testName => </span>";
                $qtestOutputString .= "<span class=\"red\">$logFile not found, testcase building might have failed </span><br>\n";
            }
        }
      }
    
    return $qtestOutputString;
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub sh
{
  my ($p, $el, %atts) = @_;
	if ( $el eq $xmlDllLine )
	{
		$qtestOutputString .= "<span class=\"head3\">";
		$dllName = %atts->{name};
		@dllNameParts = split m!(\\)!, $dllName;
		$dllName = $dllNameParts[$#dllNameParts];
		$qtestOutputString .= "$dllName => </span>";
	}
	
	if ( $el eq $xmlResultLine )
	{
		$status = %atts->{type};
		if ( $status eq $xmlCasePassed )
		{
			$passedCount++;
		}
		else
		{
			$failedCount++;
		}
	}
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub eh
{
	my ($p, $el) = @_;
	if ( $el eq $xmlDllLine )
	{
		$totalCount = $passedCount + $failedCount;
		if ( $failedCount > 0 || $totalCount == 0 )
		{
			$qtestOutputString .= "<span class=\"red\">Testcases passed/run: $passedCount/$totalCount </span><br>\n";
		}
		else
		{
			$qtestOutputString .= "<span class=\"blue\">Testcases passed/run: $passedCount/$totalCount </span><br>\n";
		}
		$passedCount = 0;	
		$failedCount = 0;
	}
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub ch
{
	my ($p, $el) = @_;
	#print ("$el\n");
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub doSystemCall
{
	#print("\nDoing system call: $_[0]\n");
	#system($_[0]);
	system("echo Doing system call: $_[0] >> $coverageResultsDir$buildResults");
	system("$_[0] >> $coverageResultsDir$buildResults 2>&1");
}

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
#
sub print_help
{
	print("\n*************************************************************\n\n");
	print("Script runs by default all multimediasharing qt tests\n");
	print("and creates build, test and coverage results to:\n\n");
	print("    mmsharing/src/internal/unittestrunner/qtresults/\n\n");
	print("Own configuration file (list of components to be processed)\n");
	print("can be used as well:\n\n");
	print("    >unittest_ctc.pl -config=myowntestcomponents.txt\n\n");
	print("The text file should follow the syntax of default configuration file\n");
	print("mmsharing/src/internal/unittestrunner/unittest_qt_config.txt\n\n");
	print("When running just some set of components you might want to \"merge\" the results");
	print("with existing coverage results.\n");
	print("That is possible with command:\n\n");
	print("    >unittest_ctc.pl -eunitdlls=myowneunitcomponents.txt -noclean\n");
	print("\n*************************************************************\n");
}
