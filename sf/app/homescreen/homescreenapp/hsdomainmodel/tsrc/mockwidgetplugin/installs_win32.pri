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
localtestfiles.files = ./debug/mockwidgetplugin.dll ./resource/mockwidgetplugin.xml 
localtestfiles.path = ./../t_hsdomainmodel/debug/hsresources/plugins/widgetplugins

localqmfiles.files = ./resource/mockwidgetplugin.qm
localqmfiles.path = ./../t_hsdomainmodel/debug/resource/qt/translations

globaltestfiles.files = ./debug/mockwidgetplugin.dll ./resource/mockwidgetplugin.xml
globaltestfiles.path = ./../../../../../bin/hstests/hsresources/plugins/widgetplugins

globalqmfiles.files = ./resource/mockwidgetplugin.qm
globalqmfiles.path = ./../../../../../bin/hstests/resource/qt/translations


INSTALLS += localtestfiles globaltestfiles localqmfiles globalqmfiles 
