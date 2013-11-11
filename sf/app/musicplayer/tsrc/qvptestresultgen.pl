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

#------------------------------------------------------------------------------------
# Includes
#------------------------------------------------------------------------------------
use strict;
use warnings;
use Cwd; # for cwd
use FindBin; # for FindBin:Bin
use File::Path; # for mkpath
use Date::Calc;

{ # No globals.
    my $cgidir = GetCgiDir();
    #require( $cgidir . "utils.pl" );
}

#------------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------------
my $lineFeed = "\n";

my $RESULT_TESTCLASS =          0;      # string
my $RESULT_TESTCASE =           1;      # string
my $RESULT_PASSEDCOUNT =        2;      # num
my $RESULT_FAILEDCOUNT =        3;      # num
my $RESULT_SKIPPEDCOUNT =       4;      # num
my $RESULT_TOTALCOUNT =         5;      # num
my $RESULT_WARNINGMSGS =        6;      # array of strings
my $RESULT_DEBUGMSGS =          7;      # array of strings
my $RESULT_DEPRECATIONMSGS =    8;      # array of strings
my $RESULT_ALLCASES =           9;     # array of arrays ( result(fail=0, pass=1), string method name, array of info strings )
    my $CASE_RESULT = 0;
    my $CASE_NAME = 1;
    my $CASE_INFO = 2;
my $RESULT_SKIPPEDCASES =       10;     # array of strings

my $optionTextReport = 0;

#------------------------------------------------------------------------------------
# Main code
#------------------------------------------------------------------------------------

ReadArgs();

Main();

exit();

#------------------------------------------------------------------------------------
# Subs
#------------------------------------------------------------------------------------

sub ReadArgs
{
    while( scalar(@ARGV) > 0 )
    {
    	my $argument = shift(@ARGV);
    	
    	if( $argument eq "-h" )
    	{
    		ShowHelp();
    	}
    	elsif( $argument eq "-txt" )
    	{
            $optionTextReport = 1;
    	}
    	elsif( $argument ne "" )
    	{
    		print "Uknown argument: $argument\n";
    		ShowHelp();
    	}
    };
}

sub ShowHelp 
{

print <<USAGE_EOF;
qvptestresultgen.pl <options: -h -txt>
  
Options
  -h                    Shows help.
  -txt                  Creates test report in text format instead of xml (Excel).

Copy the Qt test result files from device c:\\data to same folder with 
the script and run it. Creates report from the results.

Script can be run from videoplayer/internal/qt folder.

USAGE_EOF

	exit();

}

sub Main
{
    my @files;
    FindFiles(".", "txt", 1, \@files);

    my @allResults;

    foreach my $file ( @files )
    {
        my @results;

        if( ReadTestRunResults( $file, \@results ) )
        {
            push @allResults, [ @results ];
        }
    }
    
    my @totals;
    GetTotals( \@totals, \@allResults );
    
    if( $optionTextReport )
    {
        CreateTextReport( \@totals, \@allResults );
    }
    else
    {
        CreateXmlReport( \@totals, \@allResults );
    }
}

