#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Project definition file for TimezoneClient
# 

TEMPLATE = lib
TARGET = timezoneclient
DEFINES += TIMEZONECLIENT_LIBRARY

INCLUDEPATH += ../../inc \
			   ../../data/timezonedata \
			   ../../clockengines/inc \
			   ../../clockengines/clockserver/client/inc \
			   ./traces
			   
DEPENDPATH += . \
			  ../../inc \
			  ./inc \
			  ./src \
			  ../../data/timezonedata \
			  ../../clockengines/inc \
			  ../../clockengines/clockserver/client/inc

symbian: {
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.UID3 = 0x2002DD0F
	INCLUDEPATH += /epoc32/include

	LIBS += -leuser \
			-lbafl \
			-ltimezonelocalization \
			-ltzclient \
			-lclockserverclient \
			-lxqsettingsmanager

	BLD_INF_RULES.prj_exports += \
		"../../data/timezonedata/timezonelocalization.loc	APP_LAYER_LOC_EXPORT_PATH(timezonelocalization.loc)" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/release/wins/udeb/z/private/1020383E/tzdb.dbz" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/release/wins/urel/z/private/1020383E/tzdb.dbz" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/release/winscw/urel/z/private/1020383E/tzdb.dbz" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/release/winscw/udeb/z/private/1020383E/tzdb.dbz" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/release/thumb/udeb/z/private/1020383E/tzdb.dbz" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/release/thumb/urel/z/private/1020383E/tzdb.dbz" \
		"../../data/timezonedata/tzdb.dbz					/epoc32/data/z/private/1020383E/tzdb.dbz"

	# MMP rule
	timezonegroupsrule = \
		"SOURCEPATH ."\
		"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
		"START RESOURCE ../../data/timezonedata/timezonegroups.rss" \
		"HEADER" \
		"TARGETPATH /resource/timezonelocalization" \
		"LANG SC 01 02" \
		"END"
		
	timezonegroupsjerusalemrule = \
		"START RESOURCE ../../data/timezonedata/timezonegroups_jerusalem.rss" \
		"HEADER" \
		"TARGETPATH /resource/timezonelocalization" \
		"LANG SC 01 02" \
		"END" \

	timezonetelavivrule = \
		"START RESOURCE ../../data/timezonedata/timezones_tel_aviv.rss" \
		"HEADER" \
		"TARGETPATH /resource/timezonelocalization" \
		"LANG SC 01 02" \
		"END" \

	timezonesrule = \
		"START RESOURCE ../../data/timezonedata/timezones.rss" \
		"HEADER" \
		"TARGETPATH /resource/timezonelocalization" \
		"LANG SC 01 02" \
		"END"
	
	mccrule = \
		"START RESOURCE ../../data/timezonedata/mcc.rss" \
		"HEADER" \
		"TARGETPATH /resource/mcc" \
		"END"

	MMP_RULES += timezonegroupsrule \
				 timezonegroupsjerusalemrule \
				 timezonetelavivrule \
				 timezonesrule \
				 mccrule
}

SOURCES += timezoneclient.cpp \
		   debug.cpp \
		   environmentchangenotifier.cpp
HEADERS += timezoneclient.h \
		   debug.h \
		   environmentchangenotifier.h

# End of file	--Don't remove this.
