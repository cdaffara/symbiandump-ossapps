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
# Description:  snsrbigclockscreensaverplugin.pri
#

HEADERS += ./inc/*.h \
           ./snsrclockwidgets/inc/*.h \
           ./snsrindicators/inc/*.h \
           ./snsrswipewidget/inc/*.h

SOURCES += ./src/*.cpp \
           ./snsrclockwidgets/src/*.cpp \
           ./snsrindicators/src/*.cpp \
           ./snsrswipewidget/src/*.cpp

DEPENDPATH += ./inc \
              ./src \
              ./resource

INCLUDEPATH += ./inc \
               ./snsrclockwidgets/inc \
               ./snsrindicators/inc \
               ./snsrswipewidget/inc
