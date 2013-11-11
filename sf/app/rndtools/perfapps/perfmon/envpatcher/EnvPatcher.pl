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
# Environment Patcher - Patches older S60 SDKs for supporting
# tricks in newer platforms
#


require v5.6.1;	

use File::Copy;
use strict;

# check amount of commandline options is valid
if (@ARGV != 1)
{
    print "Usage: EnvPatcher <EPOCROOT>\n";
    exit 1;
}


# get epocroot and convert, convert \ -> /
(my $epocroot = $ARGV[0]) =~ s{\\}{/}g;

# remove any trailing forward slashes
$epocroot =~ s/\/$//;


# create variables for paths
my $e32toolsdir = $epocroot."/epoc32/tools";
my $e32includedir = $epocroot."/epoc32/include";
my $e32includeoemdir = $e32includedir."/oem";
my $platformpathspath = $e32includedir."/platform_paths.hrh";
my $domainplatformpathspath = $e32includedir."/domain/osextensions/platform_paths.hrh";
my $mmppmpath = $e32toolsdir."/mmp.pm";
my $pathutlpmpath = $e32toolsdir."/pathutl.pm";
my $prepfilepmpath = $e32toolsdir."/prepfile.pm";

# variables for hacked content
my $dependshack = "\t\t\tif (/^DEPENDS\$/o) {\r\n\t\t\t\tnext LINE;  # Ignore DEPENDS keyword, not needed by ABLD\r\n\t\t\t}\r\n";
my $smpsafehack = "\t\tif (/^SMPSAFE\$/o) {\r\n\t\t\tnext LINE;  # Ignore SMPSAFE keyword, not needed by older environments\r\n\t\t}\r\n";
my $forwardslashhack = "\t\t# EnvPatcher forwardslash hack begins\r\n\t\t\$_=~s{/}{\\\\}g;   # convert all forward slashes to backslashes\r\n\t\t# EnvPatcher forwardslash hack ends\r\n\r\n";
my $coreibyexportsupport = "\r\n// Following definition is used for exporting tools and stubs IBY files to\r\n// Core image.\r\n#define CORE_IBY_EXPORT_PATH(path,exported)  /epoc32/rom/include/##exported\r\n";


# check epoc32\tools exists
unless (-d $e32toolsdir)
{
    print "$e32toolsdir not found, please check valid epocroot has been given!\n";
    exit 1;
}

# check epoc32\include exists
unless (-d $e32includedir)
{
    print "$e32includedir not found, please check valid epocroot has been given!\n";
    exit 1;
}


# create epoc32\include\oem if it does not exist
unless (-d $e32includeoemdir)
{
    mkdir $e32includeoemdir or die;
    print "Missing directory $e32includeoemdir created succesfully.\n";
}


# check if epoc32\include\domain\osextensions\platform_paths.hrh exists
if (-e $domainplatformpathspath)
{
    # show an error if the file does not have any platform macros
    unless (string_exists_in_file($domainplatformpathspath, "OS_LAYER_SYSTEMINCLUDE"))
    {
        print "ERROR: $domainplatformpathspath does not have SF macros.\n";
        print "Please check your environment, if you have S60 3.2 OEM or newer, please get the latest version!\n";
        exit 2;
    }
}


# check if epoc32\include\platform_paths.hrh exists
if (-e $platformpathspath)
{
    print "$platformpathspath already exists, not checking it.\n";    
}
else
{
    # create the file missing file
    create_default_platform_paths_hrh();
    print "Missing file $platformpathspath created succesfully.\n";    
}


