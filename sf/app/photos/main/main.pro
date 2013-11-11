#/* 
#* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#* All rights reserved.
#* This component and the accompanying materials are made available
#* under the terms of "Eclipse Public License v1.0"
#* which accompanies this distribution, and is available
#* at the URL "http://www.eclipse.org/legal/epl-v10.html".
#*
#* Initial Contributors:
#* Nokia Corporation - initial contribution.
#*
#* Contributors:
#* 
#* Description:
#*
#*/ 
TEMPLATE = app
TARGET = photos
DEPENDPATH += .
INCLUDEPATH += . \
               ..\inc \
               ../ui/viewmanagement/statehandler/inc \
               ../loggers/loggerqt/inc \
               ../ui/inc \
               ../ui/uiengine/model/mediamodel/inc \
               ../ui/uiengine/medialistwrapper/inc \
               ../ui/uiengine/medialists/inc \
               ../loggers/loggerqt/inc \
               ../ui/views/viewsfactory/inc \
               ../ui/views/viewbase/inc \
               ../ui/views/gridview/inc \
               ../engine/collectionframework/plugins/glxcollectionpluginall/inc \
               ../traces

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += HB

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

symbian: { 
    TARGET.UID3 = 0x20000A14
    SKINICON = qtg_large_photos
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x14000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        41943040
    TARGET.CAPABILITY = ALL \
        -TCB
    MMP_RULES += SMPSAFE     
}

LIBS += -lglxstatehandler.dll \
        -lglxloggerqt.dll \
        -lglxviews.dll \
        -lglxmediamodel.dll \
        -lglxexternalutility.dll \
        -lglximageviewermanager.dll \
        -lglxmedialists.dll
		

# Input
SOURCES += main.cpp \
           glxaiwservicehandler.cpp \
           glxapplication.cpp
	         
HEADERS += glxaiwservicehandler.h \
           glxapplication.h

RESOURCES += ../photos.qrc
TRANSLATIONS= photos.ts
###### qthighway
CONFIG += service
LIBS+=-lxqservice -lxqserviceutil -lflogger
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
##### /qthighway

RSS_RULES += \
  "datatype_list = " \
  "      {" \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/jpeg\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/jpg\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/jp2\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/tiff\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-wmf\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/ico\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/gif\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-xbitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-win-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-windows-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/ms-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-ms-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/vnd.wap.wbmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/png\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-epoc-mbm\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/vnd.nokia.ota-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-ota-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/mng\";" \  
  "          }" \
  "      };" \

