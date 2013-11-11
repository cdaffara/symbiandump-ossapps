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
* Description:  Functional implentation of message management service.
*
*/


#ifndef CHTIMESSAGESSERVICEPLUGINHANDLER_H
#define CHTIMESSAGESSERVICEPLUGINHANDLER_H


// INCLUDES
#include <msvapi.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CClientMtmRegistry;

// CLASS DECLARATION
/**
*  Functional implentation of message management service.
*/
class CMessageMgmntHandler : public CBase,
                             public MMsvSessionObserver
    {
public:

    static CMessageMgmntHandler* NewL();

    // Interface implementation
    void ProcessMessageL( const TDesC8& aMessage,
                          THtiMessagePriority aPriority );

    virtual ~CMessageMgmntHandler();

    void SetDispatcher( MHtiDispatcher* aDispatcher );

    enum TFolder
        {
        EAllFolders = 0x00,
        EInbox,
        EDrafts,
        ESent,
        EOutbox,
        ENumberOfFolders, // this must always be the last one
        };

    enum TMessageType
        {
        EAllMessageTypes = 0x00,
        ESMS,
        EMMS,
        ESmartMessage,
        EEmail,
        EIrMessage,
        EBtMessage,
        EAudioMessage,
        EEmailPOP3,
        EEmailIMAP4,
        ENumberOfMessageTypes,  // this must always be the last one
        };

private:

    CMessageMgmntHandler();
    void ConstructL();

private: // helpers

    void HandleCreateSmsL( const TDesC8& aData );
    void HandleCreateMmsL( const TDesC8& aData );
    void HandleCreateEmailL( const TDesC8& aData );
    void HandleCreateObexMsgL( const TDesC8& aData, TUid aMtmUid,  TUid aMsgTypeUid );
    void HandleCreateSmartMsgL( const TDesC8& aData );

    void HandleDeleteMessageL( const TDesC8& aData );
    void HandleDeleteMessagesL( const TDesC8& aData );
    void HandleDeleteFromAllFoldersL( TMessageType aType );
    void HandleDeleteAllMessageTypesL( TFolder aFolder );
    void HandleDeleteFromFolderByTypeL( TFolder aFolder, TMessageType aType );

    void SendOkMsgL( const TDesC8& aData );
    void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    TBool ValidateAddSmsCommand( const TDesC8& aData );
    TBool ValidateAddMmsOrAddEmailCommand( const TDesC8& aData );
    TBool ValidateAddObexMsgCommand( const TDesC8& aData );
    TBool ValidateAddSmartMsgCommand( const TDesC8& aData );

    HBufC16* ExtractDesLC( const TDesC8& aUtf8Data, TInt& aPosition, TInt aSizeBytes );
    HBufC8* ExtractDes8LC( const TDesC8& aUtf8Data, TInt& aPosition, TInt aSizeBytes );
    TMsvId MapFolderToIdL( TFolder aFolder );
    TUid MapMessageTypeToUidL( TMessageType aType );

private: // from MMsvSessionObserver

    void HandleSessionEventL( TMsvSessionEvent aEvent,
                              TAny* aArg1,
                              TAny* aArg2,
                              TAny* aArg3 );

private:

    MHtiDispatcher*      iDispatcher; // referenced
    CMsvSession*         iSession;
    CClientMtmRegistry*  iMtmReg;
    };



// CLASS DECLARATION
/**
*  Helper class to wait the async requests.
*/
class CWaiter : public CActive
    {
public:
    static CWaiter* NewL( TInt aPriority = EPriorityStandard );
    static CWaiter* NewLC( TInt aPriority = EPriorityStandard );
    ~CWaiter();

    void StartAndWait();
    TInt Result() const;

private:
    CWaiter( TInt aPriority );

    // from CActive
    void RunL();
    void DoCancel();

private:
    CActiveSchedulerWait iWait;
    TInt iResult;
    };

#endif // CHTIMESSAGESSERVICEPLUGINHANDLER_H
