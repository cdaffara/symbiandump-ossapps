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
* Description:  Header file for contact adapter test utilities.
*
*/

#ifndef CNSMLADAPTERTESTUTILITIES_H_
#define CNSMLADAPTERTESTUTILITIES_H_

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>    // CContactDatabase
#include <cntdef.h>   // TContactItemId 

class CNSmlAdapterTestUtilities : public CBase
    {
    public:
        
        static CNSmlAdapterTestUtilities* NewL();
        static CNSmlAdapterTestUtilities* NewLC();
        virtual ~CNSmlAdapterTestUtilities();

    private:    
        CNSmlAdapterTestUtilities();
        void ConstructL();

    public:
        
        void OpenVCardFromFileLC( const TDesC& aFileName, RFileReadStream& aReadStream  );
        HBufC* ReadVCardFromFileLC( const TDesC& aFileName );
        HBufC8* ReadVCard8FromFileLC( const TDesC& aFileName );
        CBufFlat* ReadBufFromFileLC( const TDesC& aFileName );
        
        // Setting up / Tearing down a test DB with a few contacts
        void SetupEmptyContactDbL( const TDesC& aDbName );
        void SetupAndPopulateContactDbL( const TDesC& aDbName );
        void TeardownAndEmptyContactDbL( const TDesC& aDbName );
        CContactDatabase* GetDb();
        CContactIdArray* GetContactArray();
        RFs& FileSession();
        CBufBase* ExportContactLC( TContactItemId aId );
        void PrintData( const TDesC& aTitle , CBufBase& aBuf );
        void PrintData( const TDesC8& aTitle , CBufBase& aBuf );

    public:
        // Helper methods
        TContactItemId AddContactL( const TDesC& aFirstName,
                 const TDesC& aLastName,
                 const TDesC& aPhoneNumber );
        
    private:    
        RFs                 iFs;
        CContactDatabase*   iContactDatabase;
        CContactIdArray*    iIdArray;
    };

#endif /* CNSMLADAPTERTESTUTILITIES_H_ */