sub ReadTestRunResults
{
    my ( $file, $refResults ) = @_;
    
    open( FILE_HANDLE, $file ) or die("ERROR! Could not open file '" . $file . "'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	
    my $isQtResultFile = 0;
    for( my $i = 0; $i < 10; $i++ )
    {
        if( $i < scalar( @lines ) && $lines[$i] =~ m/Using QTest library/i )
        {
            $isQtResultFile = 1;
            last;
        }
    }
    return 0 if( !$isQtResultFile );    
	
	@$refResults[$RESULT_TESTCLASS] = "";
	@$refResults[$RESULT_TESTCASE] = "";
    @$refResults[$RESULT_PASSEDCOUNT] = 0;
    @$refResults[$RESULT_FAILEDCOUNT] = 0;
    @$refResults[$RESULT_SKIPPEDCOUNT] = 0;
    @$refResults[$RESULT_TOTALCOUNT] = 0;
    @$refResults[$RESULT_WARNINGMSGS] = [ () ];
    @$refResults[$RESULT_DEBUGMSGS] = [ () ];
    @$refResults[$RESULT_DEPRECATIONMSGS] = [ () ];
    @$refResults[$RESULT_ALLCASES] = [ () ];
    @$refResults[$RESULT_SKIPPEDCASES] = [ () ];
	
	for( my $i = 0; $i < scalar(@lines); $i++ )
	{
	    my $line = $lines[$i];
	    
	    #              msg                 class              function
	    if( $line =~ /([a-z\!]+)[ ]+\:[ ]+([a-z0-9\_\-]+)\:\:([a-z0-9\_\-]+\(\))/i )
	    {
	        my $result = $1;
	        my $testClass = $2;
	        my $testFunc = $3;
	        #print("result: $result, class: $testClass, func: $testFunc\n");
	        
	        @$refResults[$RESULT_TESTCLASS] = $testClass;
	        @$refResults[$RESULT_TESTCASE] = $testFunc;
	        
	        my @case;
	        $case[$CASE_RESULT] = -1;
	        $case[$CASE_NAME] = $testClass . "::" . $testFunc;;
	        $case[$CASE_INFO] = [ () ];
	        
	        if( $result =~ /pass/i )
	        {
	            @$refResults[$RESULT_PASSEDCOUNT]++;
	            $case[$CASE_RESULT] = 1;
	        }
	        if( $result =~ /fail\!/i )
	        {
	            @$refResults[$RESULT_FAILEDCOUNT]++;
	            
                $case[$CASE_RESULT] = 0;
                
	            my @info;
	            # Read rest of the line for info.
	            my $reason = substr( $line, rindex( $line, $testFunc ) + length( $testFunc ) );
	            RemoveWhiteSpaces( \$reason );
	            push @info, $reason;

                # Read next lines starting with space to info.
                while( $lines[$i+1] =~ m/^ / )
                {
                    my $str = $lines[$i+1];
                    RemoveWhiteSpaces( \$str );
                    push @info, $str;
                    $i++;                    
                    last if(!defined($lines[$i+1]));
                }

	            $case[$CASE_INFO] = [ @info ];
	        }
	        
	        my $refAllCases = @$refResults[$RESULT_ALLCASES];
	        push @$refAllCases, [ @case ];
	        
	        if( $result =~ /skip/i )
	        {
	            @$refResults[$RESULT_SKIPPEDCOUNT]++;
	            push @{@$refResults[$RESULT_SKIPPEDCASES]}, $testClass . "::" . $testFunc;
	        }
	        if( $result =~ /qdebug/i )
	        {
	            push @{@$refResults[$RESULT_DEBUGMSGS]}, $line;
	            if( $line =~ m/deprecated/i )
	            {
    	            push @{@$refResults[$RESULT_DEPRECATIONMSGS]}, $line;
	            }
	        }
	        if( $result =~ /qwarn/i )
	        {
	            push @{@$refResults[$RESULT_WARNINGMSGS]}, $line;
	        }	        
	    }
	}
	
	@$refResults[$RESULT_TOTALCOUNT] = @$refResults[$RESULT_PASSEDCOUNT] + @$refResults[$RESULT_FAILEDCOUNT] + @$refResults[$RESULT_SKIPPEDCOUNT];
	return 1;
}

sub GetTotals
{
    my ( $refTotals, $refAllResults ) = @_;
	
	@$refTotals[$RESULT_TESTCLASS] = "";
	@$refTotals[$RESULT_TESTCASE] = "";
    @$refTotals[$RESULT_PASSEDCOUNT] = 0;
    @$refTotals[$RESULT_FAILEDCOUNT] = 0;
    @$refTotals[$RESULT_SKIPPEDCOUNT] = 0;
    @$refTotals[$RESULT_TOTALCOUNT] = 0;
    @$refTotals[$RESULT_WARNINGMSGS] = [ () ];
    @$refTotals[$RESULT_DEBUGMSGS] = [ () ];
    @$refTotals[$RESULT_DEPRECATIONMSGS] = [ () ];
    @$refTotals[$RESULT_ALLCASES] = [ () ];
    @$refTotals[$RESULT_SKIPPEDCASES] = [ () ];
    
    foreach my $refResults ( @$refAllResults )
    {   
    	@$refTotals[$RESULT_TESTCLASS] = "";
    	@$refTotals[$RESULT_TESTCASE] = "";
        @$refTotals[$RESULT_PASSEDCOUNT] += @$refResults[$RESULT_PASSEDCOUNT];
        @$refTotals[$RESULT_FAILEDCOUNT] += @$refResults[$RESULT_FAILEDCOUNT];
        @$refTotals[$RESULT_SKIPPEDCOUNT] += @$refResults[$RESULT_SKIPPEDCOUNT];
        @$refTotals[$RESULT_TOTALCOUNT] += @$refResults[$RESULT_TOTALCOUNT];
        @$refTotals[$RESULT_WARNINGMSGS] = [ ( @{@$refTotals[$RESULT_WARNINGMSGS]}, @{@$refResults[$RESULT_WARNINGMSGS]} ) ];
        @$refTotals[$RESULT_DEBUGMSGS] = [ ( @{@$refTotals[$RESULT_DEBUGMSGS]}, @{@$refResults[$RESULT_DEBUGMSGS]} ) ];
        @$refTotals[$RESULT_DEPRECATIONMSGS] = [ ( @{@$refTotals[$RESULT_DEPRECATIONMSGS]}, @{@$refResults[$RESULT_DEPRECATIONMSGS]} ) ];
        @$refTotals[$RESULT_ALLCASES] = [ ( @{@$refTotals[$RESULT_ALLCASES]}, @{@$refResults[$RESULT_ALLCASES]} ) ];
        @$refTotals[$RESULT_SKIPPEDCASES] = [ ( @{@$refTotals[$RESULT_SKIPPEDCASES]}, @{@$refResults[$RESULT_SKIPPEDCASES]} ) ];
    }
}


sub CreateTextReport
{
    my ( $refTotals, $refAllResults ) = @_;

    my $template = 
"
Test Results for qvideoplayer

HW:
SW:

Test Execution Metrics
Passed: TEMPLATE_PASSED
Failed: TEMPLATE_FAILED
N/A (skipped): TEMPLATE_SKIPPED
Total number of cases: TEMPLATE_TOTAL
Pass rate of total: TEMPLATE_PRT
Pass rate of planned: TEMPLATE_PRP

Tests
TEMPLATE_TESTLIST

Warnings: 
TEMPLATE_WARNINGS
Debug message count from tests: TEMPLATE_DEBUG_COUNT
Deprecation message count from tests: TEMPLATE_DEPRECATION_COUNT

Failed test cases:
TEMPLATE_FAILED_CASES

Passed test cases
TEMPLATE_PASSED_CASES

";
    
    # List of tests.

    my $str = "";
    
    my @testResults;
    GetTestResults(\@testResults, $refAllResults);
    foreach my $refRes( @testResults )
    {
        my $testName =  @$refRes[0];
        my $testResult =  @$refRes[1];
        RemoveWhiteSpaces( \$testName );
        RemoveWhiteSpaces( \$testResult );
        $str .= " $testName $testResult\n" if( $testName ne "" );        
    }
    
    $template =~ s/TEMPLATE_TESTLIST/$str/;

    # Numeric data.
    
    $template =~ s/TEMPLATE_PASSED/@$refTotals[$RESULT_PASSEDCOUNT]/;
    $template =~ s/TEMPLATE_FAILED/@$refTotals[$RESULT_FAILEDCOUNT]/;
    $template =~ s/TEMPLATE_SKIPPED/@$refTotals[$RESULT_SKIPPEDCOUNT]/;
    $template =~ s/TEMPLATE_TOTAL/@$refTotals[$RESULT_TOTALCOUNT]/;
    
    # Pass rates.
    
    my $prt = @$refTotals[$RESULT_PASSEDCOUNT] / @$refTotals[$RESULT_TOTALCOUNT] * 100;
    $prt = sprintf( "%.2f", $prt );
    $template =~ s/TEMPLATE_PRT/$prt/;
    
    my $prp =  @$refTotals[$RESULT_PASSEDCOUNT] / ( @$refTotals[$RESULT_PASSEDCOUNT] + @$refTotals[$RESULT_FAILEDCOUNT] ) * 100;
    $prp = sprintf( "%.2f", $prp );
    $template =~ s/TEMPLATE_PRP/$prp/;
    
    # Message counts.
    
    $str = "";
    foreach my $warning ( @{@$refTotals[$RESULT_WARNINGMSGS]} )
    {
        RemoveWhiteSpaces( \$warning );
        $str .= " $warning\n";
    }
    $template =~ s/TEMPLATE_WARNINGS/$str/;
    
    my $count = scalar( @{@$refTotals[$RESULT_DEBUGMSGS]} );
    $template =~ s/TEMPLATE_DEBUG_COUNT/$count/;
    
    $count = scalar( @{@$refTotals[$RESULT_DEPRECATIONMSGS]} );
    $template =~ s/TEMPLATE_DEPRECATION_COUNT/$count/;

    # Failed cases.

    $str = "\n";
    foreach my $refCase ( @{@$refTotals[$RESULT_ALLCASES]} )
    {
        if( @$refCase[$CASE_RESULT] == 0 )
        {
            my $case = @$refCase[$CASE_NAME];
            my $refInfoArray = @$refCase[$CASE_INFO];
            if( $case ne "" )
            {
                $str .= " $case \n";
                $str .= "    Info: \n";
                foreach my $infoLine ( @$refInfoArray )
                {                
                    $str .= "     $infoLine \n";
                }
            }
            $str .= "\n";
        }
    }
    $str = substr( $str, 0, length( $str ) - 1 ) if( length( $str ) > 1 ); # Remove line break from last line.
    $template =~ s/TEMPLATE_FAILED_CASES/$str/;

    # Passed cases.

    $str = "\n";
    foreach my $refCase ( @{@$refTotals[$RESULT_ALLCASES]} )
    {
        if( @$refCase[$CASE_RESULT] == 1) 
        {
            my $case = @$refCase[$CASE_NAME];
            $str .= " $case \n";
        }
    }

    $str = substr( $str, 0, length( $str ) - 1 ) if( length( $str ) > 1 ); # Remove line break from last line.
    $template =~ s/TEMPLATE_PASSED_CASES/$str/;
    
    # Print to default output and file.
    print( $template);
    
	open(FILE_HANDLE, ">" . "qvptestreport.txt") or die("ERROR! Could not open file '" . "qvptestreport.txt" . "'\n");
    print FILE_HANDLE $template;
    close FILE_HANDLE;
}

sub CreateXmlReport
{
    my ( $refTotals, $refAllResults ) = @_;

    my $data = "";
    my $str = "";
    
    my $rows = @$refTotals[$RESULT_TOTALCOUNT] + scalar( @{@$refTotals[$RESULT_WARNINGMSGS]} ) + scalar( @{@$refTotals[$RESULT_DEBUGMSGS]} ) +
               scalar( @{@$refTotals[$RESULT_DEPRECATIONMSGS]} );
    
    # Excel xml header
    
    $data = XmlHeader( $rows*2 + 100 );
    
    $data .= XmlEmptyRow();

    # HW and SW

    $str = XmlSwHwRow();
    $str =~ s/REPLACE/INSERT SW HERE/;
    $data .= $str;
    
    $str = XmlSwHwRow();
    $str =~ s/REPLACE/INSERT HW HERE/;
    $data .= $str;
                
    $data .= XmlEmptyRow();
    
    # Metrics

    my $passCount = @$refTotals[$RESULT_PASSEDCOUNT];
    my $failCount = @$refTotals[$RESULT_FAILEDCOUNT];
    my $skipCount = @$refTotals[$RESULT_SKIPPEDCOUNT];
    my $totalCount = @$refTotals[$RESULT_TOTALCOUNT];
        
    my $passRateTotal = $passCount / $totalCount * 100;
    $passRateTotal = sprintf( "%.2f", $passRateTotal );
    
    my $passRateRun =  $passCount / ( $passCount + $failCount ) * 100;
    $passRateRun = sprintf( "%.2f", $passRateRun );
    
    my $runRate = ( $passCount + $failCount ) / $totalCount * 100;
    $runRate = sprintf( "%.2f", $runRate );
    
    $str = XmlMetrics();
    $str =~ s/TOTALCASECOUNT/$totalCount/;
    $str =~ s/PASSEDCASECOUNT/$passCount/;
    $str =~ s/FAILEDCASECOUNT/$failCount/;
    $str =~ s/SKIPPEDCASECOUNT/$skipCount/;
    $str =~ s/PASSRATEOFTOTAL/$passRateTotal/;
    $str =~ s/PASSRATEOFRUN/$passRateRun/;
    $str =~ s/RUNRATE/$runRate/;
    $data .= $str;

    $data .= XmlEmptyRow();
        
    # List of tests.

    $str = XmlTitleOnBlueRow();
    $str =~ s/THETITLE/Tests/;
    $data .= $str;

    my @testResults;
    GetTestResults(\@testResults, $refAllResults);
    foreach my $refRes( @testResults )
    {
        $str = XmlTestNameRow();
        my $testName =  @$refRes[0];
        my $testResult =  @$refRes[1];
        XmlReadyText( \$testName );
        XmlReadyText( \$testResult );
        RemoveWhiteSpaces( \$testName );
        RemoveWhiteSpaces( \$testResult );
        $str =~ s/TESTNAME/$testName/;
        $str =~ s/TESTRESULT/$testResult/;
        $data .= $str;
    }
    
    $data .= XmlEmptyRow();
    
    # Case list.

    $str = XmlTitleOnBlueRow();
    $str =~ s/THETITLE/Cases/;
    $data .= $str;

    my $cleanupTestCaseFound = 0;
    my $caseName = "";
    my $caseTestClass = "";

    foreach my $refCase ( @{@$refTotals[$RESULT_ALLCASES]} )
    {
        $caseName = @$refCase[$CASE_NAME];
        
        # Write warning if cleanupTestCase is not found for the test.
        if( index($caseName, "::") != -1 )
        {
            my $tc = substr($caseName, 0, index($caseName, "::"));
            if($caseTestClass ne "" && $caseTestClass ne $tc)
            {
                if(!$cleanupTestCaseFound) 
                {
                    $str = XmlTextRowRed();
                    $str =~ s/REPLACE/cleanupTestCase was not found! Test did not finish?/;
                    $data .= $str;            
                }
                $cleanupTestCaseFound = 0;
            }
            $caseTestClass = $tc;
        }
        $cleanupTestCaseFound = 1 if($caseName =~ "cleanupTestCase");
        
        next if( $caseName eq "" );

        if( @$refCase[$CASE_RESULT] == 0 )
        {
            my $infoStr = "";
            my $refInfoArray = @$refCase[$CASE_INFO];
            foreach my $infoLine ( @$refInfoArray )
            {                
                $infoStr .= "$infoLine\n";
            }
            
            $str = XmlFailedCaseRow();
            XmlReadyText( \$caseName );
            $str =~ s/TESTCASENAME/$caseName/;            
            XmlReadyText( \$infoStr );
            $str =~ s/FAILREASON/$infoStr/;
            $data .= $str;
        }
        elsif( @$refCase[$CASE_RESULT] == 1 )
        {
            $str = XmlPassedCaseRow();
            XmlReadyText( \$caseName );
            $str =~ s/TESTCASENAME/$caseName/;            
            $data .= $str;
        }
    }    
    
    $data .= XmlEmptyRow();
  
    # Warning messages
    
    $str = XmlTitleOnBlueRow();
    $str =~ s/THETITLE/Warning messages/;
    $data .= $str;
    
    foreach my $warning ( @{@$refTotals[$RESULT_WARNINGMSGS]} )
    {
        RemoveWhiteSpaces( \$warning );
        XmlReadyText( \$warning );
        $str = XmlMessageRow();        
        $str =~ s/MESSAGE/$warning/;
        $data .= $str;        
    }
    
    $data .= XmlEmptyRow();
    
    # Message counts.

    my $msgCountStr = "Debug message count: " . scalar( @{@$refTotals[$RESULT_DEBUGMSGS]} );
    $str = XmlMessageRow();
    $str =~ s/MESSAGE/$msgCountStr/;
    $data .= $str;

    $msgCountStr = "Deprecation message count: " . scalar( @{@$refTotals[$RESULT_DEPRECATIONMSGS]} );
    $str = XmlMessageRow();
    $str =~ s/MESSAGE/$msgCountStr/;
    $data .= $str;
    
    for( my $i = 0; $i < 20; $i++ ) 
    {
        $data .= XmlEmptyRow();
    }
    
    # Excel xml footer.
    $data .= XmlFooter();

    open(FILE_HANDLE, ">" . "qvptestreport.xml") or die("ERROR! Could not open file '" . "qvptestreport.xml" . "'\n");
    print FILE_HANDLE $data;
    close FILE_HANDLE;
}

sub GetTestResults
{
    my ($refResults, $refAllResults) = @_;

    # Read tests and results from file.
    open(TRFH, "qvptestresults.txt") or return "";
    my @lines = <TRFH>;
    close(TRFH);
    
    foreach my $line( @lines )
    {
        my @res = split(/\t/, $line);
        RemoveWhiteSpaces(\$res[0]);
        RemoveWhiteSpaces(\$res[1]);
        
        # Check if there's already result for the test.
        my $found = 0;
        foreach my $refRes ( @$refResults ) 
        {
            if(@$refRes[0] eq $res[0] )
            {
                # Update test result.
                @$refRes[1] .= " " . $res[1];
                $found = 1;
                last;
            }
        }
        
        # Test not found, add new.
        if( !$found )
        {
            push @$refResults, [ @res ];
        }
    }

    # And add those tests that were not in the result file.

    foreach my $refResult ( @$refAllResults )
    {
        my $testName = @$refResult[$RESULT_TESTCLASS];

        my $found = 0;
        foreach my $refRes ( @$refResults ) 
        {
            if(@$refRes[0] eq $testName )
            {
                $found = 1;
                last;
            }
        }
        
        if(!$found)
        {
            push @$refResults, [ ($testName, "" ) ];
        }
    }        
}

sub FindFiles
{
	my ($godir, $fileSearch, $searchType, $refIncfiles) = @_;

	my $startDir = cwd;

	chdir($godir) or return;

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		 	FindFiles( $file, $fileSearch, $searchType, $refIncfiles);
		} else
		{
			if( ($file =~ m/$fileSearch/i and $searchType == 0 ) or ($file =~ m/$fileSearch$/i and $searchType == 1 ) )
			{
                $file = cwd . "/" . $file;
				push @$refIncfiles, $file;
				#print("$file\n");
			}
		}
	}

	chdir ($startDir);
}


