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
#include "CPhoneMediatorCommandListener.h"
#include "globals.h"

extern TInt commandListenerInitialised;
CPhoneMediatorCommandListener* CPhoneMediatorCommandListener::Instance() { return NULL; }
void CPhoneMediatorCommandListener::Initialize( MPhoneMenuAndCbaEvents*, MPhoneEngineMessageSender*, MPEEngineInfo* ) 
        { 
        commandListenerInitialised  = ETrue;
        };

#include "CPhoneMediatorMessageFactory.h"

CPhoneMediatorMessageFactory* CPhoneMediatorMessageFactory::NewL( MPEEngineInfo& ) 
    {
    if( Singleton::Instance()->iLeave )
        {
        User::Leave( KErrNoMemory );
        return NULL; // never reached, here just because otherwise compiler complains
        }
    else 
        {
        return NULL;     
        }
    }
MPhoneMediatorMessage* CPhoneMediatorMessageFactory::CreateMessageL( TInt, TInt ) 
    {
    if( Singleton::Instance()->iCreateMessageLeave )
        {
        User::Leave( KErrNoMemory );
        return NULL; // never reached, here just because otherwise compiler complains
        }
    else
        {
        if( Singleton::Instance()->iMessageObject != 0 )
            {
            return reinterpret_cast<MPhoneMediatorMessage*>( Singleton::Instance()->iMessageObject );
            }
        else
            {
            return NULL;                     
            }
        }
    }

#include "CPhoneMediatorSender.h"

CPhoneMediatorSender* CPhoneMediatorSender::Instance() 
    {
    if( Singleton::Instance()->iSenderObject != 0 )
        {
        return reinterpret_cast<CPhoneMediatorSender*>( Singleton::Instance()->iSenderObject );
        }
    else
        {
        return NULL;         
        }
    }

#include "cphonelogger.h"

CPhoneMethodLogger::CPhoneMethodLogger(
        TPhoneUILoggingComponent /*aLoggingComponent*/,
        TPtrC /*aString*/ )
    {
    }

CPhoneMethodLogger::~CPhoneMethodLogger()
    {
    }

#include <coemain.h>
CCoeStatic* CCoeEnv::Static( TUid ) { return NULL; }

CCoeStatic::~CCoeStatic() {}
CCoeStatic::CCoeStatic() {}
CCoeStatic::CCoeStatic( TUid, TScope ) { }
void CCoeStatic::CCoeStatic_Reserved1() {}
void CCoeStatic::CCoeStatic_Reserved2() {}



