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

#include <coemain.h>
CCoeStatic* CCoeEnv::Static( TUid ) { return NULL; }


#include "CPhoneDataPortMessage.h"
extern CPhoneDataPortMessage* CDataPortMessage_NewLReturnValue;
extern MPEEngineInfo* CDataPortMessage_NewLEngineInfo;
CPhoneDataPortMessage* CPhoneDataPortMessage::NewL( MPEEngineInfo& aInfo ) 
    {
    CDataPortMessage_NewLEngineInfo = &aInfo;
    return CDataPortMessage_NewLReturnValue; 
    }


#include "CPhoneLogger.h"
CPhoneMethodLogger::CPhoneMethodLogger( TPhoneUILoggingComponent, TPtrC16 ) {}
CPhoneMethodLogger::~CPhoneMethodLogger() {}
void CPhoneLogger::LogIt( TPhoneUILoggingLevel, TPhoneUILoggingComponent, TRefByValue<const TDesC>, ...) {}
