/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store encrypt handler.
*
*/



#ifndef MESSAGE_STORE_ENCRYPT_HANDLER_H_
#define MESSAGE_STORE_ENCRYPT_HANDLER_H_

#include "ContainerStore.h"
#include "ContainerStoreDefs.h"
#include "EncryptableTable.h"

class CContainerStoreGeneralTable;
class CContainerStoreUtils;

//class MEncryptHandlerObserver
//    {
//    public:
//        virtual void EncryptOrDecryptComplete( TInt aErrCode ) = 0;
//    };

/** The class encrypt database records asynchronoously. */   
class CMessageStoreEncryptHandler : private CActive
    {
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
        static CMessageStoreEncryptHandler* NewL( CContainerStoreGeneralTable& aGeneralTable,
                                                   CContainerStoreUtils&        aUtils,
                                                   TInt                         aPriority );
        
        virtual ~CMessageStoreEncryptHandler();
        
        void AddEncryptableTableL( MEncryptableTable* aEncryptableTable );
        
        void EncryptL();
        
        void DecryptL();
        
        void StartL();
        
        TBool IsInProgress() { return IsActive(); }
        
    private:

        // ===============
        // PRIVATE METHODS
        // ===============
        
        CMessageStoreEncryptHandler(  CContainerStoreGeneralTable& aGeneralTable,
                                       CContainerStoreUtils&        aUtils,
                                       TInt                         aPriority );
        
        void ConstructL();
        
        void ScheduleNext();
        
        void DoNextOperationL();
        
        void InitState();
            
        // inherited from CActive
        void RunL();
        void DoCancel();

        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
            
        CContainerStoreGeneralTable&     iGeneralTable;
        CContainerStoreUtils&            iUtils;
        
        TEncryptionState                 iEncryptionState;  //EIdle, EEncrypting, EDecrypting
        RPointerArray<MEncryptableTable> iEncryptableTables;
        
        TInt                             iCurTableIndex;
        TBool                            iIsFirstInTable;
        TDbBookmark                      iBookmark;
        
        __LOG_DECLARATION
        
    }; // end MESSAGE_STORE_ENCRYPT_HANDLER_H_



#endif /*CONTAINER_STORE_ENCRYPT_HANDLER_H_*/
