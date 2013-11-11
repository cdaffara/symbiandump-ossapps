/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*/

// System includes

#include "cmmanager.h"

// User includes

#include "cellulardatasettings.h"
#include "cellulardatasettings_s60_p.h"
#include "cppluginlogging.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

CellularDataSettingsPrivate::CellularDataSettingsPrivate(CellularDataSettings *q_pointer) :
    q_ptr(q_pointer)
{
    DPRINT;
}

/*!
    Destructor.
*/

CellularDataSettingsPrivate::~CellularDataSettingsPrivate()
{
    DPRINT;
}

/*!
    Getter for setting "Cellular data usage in home network".
*/

int CellularDataSettingsPrivate::dataUseHome()
{
    DPRINT << ": IN";
    
    int result = 0;

    // Read the setting
    TCmGenConnSettings settings;
    readSettings(&settings);
    
    // Map the settings read result
    switch (settings.iCellularDataUsageHome) {
    case ECmCellularDataUsageAutomatic:
        result = CellularDataSettings::DataUsageHomeAutomatic;
        break;

    case ECmCellularDataUsageConfirm:
        result = CellularDataSettings::DataUsageHomeConfirm;
        break;

    case ECmCellularDataUsageDisabled:
        result = CellularDataSettings::DataUsageHomeWlanOnly;
        break;
    }
    
    DPRINT << ": OUT";
    
    return result;
}

/*!
    Setter for setting "Cellular data usage in home network".
*/

void CellularDataSettingsPrivate::setDataUseHome(int value)
{
    DPRINT << ": IN";
    
    TCmGenConnSettings settings;
    
    readSettings(&settings);    
    switch (value) {
    case CellularDataSettings::DataUsageHomeAutomatic:
        settings.iCellularDataUsageHome = ECmCellularDataUsageAutomatic; 
        break;

    case CellularDataSettings::DataUsageHomeConfirm:
        settings.iCellularDataUsageHome = ECmCellularDataUsageConfirm;
        break;

    case CellularDataSettings::DataUsageHomeWlanOnly:
        settings.iCellularDataUsageHome = ECmCellularDataUsageDisabled;
        break;
    }
    writeSettings(&settings);
    
    DPRINT << ": OUT";
}
    
/*!
    Getter for setting "Cellular data usage when roaming".
*/

int CellularDataSettingsPrivate::dataUseRoam()
{
    DPRINT << ": IN";
    
    int result = 0;

    // Read the setting
    TCmGenConnSettings settings;
    readSettings(&settings);
    
    // Map the settings read result
    switch (settings.iCellularDataUsageVisitor) {
    case ECmCellularDataUsageAutomatic:
        result = CellularDataSettings::DataUsageAbroadAutomatic;
        break;
        
    case ECmCellularDataUsageConfirm:
        result = CellularDataSettings::DataUsageAbroadConfirm;
        break;
        
    case ECmCellularDataUsageDisabled:
        result = CellularDataSettings::DataUsageAbroadWlanOnly;
        break;
    }
    
    DPRINT << ": OUT";
    
    return result;
}

/*!
    Getter for setting "Cellular data usage when roaming".
*/

void CellularDataSettingsPrivate::setDataUseRoam(int value)
{
    DPRINT << ": IN";
    
    TCmGenConnSettings settings;
    
    readSettings(&settings);    
    switch (value) {
    case CellularDataSettings::DataUsageAbroadAutomatic:
        settings.iCellularDataUsageVisitor = ECmCellularDataUsageAutomatic; 
        break;

    case CellularDataSettings::DataUsageAbroadConfirm:
        settings.iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;
        break;

    case CellularDataSettings::DataUsageAbroadWlanOnly:
        settings.iCellularDataUsageVisitor = ECmCellularDataUsageDisabled;
        break;
    }
    writeSettings(&settings);
    
    DPRINT << ": OUT";
}

/*!
    Cellular data settings reading from CM Manager.
*/

void CellularDataSettingsPrivate::readSettings(TCmGenConnSettings *settings)
{
    DPRINT << ": IN";
    
    QT_TRAP_THROWING(
        RCmManager CmManager;
        CmManager.CreateTablesAndOpenL();
        CleanupClosePushL(CmManager); 
        CmManager.ReadGenConnSettingsL(*settings);
        CleanupStack::PopAndDestroy(1));
    
    DPRINT << ": Read: iCellularDataUsageHome= " << settings->iCellularDataUsageHome;
    DPRINT << ": Read: iCellularDataUsageVisitor= " << settings->iCellularDataUsageVisitor;
    
    DPRINT << ": OUT";
    
}

/*!
    Cellular data settings writing to CM Manager.
*/

void CellularDataSettingsPrivate::writeSettings(TCmGenConnSettings *settings)
{
    DPRINT << ": IN";
    
    DPRINT << ": Write: iCellularDataUsageHome= " << settings->iCellularDataUsageHome;
    DPRINT << ": Write: iCellularDataUsageVisitor=" << settings->iCellularDataUsageVisitor;

    QT_TRAP_THROWING(
        RCmManager CmManager;
        CmManager.CreateTablesAndOpenL();
        CleanupClosePushL(CmManager); 
        CmManager.WriteGenConnSettingsL(*settings);
        CleanupStack::PopAndDestroy(1););
    
    DPRINT << ": OUT";
}
