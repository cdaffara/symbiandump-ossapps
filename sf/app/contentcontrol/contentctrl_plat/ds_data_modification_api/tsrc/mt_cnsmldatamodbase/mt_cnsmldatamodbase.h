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
* Description:  Header file for data mod unit tests.
*
*/

#ifndef __MT_CNSMLDATAMODBASE_H__
#define __MT_CNSMLDATAMODBASE_H__

// INCLUDE FILES
#include <CEUnitTestSuiteClass.h>
#include <stringpool.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CNSmlDataModBase;
class CSmlDataStoreFormat;
class RFileReadStream;
class CBufFlat;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( MT_CNSmlDataModBase ): public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static MT_CNSmlDataModBase* NewL();
        static MT_CNSmlDataModBase* NewLC();
        ~MT_CNSmlDataModBase();

    private: // Constructors

        MT_CNSmlDataModBase();
        void ConstructL();

    private: // New methods

         void SetupL();
         void SetupAndConfigure1L();
         void SetupAndConfigure2L();
         void Teardown();
        
         void EmptyL();
         void NeedsMerge1L();
         void NeedsMerge2L();
         void StripTx1L();
         void StripTx2L();
         void StripTx3L();
         void BasicMerge1L();
         void BasicMerge2L();
         void FieldLevelMerge1L();
         
         // Helper functions
         void ConfigureDataModL( const TDesC& aOwnFormat, const TDesC& aPartnerFormat );
         HBufC8* ReadStoreFormatLC( const TDesC& aResourceFile );
         CBufFlat* ReadBufFromFileLC( const TDesC& aFileName );
         void OpenVCardFromFileLC( const TDesC& aFileName, RFileReadStream& aReadStream );
         
    private: // Data
        CNSmlDataModBase* iDataMod;
        RFs iFs;
        RStringPool iOwnStringPool;
        CSmlDataStoreFormat* iOwnStoreFormat;
        RStringPool iPartnerStringPool;
        CSmlDataStoreFormat* iPartnerStoreFormat;
        
        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __MT_CNSMLDATAMODBASE_H__
