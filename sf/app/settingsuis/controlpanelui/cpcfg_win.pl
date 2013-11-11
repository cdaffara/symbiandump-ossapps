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
# Description: Copy all cpcfg files to destination directory when building control panel in window envionment
# usage: go to control panel source directory, 
# run cpcfg_win.pl if you want to build control panel in debug mode or cpcfg_win -R in release mode.

use File::Path qw (mkpath);

#default debug dir
$config_dir = "C:\\ControlPanel\\debug\\bin\\config";
if ($ARGV[0] =~ m/^-R$/i) { #release dir
	$config_dir = "C:\\ControlPanel\\release\\bin\\config";
} 

sub go_through_dir {  
 my @arr, $j = 0;  
 for ($i=0;$i<=$#_;$i++) {  
  if (-d $_[$i]) {  
   if (opendir($handle, $_[$i])) {  
    while ($entry = readdir($handle)) {  
     if (!($entry =~ m/^\.$/) and !($entry =~ m/^(\.\.)$/)) {   
      if (-d $_[$i]."\\$entry") {  # is a directory, push to @arr
       $arr[$j++] = $_[$i]."\\$entry";   
      }  
      else {   # is a file
      	if ($entry =~ m/\.cpcfg$/i) { # is a .cpcfg file, copy it
      		$cmd = "copy ";
      		$cmd .= $_[$i]."\\$entry ";
      		$cmd .= $config_dir."\\$entry";
      		print ("$cmd\n");
      		system($cmd);
      	}
      }
     }  
    }  
    closedir($handle);  
   }  
  }  
 }  
 if ($j>0) {  
  go_through_dir (@arr);  
 }  
}

# create target directory if it doesn't exist
print ("Creating direcotry... $config_dir \n");
mkpath $config_dir;

# go through source directories recrusively
go_through_dir ".\\src\\cpapplication",".\\src\\cpplugins";