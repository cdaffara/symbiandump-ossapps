#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

include(../unittest.pri)

TARGET = unittest_cxeimagedataitemsymbian

LIBS *= -lplatformenv
LIBS *= -lsysutil
LIBS *= -lbitgdi
LIBS *= -lws32
LIBS *= -lefsrv
LIBS *= -lcone
LIBS *= -lecom
LIBS *= -lecampluginsupport


SOURCES *= cxeimagedataitemsymbian.cpp \
           cxeerrormappingsymbian.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxestatemachinebase.cpp \
           cxestillimagesymbian.cpp \
		   unittest_cxeimagedataitemsymbian.cpp \
		   cxeimagedataitemunit.cpp \
		   cxesysutil.cpp \
		   cxefakesysutil.cpp  


HEADERS *= cxeimagedataitemsymbian.h \
           cxestatemachine.h \
           cxestatemachinebase.h \
           cxestate.h \
           cxestillimagesymbian.h \
           cxeimagedataitem.h \
           unittest_cxeimagedataitemsymbian.h \
           cxeimagedataitemunit.h \
           cxesysutil.h \
           cxefakesysutil.h \
           cxeerror.h
