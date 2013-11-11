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
# Description: Music Player mpviewplugins unit test project file.
#

TEMPLATE = subdirs

SUBDIRS += ../mpcollectionviewplugin/tsrc \
           ../mpdetailsviewplugin/tsrc \
           ../mpmediawallviewplugin/tsrc \
           ../mpplaybackviewplugin/tsrc \
           ../mpsettingsviewplugin/tsrc

CONFIG += ordered

