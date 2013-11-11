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
* Description: Declaration of CMMCScBkupBufferManagerBase
*     
*
*/

#ifndef __CMMCSCBKUPBUFFERMANAGERS_H__
#define __CMMCSCBKUPBUFFERMANAGERS_H__

// User includes
#include <e32std.h>
#include <e32cmn.h>
#include <f32file.h>
#include <ezbufman.h>

// User includes
#include "TMMCScBkupArchiveVector.h"

// Classes referenced
class MMMCScBkupArchiveDataInterface;
class CMMCScBkupDataStrategy;
class MMMCScBkupProgressObserver;

// Constants
const TInt KScBkupDefaultBufferSize = 0x4000;

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupBufferManagerBase) : public CBase, public MEZBufferManager 
    {
    public: // Enumerations

        /**
        *
        */
        enum TType
            {
            ETypeDescriptorToArchive = 0,
            ETypeFileToArchive,
            ETypeArchiveToDescriptor,
            ETypeArchiveToFile
            };

    public: // Construct / destruct

        /**
        *
        */
        static CMMCScBkupBufferManagerBase* NewByTypeL( TType aType, MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI, TAny* aParam1 = NULL, TAny* aParam2 = NULL );

        /**
        *
        */
        ~CMMCScBkupBufferManagerBase();

    protected: // Internal construct

        /**
        *
        */
        CMMCScBkupBufferManagerBase( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI, TType aType );

        /**
        *
        */
        virtual void ConstructL( TInt aBufferSize = KScBkupDefaultBufferSize );

    public: // Framework API

        /**
        *
        */
        inline TType Type() const { return iType; }

        /**
        *
        */
        inline const TMMCScBkupArchiveVector& Info() const { return iInfo; }

        /**
        *
        */
        inline TMMCScBkupArchiveVector& Info() { return iInfo; }

        /**
        *
        */
        virtual void InitializeL( TAny* aParam1 = NULL, TAny* aParam2 = NULL ) = 0;

    protected: // Internal methods

        /**
        *
        */
        inline TPtr8& WriteBuffer() { return iBufferPointer; }

        /**
        *
        */
        inline const TDesC8& DataView() const { return *iBuffer; }

        /**
        *
        */
        inline MMMCScBkupProgressObserver& ProgressManager() const { return iProgressManager; }

        /**
        *
        */
        inline MMMCScBkupArchiveDataInterface& ADI() const { return iADI; }

        /**
        *
        */
        void WriteToArchiveAndUpdateStatsL( const TDesC8& aData );

    private:

        /**
        *
        */
        TType iType;

        /**
        *
        */
        HBufC8* iBuffer;

        /**
        *
        */
        TPtr8 iBufferPointer;

        /**
        *
        */
        MMMCScBkupProgressObserver& iProgressManager;

        /**
        *
        */
        MMMCScBkupArchiveDataInterface& iADI;

        /**
        *
        */
        TMMCScBkupArchiveVector iInfo;
    };



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupBufferManagerDescriptorToArchive) : public CMMCScBkupBufferManagerBase
    {
    private:

        /**
        *
        */
        CMMCScBkupBufferManagerDescriptorToArchive( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

        /**
        *
        */
        static CMMCScBkupBufferManagerDescriptorToArchive* NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

    public: // From CMMCScBkupBufferManagerBase
        void InitializeL(TAny* aParam1, TAny* aParam2);

    private: // From MEZBufferManager
        void InitializeL( CEZZStream& aZStream );
        void NeedInputL( CEZZStream& aZStream );
        void NeedOutputL( CEZZStream& aZStream );
        void FinalizeL( CEZZStream& aZStream );

    private: // Data members

        /**
        *
        */
        TPtrC8 iSource;

        friend class CMMCScBkupBufferManagerBase;
    };




/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupBufferManagerFileToArchive) : public CMMCScBkupBufferManagerBase 
    {
    public:

        /**
        *
        */
        ~CMMCScBkupBufferManagerFileToArchive();

    private:

        /**
        *
        */
        CMMCScBkupBufferManagerFileToArchive( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

        /**
        *
        */
        void ConstructL(const TDesC& aFileName);

        /**
        *
        */
        static CMMCScBkupBufferManagerFileToArchive* NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

    public: // From CMMCScBkupBufferManagerBase
        void InitializeL(TAny* aParam1, TAny* aParam2);

    private: // From MEZBufferManager
        void InitializeL( CEZZStream& aZStream );
        void NeedInputL( CEZZStream& aZStream );
        void NeedOutputL( CEZZStream& aZStream );
        void FinalizeL( CEZZStream& aZStream );

    private: // Data members

        /**
        *
        */
        CMMCScBkupDataStrategy* iReadStrategy;

        /**
        *
        */
        HBufC8* iOutput;

        /**
        *
        */
        TPtr8 iOutputPointer;
        
        /**
        * Useful for debugging
        */
        TPtrC iFileName;

        friend class CMMCScBkupBufferManagerBase;
    };






/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupBufferManagerArchiveToDescriptor) : public CMMCScBkupBufferManagerBase
    {
    public:

        /**
        *
        */
        ~CMMCScBkupBufferManagerArchiveToDescriptor();

    private:

        /**
        *
        */
        CMMCScBkupBufferManagerArchiveToDescriptor( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

        /**
        *
        */
        static CMMCScBkupBufferManagerArchiveToDescriptor* NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

    public: // From CMMCScBkupBufferManagerBase
        void InitializeL(TAny* aParam1, TAny* aParam2);

    private: // From MEZBufferManager
        void InitializeL( CEZZStream& aZStream );
        void NeedInputL( CEZZStream& aZStream );
        void NeedOutputL( CEZZStream& aZStream );
        void FinalizeL( CEZZStream& aZStream );

    private: // Internal methods
        
        /**
        *
        */
        void TryToReadMoreSourceDataL();

    private: // Data members

        /**
        *
        */
        TPtr8 iOutputPointer;

        friend class CMMCScBkupBufferManagerBase;
    };









/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupBufferManagerArchiveToFile) : public CMMCScBkupBufferManagerBase 
    {
    public:

        /**
        *
        */
        ~CMMCScBkupBufferManagerArchiveToFile();

    private:

        /**
        *
        */
        CMMCScBkupBufferManagerArchiveToFile( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

        /**
        *
        */
        void ConstructL(const TDesC& aFileName);

        /**
        *
        */
        static CMMCScBkupBufferManagerArchiveToFile* NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI );

    public: // From CMMCScBkupBufferManagerBase
        void InitializeL(TAny* aParam1, TAny* aParam2);

    private: // From MEZBufferManager
        void InitializeL( CEZZStream& aZStream );
        void NeedInputL( CEZZStream& aZStream );
        void NeedOutputL( CEZZStream& aZStream );
        void FinalizeL( CEZZStream& aZStream );

    private: // Internal methods
        
        /**
        *
        */
        void TryToReadMoreSourceDataL();

    private: // Data members

        /**
        *
        */
        CMMCScBkupDataStrategy* iWriteStrategy;

        /**
        *
        */
        HBufC8* iOutput;

        /**
        *
        */
        TPtr8 iOutputPointer;
        
        /**
        * Needed, since we restore to temporary file
        * and then delete and rename temp file if
        * extraction succeeds.
        */
        TPtrC iFileName;

        friend class CMMCScBkupBufferManagerBase;
    };















#endif // __CMMCSCBKUPBUFFERMANAGERS_H__

//End of File
