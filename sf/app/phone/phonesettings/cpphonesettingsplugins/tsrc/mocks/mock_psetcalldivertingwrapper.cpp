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
#include "psetcalldivertingwrapper.h"

class PSetCallDivertingWrapperPrivate {
public:
    PSetCallDivertingWrapperPrivate() {}
    ~PSetCallDivertingWrapperPrivate() {}
};


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::PSetCallDivertingWrapper
// -----------------------------------------------------------------------------
//
PSetCallDivertingWrapper::PSetCallDivertingWrapper( 
        CPsetContainer & psetContainer,
        QObject * parent )
    //:
    //QObject( /*psetContainer, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::~PSetCallDivertingWrapper
// -----------------------------------------------------------------------------
//
PSetCallDivertingWrapper::~PSetCallDivertingWrapper(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::setCallDiverting
// -----------------------------------------------------------------------------
//
int PSetCallDivertingWrapper::setCallDiverting( 
        PSCallDivertingCommand & aSetting,
        int aBasicServiceGroup )
    {
    SMC_MOCK_METHOD2( int, PSCallDivertingCommand &, aSetting, 
        int, aBasicServiceGroup )
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::getCallDivertingStatus
// -----------------------------------------------------------------------------
//
void PSetCallDivertingWrapper::getCallDivertingStatus( 
        const PsServiceGroup aServiceGroup,
        const PsCallDivertingCondition aCondition,
        int aBsc )
    {
    int iDummy = aServiceGroup;
    SMC_MOCK_METHOD3( void, int, iDummy, 
        const PsCallDivertingCondition, aCondition, 
        int, aBsc )
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::cancelProcess
// -----------------------------------------------------------------------------
//
void PSetCallDivertingWrapper::cancelProcess(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::getDefaultNumbers
// -----------------------------------------------------------------------------
//
void PSetCallDivertingWrapper::getDefaultNumbers( 
        QStringList & aDefNumbers )
    {
    QStringList list;
    list << "0401234567" << "0501234567" << "0451234567";
    aDefNumbers = list;
    
    SMC_MOCK_METHOD1( void, QStringList &, aDefNumbers )
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::setNewDefaultNumber
// -----------------------------------------------------------------------------
//
void PSetCallDivertingWrapper::setNewDefaultNumber( 
        QString aNumber )
    {
    SMC_MOCK_METHOD1( void, QString, aNumber )
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::getVoiceMailBoxNumber
// -----------------------------------------------------------------------------
//
int PSetCallDivertingWrapper::getVoiceMailBoxNumber( 
        QString & aNumber,
        PsService aService )
    {
    SMC_MOCK_METHOD2( int, QString &, aNumber, 
        PsService, aService )
    }


// -----------------------------------------------------------------------------
// PSetCallDivertingWrapper::queryVoiceMailBoxNumber
// -----------------------------------------------------------------------------
//
int PSetCallDivertingWrapper::queryVoiceMailBoxNumber( 
        QString & aNumber,
        PsService aService )
    {
    SMC_MOCK_METHOD2( int, QString &, aNumber, 
        PsService, aService )
    }

