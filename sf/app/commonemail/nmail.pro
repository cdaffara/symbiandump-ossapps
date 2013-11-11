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
#

TEMPLATE = subdirs

symbian*: { 
    SUBDIRS = \
        email_plat \
        emailservices/nmailbase \
        emailuis/nmailuiengine \
        emailuis/nmailuiwidgets \
        emailuis/nmsettingui \
        emailuis/nmailcpplugin \
        emailuis/nmailui \
        emailuis/nmindicatorplugin \
        emailservices/emailcommon \
        emailservices/emailframework \
        emailservices/emailstore \
        emailservices/emailclientapi \
        ipsservices \
        emailuis/nmframeworkadapter \
        emailservices/nmclientapi \
        emailservices/nmailagent \
        emailservices/nmregister \
        emailservices/nmregister/nmregisterinstaller \
        emailuis/nmhswidget \
        emailservices/nmutilities
        
    # Need to remove warnings SYSTEMINCLUDE path "\epoc32\include\mw\hb\..." not found
    system(mkdir \\epoc32\\include\\mw\\hb\\hbcnf\\)
    system(mkdir \\epoc32\\include\\mw\\hb\\hbservices\\)
    
    # Need to remove warnings SYSTEMINCLUDE path "\epoc32\include\mw\QtXmlPatterns\" not found
    system(mkdir \\epoc32\\include\\mw\\QtXmlPatterns\\)
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "emailuis/rom/nmail.iby            CORE_APP_LAYER_IBY_EXPORT_PATH(nmail.iby)" \
        "emailuis/rom/nmaillanguage.iby    LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(nmaillanguage.iby)"
}

CONFIG += ordered
