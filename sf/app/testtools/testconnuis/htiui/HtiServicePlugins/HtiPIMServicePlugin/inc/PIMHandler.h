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
* Description:  Functional implentation of PIM service.
*
*/


#ifndef CPIMHANDLER_H
#define CPIMHANDLER_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include <HtiServicePluginInterface.h>
#include <calprogresscallback.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CCalSession;

// CLASS DECLARATION

/**
*  Functional implentation of PIM service.
*/
class CPIMHandler : public CBase,
                    public MCalProgressCallBack
    {
    public:

        static CPIMHandler* NewL();

        // Interface implementation
        void ProcessMessageL( const TDesC8& aMessage,
            THtiMessagePriority aPriority );

        TBool IsBusy();

        virtual ~CPIMHandler();

        void SetDispatcher( MHtiDispatcher* aDispatcher );
      

        // From MCalProgressCallBack

        void Progress( TInt aPercentageCompleted );
        void Completed( TInt aError );
        TBool NotifyProgress();

    private:

        CPIMHandler();
        void ConstructL();

    private: // helpers

        void HandleVCardImportFuncL( const TDesC8& aData );
        void HandleVCalendarImportFuncL( const TDesC8& aData );
        void HandleContactDeleteFuncL( const TDesC8& aData );
        void HandleCalendarDeleteFuncL( const TDesC8& aData );
        void HandleOpenCalendarFileL( const TDesC8& aData );
        void HandleListCalendarFilesL();
        void HandleNotepadAddMemoFuncL( const TDesC8& aData );
        void HandleNotepadAddMemoFromFileFuncL( const TDesC8& aData );
        void HandleNotepadDeleteAllFuncL();
        
        void SendOkMsgL( const TDesC8& aData );
        void SendNotepadOkMsgL( CHtiPIMServicePlugin::TCommand aCommand );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private: // data
        TBool                 iIsBusy;
        MHtiDispatcher*       iDispatcher; // referenced
        CActiveSchedulerWait* iWaiter;
        TInt                  iEntryViewErr;
        CCalSession*          iCalSession;
    };

#endif // CPIMHANDLER_H
