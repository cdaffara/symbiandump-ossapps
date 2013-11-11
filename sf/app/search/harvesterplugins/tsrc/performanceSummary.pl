#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#Change Log:
#version 0.2: Analyzes multiple files in current directory.
#version 0.1: Initial functionality.
#
#Provides summary of performance logs which are in the format:
#add nnn - Time for add operation
#del nnn - Time for delete operation
#upd nnn - Time for update opearation
#Ignores all other lines.
#
#Reads the current directory looking for file names 
#with the pattern '*Perf.txt' (case sensitive).
#
#Prints the cumulative times for all ADD/DEL/UPD operations

use warnings;
use strict;

#This prints the summary; called by analzyeFile
sub printSummary($$$){ 
	print "Total time for Add: ".(shift)."\n";
	print "Total time for Delete: ".(shift)."\n";
	print "Total time for Update: ".(shift)."\n";
	print "-------------------------------------\n\n";
}

#This function analyzes the contents of the performance logs file
sub analyzeFile($){
	my $fh = shift;
	my $add=0;
	my $del=0; 
	my $upd=0;
	foreach my $line (<$fh>){
		chomp($line);#remove the newline from $line.
		#Documentation recommends not to use $' for
		#performance reasons. In this case, its okay.
		if ($line =~ m/^add */) {
			$add += $';
		}
		if($line =~ m/^del */) {
			$del += $';
		}
		if($line =~ m/^upd */) {
			$upd += $';
		}
	}
	#better if we can 'return; these values and have the 
	#caller print the values returned.
	#very tight function coupling otherwise.
	printSummary($add, $del, $upd);
}

#Main entry point
sub main(){
	#We can change the function to be able to read
	#cmd line args and analyze the necessary directory
	#but thats for later. Not required for the moment.
	my $directory = ".";
	my $dirHandle;

	opendir($dirHandle, "$directory") or die("Could not list directory $directory.");
	my @thefiles = readdir($dirHandle);

	foreach my $f (@thefiles){
		if ($f =~ m/Perf\.txt$/) {#should end with "Perf.txt"
			open my $inputFile, "<$f" or die("Could not open log file $f.");
			print "File: ".$f."\n";
			print "-----------------------\n";
			analyzeFile($inputFile);
			close($inputFile);
		}
	}
	closedir($dirHandle);
}

#Call main() to do the work.
main();