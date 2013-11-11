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
#! /usr/local/bin/perl -w
#
#  Change history:
#
# 06-Aug-2006 : DLL versioning support added (Elftran tool)
# 02-Aug-2007 : Updated for changed RnD certificate path
#
#
#

sub updateVersion ;
sub signSis ;

my @args = @ARGV;
# signing related variables
my $certPath = "\\s60_RnD\\RD_RootCA";
my $certificate = "rd.cer";
my $key = "rd-key.pem";
my $version = "10.1";
my $packageFile = undef;
my $sisFile = undef;

# path to copy signed sis (if optional -c argument used)
my $copyPath = undef;

# 
my $updateDllVersion = 0;
    
# print help if needed
if ( scalar(@args) == 0 || 
     $args[0] =~ /-[h,\?]/i )
    {
print <<HELP;

-------------------------------------------------------------------------
======= Script for making and signing an installation (.sis) file =======

Usage: sisTool <package-file-w/o-ext> [-d<version>] [-c<destination-path>] 
               [-s<stub-package-file-w/o-ext>]

 -d changes minor DLL version number - needed when eclipsing DLLs in ROM.
 -c copies target file to specific destination
 -s also creates stub SIS. File name is not needed if it same as base file
    name followed by "_stub" (e.g. pkg files for eclipsing SIS and stub SIS
    are component.pkg and component_stub.pkg respectively).

Examples:
=========
sisTool myPackage -d2

  Creates and signs(* 'myPackage.sis' from 'myPackage.pkg'.
  Before creating sis file myPackage.pkg is searched for DLLs
  and their version numbers are changed to 10.2 (i.e. minor
  version is changed - default major version number is 10).

sisTool myPackage -d -s
  
  Same as first example but uses default minor version number (=1)
  for DLLs, and creates stub sis with name myPackage_stub.sis.

                                                     
(* Uses R&D certificate 'rd.cer' from \\S60\\s60_RnD\\RD_RootCA
HELP
    exit;
    }
$packageFile = shift;

foreach ( @args ) {
    # check if dll version number should be updated
    if ( /-d(\d+)?\b/ ) {
        $version = $1 ;
        $version = 1 if ( not defined $version );            
        $updateDllVersion = 1;
        }
    # check if stub SIS should be created
    elsif ( /-s((\w|\.|\\|_|:|-)+)?/ )
        {
        my $stubPkgFile = "${packageFile}_stub.pkg";
        if ( defined $1 ) {
            # user defined stub
            $stubPkgFile = $1; 
            $stubPkgFile = $stubPkgFile . ".pkg" if $stubPkgFile !~ /\.pkg$/; 
            }        
        my $cmd = "makesis -s $stubPkgFile";
        print "\n";
        system "$cmd";
        print "\n";
        }
    $copyPath = $1 if /-c(\S+)/;
    }

if ( defined($packageFile) ) {
    ( -e "$packageFile.pkg" ) or die "package file '$packageFile' not found.\n";
    
    # Update version number of DLLs listed in pkg-file.
    updateVersion if $updateDllVersion;

    my $cmd = "makesis $packageFile\.pkg\n";
   
    if ( !defined( $sisFile ) ) {
        $sisFile = "$packageFile.sis";
        }
    #################
    # Do create sis #
    #################
    
    my $err = system "$cmd";
    die "\nFailed making sis file $!" if $err;
    signSis;
    if ( defined( $copyPath ) )
        {
        system "xcopy $sisFile $copyPath 2>&1";
        }
    }

sub signSis {
    # sign a sis file
    my $signedFile = "${sisFile}S\.sis";
    ( -e "$certPath\\$certificate" ) or die <<NOCERT;
    
    
Certificate could not be found from $certPath\\$certificate
Check and modifiy the path in line ~17 in the script if needed.
NOCERT

    my $cmd = "signsis -s $packageFile.sis $signedFile $certPath\\$certificate $certPath\\$key";        
    if ( system ( "$cmd" ) == 0 )
        {        
        # delete original unsigned sis file
        system "del $sisFile";
        system "ren $signedFile $sisFile";
        print "\nSigned with R&D certificate\n";
        if ( defined ( $copyPath ) ) {
            # copy signed sis to specified directory
            system "xcopy $sisFile $copyPath";
            }
        }        
    }
    

sub updateVersion {
    open PACKAGE, "<$packageFile.pkg";    
    foreach ( <PACKAGE> )
        {
        if ( /\"(.*?\.dll)\"/i ) 
            {                        
            system "elftran -version 10\.$version $1";
            system "elftran -compressionmethod bytepair $1";
            print STDOUT "\nUpdated version number of\n$1 to 10.$version\n";
            }
        }
    close PACKAGE;  
    }    
