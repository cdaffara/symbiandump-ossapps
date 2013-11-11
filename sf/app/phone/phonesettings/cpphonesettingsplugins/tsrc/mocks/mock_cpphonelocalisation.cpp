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
#include "cpphonelocalisation.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhoneLocalisation::PhoneLocalisation
// -----------------------------------------------------------------------------
//
PhoneLocalisation::PhoneLocalisation( 
        QObject * /*parent*/ )
    //:
    //QObject( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneLocalisation::~PhoneLocalisation
// -----------------------------------------------------------------------------
//
PhoneLocalisation::~PhoneLocalisation(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneLocalisation::installTranslator
// -----------------------------------------------------------------------------
//
bool PhoneLocalisation::installTranslator( 
        TranslationFileId translationFileId )
    {
    SMC_MOCK_METHOD1( bool, TranslationFileId, translationFileId )
    }


// -----------------------------------------------------------------------------
// PhoneLocalisation::removeTranslators
// -----------------------------------------------------------------------------
//
void PhoneLocalisation::removeTranslators(  )
    {
    SMC_MOCK_METHOD0( void )
    }


