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
localtestfiles.files = ./debug/mockwidgetplugintoobig.dll ./resource/mockwidgetplugintoobig.xml
localtestfiles.path = ./../t_hsdomainmodel/debug/hsresources/plugins/widgetplugins

globaltestfiles.files = ./debug/mockwidgetplugintoobig.dll ./resource/mockwidgetplugintoobig.xml
globaltestfiles.path = ./../../../../../bin/hstests/hsresources/plugins/widgetplugins

INSTALLS += localtestfiles globaltestfiles
