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

import os
import sys
import re


def scanUnitTestDirs( dir ):
	unitTestDirectories = []
	for root, dirs, files in os.walk(dir):
		for dir in dirs:
			m = re.match("unittest_.+", dir)
			if m != None:
				m = re.match("unittest_resources", dir)
				if m == None:
					fullpath = root + "\\" + dir
					unitTestDirectories.append( fullpath )
	return unitTestDirectories

print "Creating all unit test SIS files"
print "  Compiling all unit test binaries..."
os.system( "sbs -c armv5.test reallyclean > NULL" )
os.system( "qmake" )
os.system( "sbs -c armv5_udeb.test -j 4" )

print "  Generating individual SIS files..."
dirs = scanUnitTestDirs( "." )
for dir in dirs:
	print "  Processing unit test directory " + dir
	os.chdir( dir )

### Removed individual compiling for now, using main level 
### PRO template to handle all at once.
#	print "    Cleaning up the project..."
#	os.system( "sbs -c armv5_udeb.test reallyclean > NULL" )
#	print "    Compiling the project..."
#	os.system( "qmake" )
#	os.system( "sbs -c armv5_udeb.test -j 4" )

	print "    Creating the SISX..."
	os.system( "sis.bat > NULL" )
	print "    Directory processed!\n"
	os.chdir( ".." )

print "Generating main SIS..."
print " Remove previous files..."
os.system( "del megapak.sisx > NULL" )
print " Makesis..."
os.system( "makesis megapak.pkg megapak.sis > NULL" )
os.system( "signsis megapak.sis megapak.sisx  %RNDCERTS_PATH%\Nokia_RnDCert_02.der %RNDCERTS_PATH%\Nokia_RnDCert_02.key" )
print " Cleaning up..."
os.system( "del megapak.sis > NULL" )

print "\n\nJob's done!"