sub GetCgiDir
  {
    # Solve where the script is located.
    $0=~/^(.+[\\\/])[^\\\/]+[\\\/]*$/;
    my $cgidir= $1 || "./";
    
    # And try to solve relative path.
    if( index( $cgidir, "..\\" ) != -1 )
    {
    	my $p = cwd;
    	$p =~ s/\//\\/g;
    	$cgidir =~ s/\//\\/g;
    	while(1) 
    	{
    		my $pos = index( $cgidir, "..\\" );
    		last if( $pos == -1 );
    		$cgidir = substr( $cgidir, $pos+3 );
    		
    		$pos = rindex( $p, "\\" );
    		last if( $pos == -1 );
    		$p = substr( $p, 0, $pos);
    	}
    	$cgidir = $p . "\\" . $cgidir;
    }
    return $cgidir;
}

sub GetPathFileName
{
    my ($str) = @_;

    my $startPos = rindex($str, "\\");
    if($startPos == -1)
    {
        $startPos = rindex($str, "/");
        return $str if($startPos == -1);
    }

    my $filename = substr($str, $startPos+1);

    return $filename;
}

sub GetPathDir
{
    my ($str) = @_;

    my $endPos = rindex($str, "\\");
    if($endPos == -1)
    {
        $endPos = rindex($str, "/");
        return $str if($endPos == -1);
    }

    my $filename = substr($str, 0, $endPos);

    return $filename;
}

