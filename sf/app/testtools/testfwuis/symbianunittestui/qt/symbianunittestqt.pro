HEADERS += mainwindow.h \
		dialogaddtests.h \
		tabwidgettestrun.h \
		widgettestrunner.h \
		dialogsettings.h \
		settings.h \
		testitemlist.h \
		dialogmsg.h
		
SOURCES += main.cpp \
		mainwindow.cpp \
		dialogaddtests.cpp \
		tabwidgettestrun.cpp \
		widgettestrunner.cpp \
		dialogsettings.cpp \
		settings.cpp \
		testitemlist.cpp \
		dialogmsg.cpp

RESOURCES += symbianunittestqt.qrc
symbian { 
    TARGET.UID3 = 0x2002EA7F
    TARGET.EPOCALLOWDLLDATA = 1
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
    INCLUDEPATH += /epoc32/include/mw
    INCLUDEPATH += /epoc32/include/platform
    INCLUDEPATH += /epoc32/include/platform/sutinternal
    HEADERS += testrunner_s60.h
    SOURCES += testrunner_s60.cpp
    LIBS += -leuser \
        -lefsrv \
        -lsymbianunittestfw \
        -lbafl
    TARGET.CAPABILITY = AllFiles \
        CommDD
    
    # Export headers to SDK Epoc32/include directory
    deploy.path = $$EPOCROOT
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    BLD_INF_RULES.prj_exports += "rom/sutui.iby CORE_IBY_EXPORT_PATH(tools,sutui.iby)"

}
