#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Project file for calendar widget homescreen plugin
#

TEMPLATE = subdirs
SUBDIRS = calendarwidgetplugin \
          calendarwidgetinstaller

symbian: {
    BLD_INF_RULES.prj_exports += \
      "./rom/calendarwidget.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(calendarwidget.iby)" \
      "./rom/calendarwidgetresources.iby    LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(calendarwidgetresources.iby)"
    
    BLD_INF_RULES.prj_mmpfiles += \
      " gnumakefile sis/calendarwidget_stub_sis.mk" \
      
	
    BLD_INF_RULES.prj_extensions += \
      " START EXTENSION app-services/buildstubsis" \
      " OPTION SRCDIR ./sis" \
      " OPTION SISNAME calendarwidget_stub" \
      " END" \
}
          