sub RemoveWhiteSpaces
{
	my ($text) = @_;
	${$text} =~ s/\s+$//; #whitespaces in the end
	${$text} =~ s/^\s+//; #whitespaces at the start
}

sub XmlReadyText
{
	my ($text) = @_;

	my $txtlt = "&lt;";
	my $txtgt = "&gt;";
	${$text} =~ s/</$txtlt/g;
	${$text} =~ s/>/$txtgt/g;

	${$text} =~ s/Ä/&Auml/g;
	${$text} =~ s/ä/&auml/g;
	${$text} =~ s/Ö/&Ouml/g;
	${$text} =~ s/ö/&ouml/g;

	#ä &auml;

#	Ö &Ouml;

#	ö &ouml;
}

sub XmlHeader 
{ 
    my ( $rowCount ) = @_;
    my $str =
"<?xml version=\"1.0\"?>\n" .
"<?mso-application progid=\"Excel.Sheet\"?>\n" .
"<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n" .
" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n" .
" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n" .
" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n" .
" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n" .
" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n" .
"  <Author>Senbom Petri</Author>\n" .
"  <LastAuthor>Senbom Petri</LastAuthor>\n" .
"  <Created>2006-06-09T11:08:05Z</Created>\n" .
"  <Company>Nokia Oyj</Company>\n" .
"  <Version>11.9999</Version>\n" .
" </DocumentProperties>\n" .
" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n" .
"  <WindowHeight>12210</WindowHeight>\n" .
"  <WindowWidth>18780</WindowWidth>\n" .
"  <WindowTopX>360</WindowTopX>\n" .
"  <WindowTopY>255</WindowTopY>\n" .
"  <ProtectStructure>False</ProtectStructure>\n" .
"  <ProtectWindows>False</ProtectWindows>\n" .
" </ExcelWorkbook>\n" .
" <Styles>\n" .
"  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n" .
"   <Alignment ss:Vertical=\"Bottom\"/>\n" .
"   <Borders/>\n" .
"   <Font/>\n" .
"   <Interior/>\n" .
"   <NumberFormat/>\n" .
"   <Protection/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153896384\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153896394\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Top\" ss:WrapText=\"1\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"   <NumberFormat/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153895896\">\n" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#FFFF00\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"redbg\">\n" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#FF0000\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153895906\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Center\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153881708\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153881728\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#99CCFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153881376\">\n" .
"   <Alignment ss:Horizontal=\"Right\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Interior ss:Color=\"#99CCFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153881436\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#99CCFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"m153895744\">\n" .
"   <Alignment ss:Horizontal=\"Right\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" .
"   </Borders>\n" .
"   <Interior ss:Color=\"#99CCFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s24\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s27\">\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s32\">\n" .
"   <Alignment ss:Vertical=\"Top\" ss:WrapText=\"1\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s34\">\n" .
"   <Alignment ss:Horizontal=\"Right\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"<Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" .
"   </Borders>\n" .
"   <Interior ss:Color=\"#99CCFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s35\">\n" .
"   <Alignment ss:Vertical=\"Top\" ss:WrapText=\"1\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"   <NumberFormat ss:Format=\"Fixed\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s48\">\n" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#CCFFCC\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s109\">\n" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Bottom\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"REDTEXT\">\n" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Bottom\"/>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#FF0000\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s111\">\n" .
"   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Bottom\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s119\">\n" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Bottom\"/>\n" .
"   <Borders>\n" .
"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n" .
"   </Borders>\n" .
"   <Font x:Family=\"Swiss\" ss:Bold=\"1\"/>\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
"  <Style ss:ID=\"s82\">\n" .
"   <Interior ss:Color=\"#FFFFFF\" ss:Pattern=\"Solid\"/>\n" .
"  </Style>\n" .
" </Styles> \n" .
" <Worksheet ss:Name=\"Sheet1\">\n" .
"  <Table ss:ExpandedColumnCount=\"15\" ss:ExpandedRowCount=\"EXCELEXPANDEDROWCOUNT\" x:FullColumns=\"1\"\n" .
"   x:FullRows=\"1\">\n" .
"   <Column ss:StyleID=\"s82\" ss:AutoFitWidth=\"0\" ss:Width=\"12.75\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"63\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"69\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"78\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"197.25\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"388.5\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"39\" ss:Span=\"1\"/>\n" .
"   <Column ss:Index=\"11\" ss:AutoFitWidth=\"0\" ss:Width=\"39.75\"/>\n" .
"   <Column ss:Index=\"13\" ss:AutoFitWidth=\"0\" ss:Width=\"26.25\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"15\"/>\n" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"525.75\"/>\n";

    $str =~ s/EXCELEXPANDEDROWCOUNT/$rowCount/;
    return $str;
};

