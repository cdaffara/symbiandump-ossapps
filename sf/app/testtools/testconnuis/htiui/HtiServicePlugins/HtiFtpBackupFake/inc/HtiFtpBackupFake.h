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
* Description:  Definition of the CHtiFtpBackupFake class.
*
*/


#ifndef __HTIFTPBACKUPFAKE_H
#define __HTIFTPBACKUPFAKE_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <connect/sbeclient.h>
#include <HtiFtpBackupFakeBase.h>

using namespace conn;

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Definition of the class implementing the CHtiFtpBackupFakeBase
*/
NONSHARABLE_CLASS( CHtiFtpBackupFake ) : public CHtiFtpBackupFakeBase
    {
    public:
        // constructor support
        virtual void ConstructL( RFs* aFs );

        // destructor
        virtual ~CHtiFtpBackupFake();

    public: // from CHtiFtpBackupFakeBase
        virtual TInt ActivateBackup();
        virtual TInt DeactivateBackup();

    private:
        CSBEClient* iSBEClient; // owned
    };

#endif // __HTIFTPBACKUPFAKE_H

// End of File
