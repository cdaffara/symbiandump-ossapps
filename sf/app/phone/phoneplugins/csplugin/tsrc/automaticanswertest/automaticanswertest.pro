TEMPLATE = app
CONFIG += hb 

TRANSLATIONS = automaticanswertest.ts

symbian { 
    TARGET = automaticanswertest
    TARGET.UID3 = 0xE23CECCD
    TARGET.CAPABILITY = ALL -TCB 
        #-NetworkControl \
        # -NetworkServices \
        # -ReadDeviceData \
        # -MultimediaDD
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE \
										/epoc32/include/platform \
										/epoc32/include/ecom
    LIBS+= -letelmm
		LIBS+= -letel      
    LIBS+= -leuser 
    LIBS+= -lecom 										
        }


HEADERS += automaticanswertest.h \
					 etelincomingcallmonitor.h
SOURCES += main.cpp \
           automaticanswertest.cpp \
           etelincomingcallmonitor.cpp
FORMS += 
RESOURCES += 


symbian:MMP_RULES += SMPSAFE