sub XmlFooter 
{
    return 
"  </Table>\n" .
"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n" .
"   <Print>\n" .
"    <ValidPrinterInfo/>\n" .
"    <PaperSizeIndex>9</PaperSizeIndex>\n" .
"    <HorizontalResolution>600</HorizontalResolution>\n" .
"    <VerticalResolution>600</VerticalResolution>\n" .
"   </Print>\n" .
"   <Selected/>\n" .
"   <Panes>\n" .
"    <Pane>\n" .
"     <Number>3</Number>\n" .
"     <ActiveRow>1</ActiveRow>\n" .
"     <RangeSelection>R2C1:R2C14</RangeSelection>\n" .
"    </Pane>\n" .
"   </Panes>\n" .
"   <ProtectObjects>False</ProtectObjects>\n" .
"   <ProtectScenarios>False</ProtectScenarios>\n" .
"  </WorksheetOptions>\n" .
" </Worksheet>\n" .
"</Workbook>\n";
}

sub XmlEmptyRow 
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"13\" ss:StyleID=\"s109\"><Data ss:Type=\"String\"></Data></Cell>\n" .
"   </Row>\n";
}

sub XmlSwHwRow
{
    return 
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"13\" ss:StyleID=\"s111\"><Data ss:Type=\"String\">REPLACE</Data></Cell>\n" .
"   </Row>\n";
}

