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
#!/usr/bin/perl -w
use warnings;
use strict;

my $TOTALCASES 		= 0;
my $TOTALPASSED 	= 0;
my $TOTALRUN 			= 0;
my $TOTALFAILED 	= 0;
my $TOTALSKIPPED 	= 0;

sub parse_line
{
	my $line = $_[0];
	my @parts = split (" ", $line);
	my $passed  = $parts[1];
	my $failed  = $parts[3];
	my $skipped = $parts[5];
	
	my $total = $passed + $failed + $skipped;
	my $passrate = $passed * 100 / $total;
	my $passrateround = sprintf("%.0f",$passrate);
	my $runrate = ($total - $skipped)*100/$total;
	my $runrateround = sprintf("%.0f",$runrate);
	
	$TOTALCASES += $total;
	$TOTALPASSED += $passed;
	$TOTALRUN += ($total - $skipped);
	$TOTALFAILED += $failed;
	$TOTALSKIPPED  += $skipped;
	
	printf MYFILE "<td>\n";
	printf MYFILE "$passed\n";
	printf MYFILE "</td>\n";
	
	if($failed > 0)
	{
	printf MYFILE "<td style=\"font-weight:bold;color:red\">\n";
	printf MYFILE "$failed\n";
	printf MYFILE "</td>\n";
	}
	else
	{
	printf MYFILE "<td>\n";
	printf MYFILE "$failed\n";
	printf MYFILE "</td>\n";	
	}

	printf MYFILE "<td>\n";
	printf MYFILE "$skipped\n";
	printf MYFILE "</td>\n";

	printf MYFILE "<td>\n";
	printf MYFILE "$total\n";
	printf MYFILE "</td>\n";
	 
	printf MYFILE "<td>\n";
	printf MYFILE ("$passrateround %%\n");
	printf MYFILE "</td>\n";

	printf MYFILE "<td>\n";
	printf MYFILE "$runrateround %%\n";
	printf MYFILE "</td>\n";
	
	
}

sub parse_file
	{
		my $pattern = "Totals";
		my $file = $_[0];
		open (FILE,$file);
	
		while (my $line= <FILE>)
		{
			chomp ($line);
		if ($line =~ m/$pattern/)
			{				
			parse_line $line;
			}
		}
		close(FILE);
	}


sub generate_report
	{
		open (MYFILE, '>/epoc32/winscw/c/logs/messagingtestsuite/report.html');
		printf MYFILE "<html>\n"; 
		printf MYFILE "<body>\n";
		printf MYFILE "<head>\n"; 
		printf MYFILE "<title>MsgApp Test Suite Reports</title>"; 
		printf MYFILE "</head>\n";
		printf MYFILE "<h2 align = center>Messaging101 Test Suite Report</h2>\n";
		printf MYFILE "<table border = 2 cellpadding = 10 align = center>\n";
		printf MYFILE "<tr style = \" background-color:lavender \">\n";
		printf MYFILE "<th>Module</th>\n";
		printf MYFILE "<th style = color:green>Passed</th>\n";
		printf MYFILE "<th style = color:red>Failed</th>\n";
		printf MYFILE "<th style = color:chocolate>Skipped</th>\n";
		printf MYFILE "<th>Total Cases</th>\n";
		printf MYFILE "<th>Pass Rate</th>\n";
		printf MYFILE "<th>Run Rate</th>\n";
		printf MYFILE "</tr>\n";
		
		
		my @files = </epoc32/winscw/c/logs/messagingtestsuite/*.txt>;
		foreach my $file (@files) 
		{	
			my @splitedpath = split("/",$file);
			my $filename = $splitedpath[-1];	
			my $length = length($filename);
		  my $name = substr($filename,11,$length-15);
		  printf MYFILE "<tr>\n";
		  printf MYFILE "<td>\n";
		  printf MYFILE "<a HREF=$filename style = text-decoration:none><b>$name</b></a>\n";
		  printf MYFILE "</td>\n";
		  
		  parse_file $file;
		  
		  printf MYFILE "</tr>\n";
		}
		
		printf MYFILE "<tr style= \"font-weight:bold; color:white; background-color:gray\">\n";
		printf MYFILE "<td>\n";
		printf MYFILE "<b>Overall</b>\n";
		printf MYFILE "</td>\n";
		
		
		printf MYFILE "<td>\n";
		printf MYFILE "$TOTALPASSED\n";
		printf MYFILE "</td>\n";
		
		printf MYFILE "<td>\n";
		printf MYFILE "$TOTALFAILED\n";
		printf MYFILE "</td>\n";
		
		printf MYFILE "<td>\n";
		printf MYFILE "$TOTALSKIPPED\n";
		printf MYFILE "</td>\n";
		
		printf MYFILE "<td>\n";
		printf MYFILE "$TOTALCASES\n";
		printf MYFILE "</td>\n";
		
		my $passrate = $TOTALPASSED*100/$TOTALCASES;
		my $passrateround = sprintf("%.0f",$passrate);
		printf MYFILE "<td>\n";
		printf MYFILE "$passrateround%%\n";
		printf MYFILE "</td>\n";
		
		my $runrate = ($TOTALCASES - $TOTALSKIPPED)*100/$TOTALCASES;
		my $runrateround = sprintf("%.0f",$runrate);
		printf MYFILE "<td>\n";
		printf MYFILE "$runrateround%%\n";
		printf MYFILE "</td>\n";
		
		printf MYFILE "</tr>\n";
		
		printf MYFILE "</table>\n";
	  printf MYFILE "</body>\n"; 
		printf MYFILE "</html>\n";
			
		close (MYFILE);
	}


generate_report;