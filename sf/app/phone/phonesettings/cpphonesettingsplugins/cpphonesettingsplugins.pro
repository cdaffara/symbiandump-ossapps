# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
TEMPLATE = subdirs
SUBDIRS = cptelephonyutils \
    telephonyplugin \
    divertplugin \
    callsplugin \
    barringplugin \
    cpnetworkplugin
CONFIG += ordered
symbian: { 
    load(data_caging_paths)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    deploy.path = C:
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "./rom/cpphonesettingsplugins.iby CORE_APP_LAYER_IBY_EXPORT_PATH(cpphonesettingsplugins.iby)" \
        "./rom/cpphonesettingsplugins_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cpphonesettingsplugins_resources.iby)"
}
HEADERS = cptelephonyutils/inc/cpphonelocalisation.h
SOURCES = cptelephonyutils/src/cpphonelocalisation.cpp