sub XmlMetrics
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"2\" ss:StyleID=\"s24\"><Data ss:Type=\"String\">Test Execution Metrics</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"m153895744\"><Data ss:Type=\"String\">Total cases</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s32\"><Data ss:Type=\"Number\">TOTALCASECOUNT</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"s34\"><Data ss:Type=\"String\">Passed</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s32\"><Data ss:Type=\"Number\">PASSEDCASECOUNT</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"s34\"><Data ss:Type=\"String\">Failed</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s32\"><Data ss:Type=\"Number\">FAILEDCASECOUNT</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"s34\"><Data ss:Type=\"String\">Skipped</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s32\"><Data ss:Type=\"Number\">SKIPPEDCASECOUNT</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"s34\"><Data ss:Type=\"String\">Pass rate % (total cases)</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s35\"><Data ss:Type=\"Number\">PASSRATEOFTOTAL</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"s34\"><Data ss:Type=\"String\">Pass rate % (run cases)</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s35\"><Data ss:Type=\"Number\">PASSRATEOFRUN</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"1\" ss:StyleID=\"m153881376\"><Data ss:Type=\"String\">Run rate %</Data></Cell>\n" .
"    <Cell ss:StyleID=\"s35\"><Data ss:Type=\"Number\">RUNRATE</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"10\" ss:StyleID=\"s27\"/>\n" .
"   </Row>\n";
}

