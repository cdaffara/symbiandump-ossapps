/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The document class for ConnTest, stores the settings
*
*/

// INCLUDE FILES
#include "ConnTestDocument.h"
#include "ConnTestAppUi.h"
#include "ConnTestApp.h"
#include "SettingData.h"
#include "CustomPrefsData.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CConnTestDocument::CConnTestDocument(CEikApplication& aApp)
    : CEikDocument(aApp)    
    {
    }

// destructor
CConnTestDocument::~CConnTestDocument()
    {
    RDebug::Print(_L("ConnTest: CConnTestDocument::~CConnTestDocument"));
    for(TInt i = 0; i < KConnTestViews; i++)
        {
        delete iSettingDataArray[i];
        delete iCustomPrefsDataArray[i];
        }
    }

// Symbian default constructor can leave.
void CConnTestDocument::ConstructL()
    {
    RDebug::Print(_L("ConnTest: CConnTestDocument::ConstructL"));
    for(TInt i = 0; i < KConnTestViews; i++)
        {
        iSettingDataArray[i] = CSettingData::NewL();
        iCustomPrefsDataArray[i] = CCustomPrefsData::NewL();
        }
    }

// Two-phased constructor.
CConnTestDocument* CConnTestDocument::NewL(CEikApplication& aApp)
    {
    CConnTestDocument* self = new (ELeave) CConnTestDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------
// CConnTestDocument::CreateAppUiL()
// constructs CConnTestAppUi
// ----------------------------------------------------
//
CEikAppUi* CConnTestDocument::CreateAppUiL()
    {
    return new (ELeave) CConnTestAppUi(iSettingDataArray, iCustomPrefsDataArray);
    }


// ----------------------------------------------------
// CConnTestDocument::StoreL()
// Store current settings into file \\private\\101F6D2B\\ConnTest
// ----------------------------------------------------
//
void CConnTestDocument::StoreL(CStreamStore& aStore, 
                               CStreamDictionary& aStreamDic) const
    {
    RStoreWriteStream stream;
    TStreamId id = stream.CreateLC(aStore);
    for(TInt i = 0; i < KConnTestViews; i++)
        {
        stream << iSettingDataArray[i]->iServerName;
        stream.WriteInt32L(iSettingDataArray[i]->iPort);
        stream.WriteInt32L(iSettingDataArray[i]->iProtocol);
        stream.WriteInt32L(iSettingDataArray[i]->iRoaming);
        stream.WriteInt32L(iSettingDataArray[i]->iPacketSize);
        stream.WriteInt32L(iSettingDataArray[i]->iPackets);
        stream.WriteInt32L(iSettingDataArray[i]->iDelay);
        stream << iSettingDataArray[i]->iHttpPage;
        }
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream); // stream
    aStreamDic.AssignL(KUidConnTest, id);
    }


// ----------------------------------------------------
// CConnTestDocument::RestoreL()
// Read previous settings from file
// ----------------------------------------------------
//
void CConnTestDocument::RestoreL(const CStreamStore& aStore, 
                                 const CStreamDictionary& aStreamDic)
    {
    TStreamId id = aStreamDic.At(KUidConnTest);
    RStoreReadStream stream;
    stream.OpenLC(aStore, id);
    for(TInt i = 0; i < KConnTestViews; i++)
        {
        stream >> iSettingDataArray[i]->iServerName;
        iSettingDataArray[i]->iPort       = stream.ReadInt32L();
        iSettingDataArray[i]->iProtocol   = stream.ReadInt32L();
        iSettingDataArray[i]->iRoaming    = stream.ReadInt32L();
        iSettingDataArray[i]->iPacketSize = stream.ReadInt32L();
        iSettingDataArray[i]->iPackets    = stream.ReadInt32L();
        iSettingDataArray[i]->iDelay      = stream.ReadInt32L();
        stream >> iSettingDataArray[i]->iHttpPage;
        }
    CleanupStack::PopAndDestroy(&stream); // stream    
    }


// End of File  
