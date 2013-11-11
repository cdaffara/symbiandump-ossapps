#
# Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 # Description: Implementation of applicationmanagement components
 #
 #
 
symbian: { 
    SYMBIAN_PLATFORMS = WINSCW \
        ARMV5
    :BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    :BLD_INF_RULES.prj_exports += "./src/200267FB.SPD /epoc32/release/armv5/udeb/z/private/100012a5/policy/200267FB.SPD"
    :BLD_INF_RULES.prj_exports += "./src/200267FB.SPD /epoc32/release/armv5/urel/z/private/100012a5/policy/200267FB.SPD"
    :BLD_INF_RULES.prj_exports += "./src/200267FB.SPD /epoc32/release/winscw/udeb/z/private/100012a5/policy/200267FB.SPD"
    :BLD_INF_RULES.prj_exports += "./src/200267FB.SPD /epoc32/release/winscw/urel/z/private/100012a5/policy/200267FB.SPD"
    :BLD_INF_RULES.prj_exports += "./src/200267FB.SPD /epoc32/data/z/private/100012a5/policy/200267FB.SPD"
    :BLD_INF_RULES.prj_exports += "./group/200267FB.txt /epoc32/data/z/private/10202be9/200267FB.txt"
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x200267FB
    TARGET.CAPABILITY = CAP_SERVER \
        TrustedUI \
        AllFiles \
        NetworkControl
        
RSS_RULES += "hidden = KAppIsHidden;"

    LIBS += -lWrtDownloadMgr \
        -lwrtserviceipcclient \
        -leuser \
        -ldrmhelper \
        -ldrmlicensemanager \
        -ledbms \
        -lsysutil \
        -lefsrv \
        -lesock \
        -lbafl \
        -lcentralrepository \
        -lestor \
        -lsisregistryclient \
        -laminstalloptions \
        -limut \
        -lswinstcli \
        -lezip \
        -lplatformenv \
        -lecom \
        -lcaf \
        -lnsmldmtreedbhandler \
        -lsyncmlclientapi \
        -lnsmldmmodule \
        -lnsmldmtreedbclient \
        -lconnmon \
        -lcommonengine \
        -lnsmltransporthandler \
        -lnsmltransport \
        -lcone \
        -ljavaregistryclient \
        -lapparc \
        -lfeatmgr \
        -lpolicyengine \
        -lflogger \ 
        -lcommdb \
        -lapgrfx \
        -lws32 \
        -lapmime \ 
        -lsif \
        -lcmmanager
}
TEMPLATE = app
TARGET = applicationmanagementserver
DEPENDPATH += ./inc
INCLUDEPATH += . \
    ../inc \
    ../options/inc \
    ../installer/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE \
    /epoc32/include/mw/cwrt \
    /epoc32/include/devman \
    /epoc32/include/platform/mw/swi \
    /epoc32/include/swi \
    /epoc32/include/ecom \
    /epoc32/include/mw/usif \
    /epoc32/include/platform/mw/cwrt
CONFIG += hb
RESOURCES += ./conf/applicationmanagement.qrc

SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

# Input
HEADERS +=./inc/amview.h \
    ./inc/amwaitprogdialog.h \
    ./inc/ServicePluginLogger.h \
    ./inc/AMDownloadStore.h \
    ./inc/ServicePluginLogger.h \
    ./inc/AMAppHideUtil.h \
    ./inc/AMDeploymentComponent.h \
    ./inc/amdeploymentcomponent.inl \
    ./inc/amdeploymentcomponentdata.h \ 
    ./inc/AMDownloaddb.h \ 
    ./inc/AMPreInstallApp.h \
    ./inc/amsmlhelper.h \
    ./inc/amstorage.h \
    ./inc/ApplicationManagementJavaUtility.h \
    ./inc/ApplicationManagementServer.h \
    ./inc/ApplicationManagementUtility.h \
    ./inc/ApplicationManagementUtilityFactory.h \
    ./inc/nsmldmiapmatcher.h \
    ./inc/appmgmtdownloadmgr.h \
    ./inc/nsmldmuri.h \
    ./inc/ServicePluginLogger.h \
    ./inc/appmgmtprogdialog.h \
    ./inc/PlatformSecurityPolicies.h \
    ./inc/AMDownloadStore.h \
    ./inc/appmgmtnotifier.h
SOURCES += ./src/amview.cpp \
    ./src/amwaitprogdialog.cpp \
    ./src/AMAppHideUtil.cpp \
    ./src/AMDeploymentComponent.cpp \
    ./src/AMDeploymentComponentData.cpp \ 
    ./src/AMDownloaddb.cpp \ 
    ./src/AMPreInstallApp.cpp \
    ./src/AMSmlHelper.cpp \
    ./src/amstorage.cpp \
    ./src/ApplicationManagementJavaUtility.cpp \
    ./src/ApplicationManagementServer.cpp \
    ./src/ApplicationManagementUtility.cpp \
    ./src/ApplicationManagementUtilityFactory.cpp \
    ./src/nsmldmiapmatcher.cpp \
    ./src/appmgmtdownloadmgr.cpp \
    ./src/nsmldmuri.cpp \
    ./src/appmgmtprogdialog.cpp \
    ./src/AMDownloadStore.cpp \
    ./src/main.cpp \
    ./src/appmgmtnotifier.cpp