sub XmlTitleOnBlueRow 
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"4\" ss:StyleID=\"m153881436\"><Data ss:Type=\"String\">THETITLE</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n";
}

sub XmlTestNameRow
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"2\" ss:StyleID=\"m153896384\"><Data ss:Type=\"String\">TESTNAME</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"1\" ss:StyleID=\"m153881708\"><Data ss:Type=\"String\">TESTRESULT</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n";
}

sub XmlMessageRow
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"4\" ss:StyleID=\"m153896384\"><Data ss:Type=\"String\">MESSAGE</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n";
}

sub XmlPassedCaseRow
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:StyleID=\"s48\"><Data ss:Type=\"String\">PASSED</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"3\" ss:StyleID=\"m153881708\"><Data ss:Type=\"String\">TESTCASENAME</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n";
}

sub XmlFailedCaseRow 
{
    return 
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeDown=\"1\" ss:StyleID=\"m153895896\"><Data ss:Type=\"String\">FAILED</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"2\" ss:MergeDown=\"1\" ss:StyleID=\"m153895906\"><Data\n" .
"      ss:Type=\"String\">TESTCASENAME</Data></Cell>\n" .
"    <Cell ss:MergeDown=\"1\" ss:StyleID=\"m153896394\"><Data ss:Type=\"String\">FAILREASON</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:Index=\"7\" ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n";
}

sub XmlTextRowRed
{
    return
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:StyleID=\"redbg\"><Data ss:Type=\"String\"></Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"3\" ss:StyleID=\"m153881708\"><Data ss:Type=\"String\">REPLACE</Data></Cell>\n" .
"    <Cell ss:MergeAcross=\"8\" ss:StyleID=\"s119\"/>\n" .
"   </Row>\n";
}

sub XmlTextRowRedx
{
    return 
"   <Row>\n" .
"    <Cell ss:Index=\"2\" ss:MergeAcross=\"8\" ss:StyleID=\"REDTEXT\"><Data ss:Type=\"String\">REPLACE</Data></Cell>\n" .
"   </Row>\n";
}
