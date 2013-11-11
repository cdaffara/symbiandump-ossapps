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

// User includes

#include "cellulardatasettings.h"
#include "cellulardatasettings_s60_p.h"
#include "cppluginlogging.h"

/*!
    \class CellularDataSettings
    \brief Class for maintaining and updating the Cellular Data Settings.

*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

CellularDataSettings::CellularDataSettings() :
    QObject(),
    d_ptr(new CellularDataSettingsPrivate(this))
{
    DPRINT;
}

/*!
    Destructor.
*/

CellularDataSettings::~CellularDataSettings()
{
    DPRINT << ": IN";
    
    delete d_ptr;
    
    DPRINT << ": OUT";
}

/*!
    Getter for setting "Cellular data usage in home network".
*/

int CellularDataSettings::dataUseHome()
{
    DPRINT;
    
    return d_ptr->dataUseHome();
}

/*!
    Setter for setting "Cellular data usage in home network".
*/

void CellularDataSettings::setDataUseHome(int value)
{
    DPRINT << ": IN";
    
    // The value must be valid
    Q_ASSERT(value <= DataUsageHomeWlanOnly);
    d_ptr->setDataUseHome(value);
    
    DPRINT << ": OUT";
}
    
/*!
    Getter for setting "Cellular data usage when roaming".
*/

int CellularDataSettings::dataUseRoam()
{
    DPRINT;
    
    return d_ptr->dataUseRoam();
}

/*!
    Getter for setting "Cellular data usage when roaming".
*/

void CellularDataSettings::setDataUseRoam(int value)
{
    DPRINT << ": IN";
    
    // The value must be valid
    Q_ASSERT(value <= DataUsageAbroadWlanOnly);
    d_ptr->setDataUseRoam(value);
    
    DPRINT << ": OUT";
}
