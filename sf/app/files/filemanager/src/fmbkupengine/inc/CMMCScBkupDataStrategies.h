/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration of CMMCScBkupDataStrategy 
*     
*
*/

#ifndef __CMMCSCBKUPDATASTRATEGIES_H__
#define __CMMCSCBKUPDATASTRATEGIES_H__

// User includes
#include <f32file.h>

// Constants
const TBool KMMCScBkupUseTempFile = EFalse;


/**
*
*
* @since 3.0
*/
class CMMCScBkupDataStrategy : public CBase
    {
    public:

        /**
        * Factory method
        */
        static CMMCScBkupDataStrategy* ReadStrategyLC( const TDesC& aName, RFs& aFsSession );

        /**
        * Factory method
        */
        static CMMCScBkupDataStrategy* ReadStrategyLC( const TDesC& aName, RFs& aFsSession, TInt aStartOffset, TInt aLengthToRead );

        /**
        * Factory method
        */
        static CMMCScBkupDataStrategy* WriteStrategyLC( const TDesC& aName, RFs& aFsSession, TBool aUseTempFile );

    public: // Framework API

        /**
        * Read data from the source
        *
        * @return TInt The amount of data read, or 0 if the end of file has been reached
        */
        virtual TInt Read(TDes8& aSink) = 0;

        /**
        * Write data to the source
        *
        * @return TInt KErrNone if success, KErrNotSupported if the strategy doesn't support writing
        *         or any of the other System-Wide error codes upon failure.
        */
        virtual TInt Write(const TDesC8& aSource);
        
        /**
        * Perform any final actions
        *
        * @return TInt an error code.
        */
        virtual TInt Finalize();

    protected:

        /**
        * C++ Constructor
        */
        CMMCScBkupDataStrategy();

    private: // Internal methods

        /**
        * Sets the offset and length
        */
        void SetOffsetAndLength(TInt aStartOffset, TInt aLengthToRead);

    protected: // Internal
        inline TInt Offset() const { return iOffset; }
        inline TInt LengthToRead() const { return iLengthToRead; }
        inline void SetOffset(TInt64 aOffset) { iOffset = aOffset; }

    private: // Data members

        //
        TInt iOffset;
        //
        TInt iLengthToRead;
    };







/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDataFileStrategy) : public CMMCScBkupDataStrategy
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupDataFileStrategy* NewForReadingL( const TDesC& aName, RFs& aFsSession );

        /**
        * Static constructor
        */
        static CMMCScBkupDataFileStrategy* NewForWritingL( const TDesC& aName, RFs& aFsSession, TBool aUseTempFile );

        /**
        * C++ Destructor
        */
        ~CMMCScBkupDataFileStrategy();

    private:

        /**
        * C++ Constructor
        */
        CMMCScBkupDataFileStrategy( RFs& aFsSession, TBool aUseTempFile = EFalse );

        /**
        * Second phase construction
        */
        void ConstructL( const TDesC& aName, TUint aMode );

    public: // From CMMCScBkupDataStrategy
        TInt Read(TDes8& aSink);
        TInt Write(const TDesC8& aSource);
        TInt Finalize();

    private: // Internal functions
        
        /**
        *
        */
        static TInt PrepareToOverwriteFile( RFs& aFsSession, const TDesC& aFileName );
    
    private: // Data members

        RFs& iFsSession;
        //
        RFile64 iFile;
        //
        TBool iUseTempFile;
        //
        HBufC* iFileName;
        //
        TFileName iTempFileName;
    };







/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDataFileSectionReadStrategy) : public CMMCScBkupDataStrategy
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupDataFileSectionReadStrategy* NewL(const TDesC& aName, RFs& aFsSession);

        /**
        * C++ Destructor
        */
        ~CMMCScBkupDataFileSectionReadStrategy();

    private:

        /**
        * C++ Constructor
        */
        CMMCScBkupDataFileSectionReadStrategy(RFs& aFsSession);

        /**
        *
        */
        void ConstructL(const TDesC& aName);

    public: // From CMMCScBkupDataStrategy
        TInt Read(TDes8& aSink);

    private: // Data members

        //
        RFs& iFsSession;
        //
        HBufC* iFileName;
    };




#endif // __CMMCSCBKUPDATASTRATEGIES_H__

//End of File