# check if CORE_IBY_EXPORT_PATH macro exist in the platform_paths.hrh
unless (string_exists_in_file($platformpathspath, "CORE_IBY_EXPORT_PATH"))
{
    # read content of the platform_paths.hrh
    my @filecontent = read_file_to_array($platformpathspath);  

    my $match_found = 0;
    my $i = 0;
    my $match_found_pos = 0;
    
    # find the position where the include guards start (this should be a safe position)
    foreach (@filecontent)
    {
        if ($_ =~ /#define PLATFORM_PATHS_HRH/)
        {
            $match_found = 1;
            $match_found_pos = $i;
            last;
        } 

        $i++;
    }
    
    if ($match_found)
    {
        # insert the patched content to the file
        splice(@filecontent, $match_found_pos+1, 0, $coreibyexportsupport);
        
        # write the modified array to the file
        write_file_from_array($platformpathspath, @filecontent);
    
        print "Platform_paths.hrh updated to support CORE_IBY_EXPORT_PATH macro.\n";
    }
    else
    {
        print "WARNING: $platformpathspath is corrupted or not supported!\n";    
    }
}
 
    
# check if epoc32\tools\mmp.pm exists
if (-e $mmppmpath)
{
    # check if DEPENDS keyword already exists in the file
    if (string_exists_in_file($mmppmpath, "DEPENDS"))
    {
        print "The SDK can already handle DEPENDS keyword in a MMP file.\n";        
    }
    else
    {
        # read content of the mmp.pm file
        my @filecontent = read_file_to_array($mmppmpath);
        
        my $match_found = 0;
        my $i = 0;
        my $match_found_pos = 0;
        
        # loop through the array to find the correct place
        foreach (@filecontent)
        {
            if ($_ =~ /Unrecognised Resource Keyword/)
            {
                $match_found = 1;
                $match_found_pos = $i;
                last;
            } 

            $i++;
        }
        
        if ($match_found)
        {
            # insert the patched content to the file
            splice(@filecontent, $match_found_pos-1, 0, $dependshack);
            
            # write the modified array to the file
            write_file_from_array($mmppmpath, @filecontent);
        
            print "Mmp.pm patched with DEPENDS keyword hack.\n";
        }
        else
        {
            print "ERROR: Unable to find correct place from $mmppmpath for patching!\n";
            print "Your SDK environment probably is not supported by this script!\n";
            exit(2);    
        }
    }

    # check if SMPSAFE keyword already exists in the file
    if (string_exists_in_file($mmppmpath, "SMPSAFE"))
    {
        print "The SDK can already handle SMPSAFE keyword in a MMP file.\n";        
    }
    else
    {
        # read content of the mmp.pm file
        my @filecontent = read_file_to_array($mmppmpath);
        
        my $match_found = 0;
        my $i = 0;
        my $match_found_pos = 0;
        
        # loop through the array to find the correct place
        foreach (@filecontent)
        {
            if ($_ =~ /Unrecognised Keyword/)
            {
                $match_found = 1;
                $match_found_pos = $i;
                last;
            } 

            $i++;
        }
        
        if ($match_found)
        {
            # insert the patched content to the file
            splice(@filecontent, $match_found_pos, 0, $smpsafehack);
            
            # write the modified array to the file
            write_file_from_array($mmppmpath, @filecontent);
        
            print "Mmp.pm patched with SMPSAFE keyword hack.\n";
        }
        else
        {
            print "ERROR: Unable to find correct place from $mmppmpath for patching!\n";
            print "Your SDK environment probably is not supported by this script!\n";
            exit(2);    
        }
    }
}
else
{
    print "WARNING: $mmppmpath not found, this environment is not supported!\n";
}


# check if epoc32\tools\pathutl.pm exists
if (-e $pathutlpmpath)
{
    # check if "sub Path_Norm" already exists in the pathutil.pm file
    # if it does not exists, then we need to patch prepfile.pm
    if (string_exists_in_file($pathutlpmpath, "sub Path_Norm"))
    {
        print "The SDK is non Symbian OS 9.1, no need to add forward slash hack.\n";        
    }
    else
    {
        # check if prepfile.pm has already been patched
        if (string_exists_in_file($prepfilepmpath, "EnvPatcher forwardslash hack"))
        {        
            print "The SDK has already been patched with forwardslash hack.\n";         
        }
        else
        {    
            # read content of the prepfile.pm file
            my @filecontent = read_file_to_array($prepfilepmpath);  
    
            my $match_found = 0;
            my $i = 0;
            my $match_found_pos = 0;
            
            # loop through the array to find the correct place
            foreach (@filecontent)
            {
                if ($_ =~ /# skip blank lines/)
                {
                    $match_found = 1;
                    $match_found_pos = $i;
                    last;
                } 
    
                $i++;
            }
            
            if ($match_found)
            {
                # insert the patched content to the file
                splice(@filecontent, $match_found_pos+6, 0, $forwardslashhack);
                
                # write the modified array to the file
                write_file_from_array($prepfilepmpath, @filecontent);
            
                print "Prepfile.pm patched with forward slash hack.\n";
            }
            else
            {
                print "ERROR: Unable to find correct place from $prepfilepmpath for patching!\n";
                print "Your SDK environment probably is not supported by this script!\n";
                exit(2);    
            }
        }
    }    
}
else
{
    print "WARNING: $pathutlpmpath not found, this environment is not supported!\n";
}
 
 
 
# checks if string exists in the file    
sub string_exists_in_file
{
    my $filepath = $_[0];
    my $findstring = $_[1];
    my $match_found = 0;     

    open(FILE, "<", $filepath) or die "Failed to open $filepath for reading!";

    # loop through the file for occurances
    while (<FILE>)
    {
        if ($_ =~ /$findstring/)
        {
            $match_found = 1;
            last;
        } 
    }

    close FILE;
    
    return $match_found;
}


# reads lines from a file to an array    
sub read_file_to_array
{
    my $filepath = $_[0];

    open(FILE, "<", $filepath) or die "Failed to open $filepath for reading!";
    my @data = <FILE>;
    close FILE;
    
    return(@data);
}


# writes lines from an array to a file
sub write_file_from_array
{
    my ($filepath, @data) = @_;
    
    # take a backup of the file
    copy ($filepath, $filepath."EnvPatcher") or die "Cannot take backup of $filepath to $filepath.EnvPatcher";
        
    open(FILE, ">", $filepath) or die "Failed to open $filepath for writing!";

    # write the array to file
    foreach my $line (@data)
    {
        print FILE "$line";
    }

    close FILE;
}

sub create_default_platform_paths_hrh
{
    # the file does not exist, so create the missing file
    open(FILE, ">", $platformpathspath) or die "Failed to open $platformpathspath for writing!\n";
    
    print FILE <<ENDOFTHEFILE;
#ifndef PLATFORM_PATHS_HRH
#define PLATFORM_PATHS_HRH

/**
* ---------------------------------------
* Location, where the applications layer specific public headers should be exported
* See usage on top of this hrh-file.
* ---------------------------------------
*/
#define APP_LAYER_SDK_EXPORT_PATH(exported) /epoc32/include/##exported
#define APP_LAYER_PUBLIC_EXPORT_PATH(exported) /epoc32/include/##exported

/**
* ---------------------------------------
* Location, where the applications layer specific platform headers should be exported
* See usage on top of this hrh-file.
* ---------------------------------------
*/
#define APP_LAYER_DOMAIN_EXPORT_PATH(exported) /epoc32/include/##exported
#define APP_LAYER_PLATFORM_EXPORT_PATH(exported) /epoc32/include/##exported

/**
* ---------------------------------------
* Location, where the middleware layer specific public headers should be exported
* See usage on top of this hrh-file.
* ---------------------------------------
*/
#define MW_LAYER_SDK_EXPORT_PATH(exported) /epoc32/include/##exported
#define MW_LAYER_PUBLIC_EXPORT_PATH(exported) /epoc32/include/##exported

/**
* ---------------------------------------
* Location, where the middleware layer specific platform headers should be exported
* ---------------------------------------
*/
#define MW_LAYER_DOMAIN_EXPORT_PATH(exported) /epoc32/include/##exported
#define MW_LAYER_PLATFORM_EXPORT_PATH(exported) /epoc32/include/##exported

/**
* ---------------------------------------
* Location, where the os layer specific public headers should be exported
* ---------------------------------------
*/
#define  OSEXT_LAYER_SDK_EXPORT_PATH(exported) /epoc32/include/##exported
#define  OS_LAYER_PUBLIC_EXPORT_PATH(exported) /epoc32/include/##exported

/**
* ---------------------------------------
* Location, where the os specific platform headers should be exported
* ---------------------------------------
*/
#define OSEXT_LAYER_DOMAIN_EXPORT_PATH(exported) /epoc32/include/##exported
#define OS_LAYER_PLATFORM_EXPORT_PATH(exported) /epoc32/include/##exported

/**
* ---------------------------------------
* Location, where the  cenrep excel sheets should be exported
* Deprecated: should no longer be used. Kept for compability.
* ---------------------------------------
*/
#define CENREP_XLS_EXPORT_PATH(exported) /epoc32/tools/cenrep/data/src/##exported

/**
* This define statements defines the SYSTEMINCLUDE-line, which is intended to be 
* used in the mmp-files that are part of the applications-layer. It includes all 
* the needed directories from the /epoc32/include, that are valid ones for the 
* application-layer components. 
*
* Applications layer is the last one in the list, since most likely the most of 
* the headers come from middleware or os-layer  => thus they are first.
*/
#define APP_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE /epoc32/include /epoc32/include/oem
 
/**
* This define statements defines the SYSTEMINCLUDE-line, which is intended to be
* used in the mmp-files that are part of the middleware-layer. It includes all 
* the needed directories from the /epoc32/include, that are valid ones for the 
* middleware-layer components. 
*/
#define MW_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE /epoc32/include /epoc32/include/oem

/**
* This define statements defines the SYSTEMINCLUDE-line, which is intended to be
* used in the mmp-files that are part of the  osextensions-layer. It includes all
* the needed directories from the /epoc32/include, that are valid ones for the
* os-layer components. 
*/
#define OS_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE /epoc32/include /epoc32/include/oem


// Below statement is Deprecated and the OS_LAYER_SYSTEMINCLUDE-macro has to be
// used.
#define OSEXT_LAYER_SYSTEMINCLUDE OS_LAYER_SYSTEMINCLUDE

/**
* This define statements defines the SYSTEMINCLUDE-line, which is intended to be
* used in the mmp-files that are part of the os-layer. This is intended 
* to be only used by those components which need to use in their mmp-file either
* kern_ext.mmh or nkern_ext.mmh. Reason is that those
* 2 files already contain the /epoc32/include  as system include path.
* 
*/
#define OS_LAYER_KERNEL_SYSTEMINCLUDE SYSTEMINCLUDE /epoc32/include/oem


// Below statement is Deprecated and the OS_LAYER_KERNEL_SYSTEMINCLUDE-macro 
// has to be used.
#define OSEXT_LAYER_KERNEL_SYSTEMINCLUDE OS_LAYER_KERNEL_SYSTEMINCLUDE

/**
****************************************************************************
* Definitions that also define the paths to the layer specific source directories.
****************************************************************************
*/
/**
* The below 3 macros define the paths to the layer-specific source dirs.
* See usage on top of this hrh-file, these are used the same way as 
* for instance the OS_LAYER_DOMAIN_EXPORT_PATH
* Deprecated: is not allowed to be using in Symbian Foundation
*/
#define APP_LAYER_SOURCE_PATH(rest)    /s60/app/##rest
#define MW_LAYER_SOURCE_PATH(rest)     /s60/mw/##rest
#define OSEXT_LAYER_SOURCE_PATH(rest)  /s60/osext/##rest

/**
****************************************************************************
* Definitions to export IBY files to different folders where they will be taken 
* to ROM image
****************************************************************************
*/
// Following definition is used for exporting tools and stubs IBY files to 
// Core image.
#define CORE_IBY_EXPORT_PATH(path,exported)  /epoc32/rom/include/##exported

/**
* ---------------------------------------
* Macros for Configuration tool migration. 
* The below macros define the location under epoc32, where the confml 
* (Configuration Markup Language) and crml (Central Repository Markup Language) 
* files should be exported.
* ---------------------------------------
*/
#define CONFML_EXPORT_PATH(file,category)           /epoc32/rom/config/confml_data/##category##/##file
#define CRML_EXPORT_PATH(file,category)             /epoc32/rom/config/confml_data/##category##/##file
#define GCFML_EXPORT_PATH(file,category)            /epoc32/rom/config/confml_data/##category##/##file
#define CONFML_CONFIG_EXPORT_PATH(file,category)    /epoc32/rom/config/confml_data/##category##/config/##file

#define APP_LAYER_CONFML(exported) 	                CONFML_EXPORT_PATH(exported,s60)
#define APP_LAYER_CRML(exported)                    CRML_EXPORT_PATH(exported,s60)
#define APP_LAYER_GCFML(exported)                   GCFML_EXPORT_PATH(exported,s60)
#define APP_LAYER_CONFML_CONFIG(exported)           CONFML_CONFIG_EXPORT_PATH(exported,s60)
                                                    
#define MW_LAYER_CONFML(exported)                   CONFML_EXPORT_PATH(exported,s60)
#define MW_LAYER_CRML(exported)                     CRML_EXPORT_PATH(exported,s60)
#define MW_LAYER_GCFML(exported)                    GCFML_EXPORT_PATH(exported,s60)
#define MW_LAYER_CONFML_CONFIG(exported)            CONFML_CONFIG_EXPORT_PATH(exported,s60)
       
// Deprecate: Use the OS_LAYER_* macros instead of OSEXT_LAYER_*                                             
#define OSEXT_LAYER_CONFML(exported)                CONFML_EXPORT_PATH(exported,s60)
#define OSEXT_LAYER_CRML(exported)                  CRML_EXPORT_PATH(exported,s60)
#define OSEXT_LAYER_GCFML(exported)                 GCFML_EXPORT_PATH(exported,s60)
#define OSEXT_LAYER_CONFML_CONFIG(exported)         CONFML_CONFIG_EXPORT_PATH(exported,s60)
#define OS_LAYER_CONFML(exported)                   CONFML_EXPORT_PATH(exported,s60)
#define OS_LAYER_CRML(exported)                     CRML_EXPORT_PATH(exported,s60)
#define OS_LAYER_GCFML(exported)                    GCFML_EXPORT_PATH(exported,s60)
#define OS_LAYER_CONFML_CONFIG(exported)            CONFML_CONFIG_EXPORT_PATH(exported,s60)

#endif  // end of PLATFORM_PATHS_HRH

ENDOFTHEFILE

    close FILE;    
}
