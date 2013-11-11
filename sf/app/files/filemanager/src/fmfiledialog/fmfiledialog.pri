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
# Description: FmFileDialog project - source files
#


# API headers
HEADERS += ../../../files_plat/filemanager_api/inc/fmfiledialog.h
HEADERS += ../../../files_plat/filemanager_api/inc/fmglobal.h

# normal headers and sources
HEADERS +=  ./src/fmfiledialog_p.h \
            ./src/fmfilewidget.h

SOURCES +=  ./src/fmfiledialog.cpp \
            ./src/fmfiledialog_p.cpp \
            ./src/fmfilewidget.cpp
