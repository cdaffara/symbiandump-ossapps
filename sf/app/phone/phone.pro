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
# Description: Project file for building Phone components 
#
#

TEMPLATE = subdirs
CONFIG += ordered

symbian: {
:BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"group/bld.inf\""
SUBDIRS += phoneengine/phonecntfinder2
SUBDIRS += phoneengine/phoneservices
SUBDIRS += phoneengine/phonemodel
SUBDIRS += phoneengine/networkhandlingstarter
SUBDIRS += phoneuis/bubblemanager2
SUBDIRS += phoneuis/ussdeditor
SUBDIRS += phoneplugins/phoneindicatorplugin
SUBDIRS += phoneapp/phoneuiview2
SUBDIRS += phoneapp/phoneuiqtviewadapter
SUBDIRS += phoneapp/phonestringloader
SUBDIRS += phoneapp/phoneui2
SUBDIRS += phonesettings/cpphonesettingsplugins
SUBDIRS += phoneplugins/dialerwidgetplugin
SUBDIRS += phoneplugins/infowidgetplugin
}
          