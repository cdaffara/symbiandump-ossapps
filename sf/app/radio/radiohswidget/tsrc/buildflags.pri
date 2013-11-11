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

# ##########################################################
# Start of Logging flag definitions
# ##########################################################

# Full logging flag that enables the full logging including also timestamps
# Enabled by default in debug builds
# CONFIG(debug, debug|release) : LOGGING_FLAGS += LOGGING_ENABLED

# Uncomment to enable full logging in release builds
# LOGGING_FLAGS *= LOGGING_ENABLED

# Timestamp logging flag that enables only timestamp logging
# LOGGING_FLAGS += TIMESTAMP_LOGGING_ENABLED

# Combines Ui and Engine logs by feeding UI traces to the engine logger
# LOGGING_FLAGS += COMBINE_WITH_ENGINE_LOGGER

# Logging level
# 1 - Normal level
# 2 - More verbose level
# 3 - Most verbose level
LOGGING_FLAGS += LOGGING_LEVEL=1

# Select which radio component is being logged
# 1 - Radio application
# 2 - Radio homescreen widget
LOGGING_FLAGS += LOGGED_COMPONENT=2

contains(LOGGING_FLAGS, TIMESTAMP_LOGGING_ENABLED)|contains(LOGGING_FLAGS, LOGGING_ENABLED) {

    # Writes debug prints to file if enabled
    #LOGGING_FLAGS += TRACE_TO_FILE

    # Settings for the log file name
    symbian:contains(LOGGING_FLAGS, TRACE_TO_FILE) {
        LOGGING_FLAGS += \"TRACE_OUTPUT_FILE=\\\"c:/logs/radio/radiohswidget.txt\\\"\"
        LOGGING_FLAGS += FILTER_BY_LOGMARKER=true
    }
}

# Method for testing signal-slot connections
# 1 - Log failed connection
# 2 - Log failed connection and halt debugger
LOGGING_FLAGS += CONNECT_TEST_MODE=2

# Logging is always enabled in Win32 environment
win32:LOGGING_FLAGS *= LOGGING_ENABLED

DEFINES += $$LOGGING_FLAGS

# ##########################################################
# End of Flag definitions
# ##########################################################

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR         = $$_PRO_FILE_PWD_/tmp
RCC_DIR         = $$_PRO_FILE_PWD_/tmp
OBJECTS_DIR     = $$_PRO_FILE_PWD_/tmp
UI_DIR          = $$_PRO_FILE_PWD_/tmp
UI_HEADERS_DIR  = $$_PRO_FILE_PWD_/tmp
UI_SOURCES_DIR  = $$_PRO_FILE_PWD_/tmp
