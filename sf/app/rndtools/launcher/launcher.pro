TEMPLATE = app
TARGET = Launcher
load(hb.prf)
symbian:CONFIG -= symbian_i18n
DEPENDPATH += src
DEPENDPATH += inc
INCLUDEPATH += inc

HEADERS += launchermainwindow.h applicationview.h enginewrapper.h notifications.h  outputview.h commonActions.h
SOURCES += main.cpp launchermainwindow.cpp applicationview.cpp enginewrapper.cpp notifications.cpp  outputview.cpp commonActions.cpp
RESOURCES += launcher.qrc

symbian: {

        BLD_INF_RULES.prj_exports += "./rom/Launcher.iby                CORE_IBY_EXPORT_PATH(tools,Launcher.iby)"
        BLD_INF_RULES.prj_exports += "./rom/backup_registration.xml     Z:/private/101fb74f/backup_registration.xml"

        launcher_stub_sis_mk = \
        "$${LITERAL_HASH}if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__S60_32__)" \
        "   $${LITERAL_HASH}ifdef MARM" \
        "       gnumakefile .sis/launcher_stub_sis.mk" \
        "   $${LITERAL_HASH}endif" \
        "$${LITERAL_HASH}endif"
        BLD_INF_RULES.prj_mmpfiles += launcher_stub_sis_mk
        
        launcher_buildstubsis_extension = \
        "$${LITERAL_HASH}if(!defined __SERIES60_30__ && !defined __SERIES60_31__ && !defined __S60_32__)" \
        "   $${LITERAL_HASH}ifdef MARM" \
        "       START EXTENSION app-services/buildstubsis" \
        "       OPTION SRCDIR sis" \
        "       OPTION SISNAME Launcher_stub" \
        "       END" \
        "   $${LITERAL_HASH}endif" \
        "$${LITERAL_HASH}endif"
        BLD_INF_RULES.prj_extensions += launcher_buildstubsis_extension
        
        INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
                     
        DEPENDPATH += engine/inc
        DEPENDPATH += engine/src
        INCLUDEPATH += engine/inc
        
        HEADERS += launcherengine.h launchertraces.h e32image.h e32imageheaders.h launcherxmlparser.h launcherdllelement.h launcherparserobserver.h launcherdllparser.h
		SOURCES += launcherengine.cpp e32image.cpp launcherxmlparser.cpp launcherdllelement.cpp launcherdllparser.cpp
		
		RSS_RULES += "group_name = \"RnD Tools\"";
		
		LIBS += -lapgrfx -lPlatformEnv -lxqutils -lxmlframework -lbafl -lshareui -lws32 -lapparc -lefsrv -lcone


        TARGET.CAPABILITY = ReadUserData ReadDeviceData WriteUserData WriteDeviceData PowerMgmt AllFiles
        
        TARGET.UID2 = 0x100039CE
        TARGET.UID3 = 0x101FB74F
        TARGET.SID = 0x101FB74F
        TARGET.VID = 0x101FB657 // Nokia
	
        TARGET.EPOCHEAPSIZE = 0x20000 0x2000000  // Min 128Kb, Max 32Mb

        ICON = ./icons/qgn_menu_launcher.svg 

} else {
    error("Only Symbian supported!")
}
