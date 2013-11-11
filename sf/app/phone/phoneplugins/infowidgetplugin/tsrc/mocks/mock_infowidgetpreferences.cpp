/* 
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
#include <QDebug>
#include <smcmockclassincludes.h>
#include "infowidgetpreferences.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// InfoWidgetPreferences::InfoWidgetPreferences
// -----------------------------------------------------------------------------
//
InfoWidgetPreferences::InfoWidgetPreferences( 
        QObject * parent ):
    QObject( parent )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::~InfoWidgetPreferences
// -----------------------------------------------------------------------------
//
InfoWidgetPreferences::~InfoWidgetPreferences(  )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::loadPreferences
// -----------------------------------------------------------------------------
//
void InfoWidgetPreferences::restorePreferences(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::storePreferences
// -----------------------------------------------------------------------------
//
bool InfoWidgetPreferences::storePreferences(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::preference
// -----------------------------------------------------------------------------
//
QString InfoWidgetPreferences::preference( 
        Option preferenceId ) const
    {
    SMC_MOCK_METHOD1( QString, Option, preferenceId )
    }

// -----------------------------------------------------------------------------
// InfoWidgetPreferences::preferences
// -----------------------------------------------------------------------------
//
InfoWidgetPreferences::Options InfoWidgetPreferences::preferences() const
    {
    SMC_MOCK_METHOD0( InfoWidgetPreferences::Options )
    }

// -----------------------------------------------------------------------------
// InfoWidgetPreferences::setPreference
// -----------------------------------------------------------------------------
//
void InfoWidgetPreferences::setPreference( 
        Option preferenceId,
        const QString & preferenceString )
    {
    SMC_MOCK_METHOD2( void, Option, preferenceId, 
        const QString &, preferenceString )
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::isPreferenceSet
// -----------------------------------------------------------------------------
//
bool InfoWidgetPreferences::isPreferenceSet( Option preferenceId ) const
    {
    SMC_MOCK_METHOD1( bool, Option, preferenceId)
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::validate
// -----------------------------------------------------------------------------
//
bool InfoWidgetPreferences::validate(  )
    {
    SMC_MOCK_METHOD0( bool)
    }



// -----------------------------------------------------------------------------
// InfoWidgetPreferences::visibleItemCount
// -----------------------------------------------------------------------------
//
int InfoWidgetPreferences::visibleItemCount(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// InfoWidgetPreferences::preferenceNames
// -----------------------------------------------------------------------------
//
QStringList InfoWidgetPreferences::preferenceNames() 
    {
    SMC_MOCK_METHOD0( QStringList )
    }
    
    
    