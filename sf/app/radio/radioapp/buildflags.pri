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


# ##########################################################
# Start of Flag definitions
# Add or remove comments to disable or enable the features
# ##########################################################

# Use of some debugging controls that are not part of official UI spec
# RADIOFLAGS += USE_DEBUGGING_CONTROLS

# Flag to use dummy radio data read from XML file
# RADIOFLAGS += USE_DUMMY_RADIO_DATA

# Flag to initialize radio stations with dummy rt
# RADIOFLAGS += INIT_STATIONS_WITH_DUMMY_RT

# Functional testing
# RADIOFLAGS += SHOW_CALLSIGN_IN_ANY_REGION

# Build flag to add EXPORTUNFROZEN to the pro file
# RADIOFLAGS += USE_UNFROZEN_EXPORTS

# Enables the assert macros
# RADIOFLAGS += ENABLE_ASSERTS

# ##########################################################
# Start of Logging flag definitions
# ##########################################################

# Full logging flag that enables the full logging including also timestamps
# Enabled by default in debug builds
# CONFIG(debug, debug|release) : LOGGING_FLAGS += LOGGING_ENABLED

# Uncomment to enable full logging in release builds
LOGGING_FLAGS *= LOGGING_ENABLED

# Timestamp logging flag that enables only timestamp logging
# LOGGING_FLAGS += TIMESTAMP_LOGGING_ENABLED

# Combines Ui and Engine logs by feeding UI traces to the engine logger
LOGGING_FLAGS += COMBINE_WITH_ENGINE_LOGGER

# Logging level
# 1 - Normal level
# 2 - More verbose level
# 3 - Most verbose level
LOGGING_FLAGS += LOGGING_LEVEL=1

# Select which radio component is being logged
# 1 - Radio application
# 2 - Radio homescreen widget
LOGGING_FLAGS += LOGGED_COMPONENT=1

contains(LOGGING_FLAGS, TIMESTAMP_LOGGING_ENABLED)|contains(LOGGING_FLAGS, LOGGING_ENABLED) {

    # Writes debug prints to file if enabled
    #LOGGING_FLAGS += TRACE_TO_FILE

    # Settings for the log file name
    symbian:contains(LOGGING_FLAGS, TRACE_TO_FILE) {
        LOGGING_FLAGS += \"TRACE_OUTPUT_FILE=\\\"c:/logs/radio/fmradioui.txt\\\"\"
        LOGGING_FLAGS += FILTER_BY_LOGMARKER=true
    }
}

# Method for testing signal-slot connections
# 1 - Log failed connection
# 2 - Log failed connection and halt debugger
LOGGING_FLAGS += CONNECT_TEST_MODE=2

win32: {
    LOGGING_FLAGS *= LOGGING_ENABLED    # Logging is always enabled in Win32 environment
    RADIOFLAGS += VID_DEFAULT=0
}

DEFINES += $$LOGGING_FLAGS

# ##########################################################
# End of Flag definitions
# ##########################################################

# Copy all radio flags to defines to define them as preprocessor macros
DEFINES += $$RADIOFLAGS
CONFIG += $$RADIOFLAGS
CONFIG += $$LOGGING_FLAGS

symbian: {
    DEFINES                 += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.VID              = VID_DEFAULT
    TARGET.CAPABILITY       = CAP_GENERAL_DLL
    INCLUDEPATH             += $$APP_LAYER_SYSTEMINCLUDE
    SYMBIAN_PLATFORMS       = WINSCW ARMV5
    MMP_RULES               += SMPSAFE
}

# QT 4.7 changes the def file location so force them to stay where they are
defFilePath     = ..

win32: {
    DEFINES     += BUILD_WIN32
    DESTDIR     = ../bin
    LIBS        += -L../bin
    INCLUDEPATH += ../radioenginewrapper/inc
    INCLUDEPATH += ../radioapplication/win32_stubs
    HEADERS     += ../radioapplication/win32_stubs/qsysteminfo.h
    HEADERS     += ../radioapplication/win32_stubs/xqserviceprovider.h
    HEADERS     += ../radioapplication/win32_stubs/xqserviceutil.h
    HEADERS     += ../radioapplication/win32_stubs/xqsettingskey.h
    HEADERS     += ../radioapplication/win32_stubs/xqsettingsmanager.h
    HEADERS     += ../radioapplication/win32_stubs/afactivitystorage.h
    SOURCES     += ../radioapplication/win32_stubs/win32_stubs.cpp
}

USE_UNFROZEN_EXPORTS {
    symbian:MMP_RULES   += "exportunfrozen"
    symbian:DEF_FILE    = not_used.def
}

# Place generated files away from the source folder
MOC_DIR         = /epoc32/build/radioapp/$$TMP_DIR_NAME
RCC_DIR         = /epoc32/build/radioapp/$$TMP_DIR_NAME
OBJECTS_DIR     = /epoc32/build/radioapp/$$TMP_DIR_NAME
