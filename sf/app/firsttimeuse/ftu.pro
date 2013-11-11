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
# Description:  FTU project file
#
TEMPLATE = subdirs

SUBDIRS  = ftuapplication \
           fturuntimeservices \        
           wizardproviders \           
           stateproviders \
	   runtimeproviders
        


tests:SUBDIRS += ftuapplication\tsrc\t_ftuapplication
tests:SUBDIRS += runtimeproviders\ftudefaultruntimeprovider\tsrc\t_ftudefaultruntimeprovider
tests:SUBDIRS += stateproviders\ftustateprovider\tsrc\t_ftustateprovider
tests:SUBDIRS += fturuntimeservices\tsrc\t_fturuntimeservices

CONFIG += ordered

symbian:include(ftu_exports_to_rom.pri)

symbian:MMP_RULES += SMPSAFE
