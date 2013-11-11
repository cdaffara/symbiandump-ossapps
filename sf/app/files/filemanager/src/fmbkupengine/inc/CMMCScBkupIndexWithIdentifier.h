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
* Description: Declaration for CMMCScBkupIndexWithIdentifier
*     
*
*/

#ifndef __CMMCSCBKUPINDEXWITHIDENTIFIER_H__
#define __CMMCSCBKUPINDEXWITHIDENTIFIER_H__

// User includes
#include "CMMCScBkupIndexBase.h"
#include "TMMCScBkupOwnerDataType.h"
#include "TMMCScBkupArchiveVector.h"
#include "MMMCScBkupArchiveDataInterface.h"



/**
*
*
* @since 3.0
*/
template < class TIdentifier >
class CMMCScBkupIndexEntry : public CBase
    {
    public:
        inline static CMMCScBkupIndexEntry* NewLC( const TIdentifier& aIdentifier )
            {
            CMMCScBkupIndexEntry* self = new(ELeave) CMMCScBkupIndexEntry( aIdentifier );
            CleanupStack::PushL( self );
            return self;
            }

        inline static CMMCScBkupIndexEntry* NewLC( RReadStream& aStream )
            {
            CMMCScBkupIndexEntry* self = new(ELeave) CMMCScBkupIndexEntry();
            CleanupStack::PushL( self );
            aStream >> *self;
            return self;
            }

       inline ~CMMCScBkupIndexEntry()
            {
            iAssociatedData.Close();
            }

    private:

        inline CMMCScBkupIndexEntry()
            {
            }

        inline CMMCScBkupIndexEntry( const TIdentifier& aIdentifier )
            : iIdentifier( aIdentifier )
            {
            }

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    public:// Store & Restore

        inline void InternalizeL( RReadStream& aStream )
            {
            aStream.ReadInt32L(); // EStreamFormatVersion1
            aStream.ReadInt32L(); // spare1
            aStream.ReadInt32L(); // spare2
            aStream.ReadInt32L(); // spare3
            //
            iIdentifier = aStream.ReadInt32L();
            //
            const TInt count = aStream.ReadInt32L();
            for(TInt i=0; i<count; i++)
                {
                TMMCScBkupArchiveDriveAndVector entry;
                aStream >> entry;
                //
                iAssociatedData.AppendL( entry );
                }
            }

        inline void ExternalizeL( RWriteStream& aStream ) const
            {
            aStream.WriteInt32L( EStreamFormatVersion1 );
            aStream.WriteInt32L( 0 ); // spare1
            aStream.WriteInt32L( 0 ); // spare2
            aStream.WriteInt32L( 0 ); // spare3
            //
            aStream.WriteInt32L( iIdentifier );
            //
            const TInt count = iAssociatedData.Count();
            aStream.WriteInt32L(count);
            //
            for(TInt i=0; i<count; i++)
                {
                const TMMCScBkupArchiveDriveAndVector& entry = iAssociatedData[i];
                aStream << entry;
                }
            }

    public: // General access
        
        inline const TIdentifier& Identifier() const
            { return iIdentifier; }

        inline void AddEntryL( const TMMCScBkupArchiveDriveAndVector& aEntry )
            { iAssociatedData.AppendL(aEntry); }

        inline TInt Count() const
            { return iAssociatedData.Count(); }

        inline const TMMCScBkupArchiveDriveAndVector& Entry(TInt aIndex) const
            { return iAssociatedData[ aIndex ]; }

        inline const RArray<TMMCScBkupArchiveDriveAndVector>& Entries() const
            { return iAssociatedData; }

    private:

        /**
        *
        */
        TIdentifier iIdentifier;

        /**
        *
        */
        RArray<TMMCScBkupArchiveDriveAndVector> iAssociatedData;
   };






/**
*
*
* @since 3.0
*/
template < class TIdentifier >
class CMMCScBkupIndexWithIdentifier : public CMMCScBkupIndexBase
    {

    public:

        /**
        * C++ destructor
        */
        inline ~CMMCScBkupIndexWithIdentifier()
            {
            iEntries.Close();
            }

    protected:

        /**
        * C++ default constructor
        */
        inline CMMCScBkupIndexWithIdentifier( TMMCScBkupOwnerDataType aType )
            : CMMCScBkupIndexBase( aType )
            {
            }

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    public: // API

        /**
        *
        */
        inline void AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive, const TIdentifier& aIdentifier )
            {
            // Locate the parent
            CMMCScBkupIndexEntry< TIdentifier >& entry = EntryByIdentifierL( aIdentifier );

            // Create new sub-entry
            TMMCScBkupArchiveDriveAndVector subItem;
            subItem.iDrive = aDrive;
            subItem.iVector = aInfo;

            // Add it to our index
            entry.AddEntryL( subItem );
            }

        /**
        *
        */
        inline TInt Count() const
            { return iEntries.Count(); }

        /**
        *
        */
        inline const CMMCScBkupIndexEntry< TIdentifier >& At( TInt aIndex ) const
            { return *iEntries[ aIndex ]; }

        /**
        *
        */
        inline void StoreL(MMMCScBkupDriver& aDriver)
            {
            MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
            RWriteStream stream( archiveDataInterface.ADIWriteStreamUncompressedLC() );
            //    
            stream.WriteInt32L( EStreamFormatVersion1 );
            stream.WriteInt32L( 0 ); // spare1
            stream.WriteInt32L( 0 ); // spare2
            stream.WriteInt32L( 0 ); // spare3

            const TInt count = iEntries.Count();
            stream.WriteInt32L( count );
            //
            for(TInt i=0; i<count; i++)
                {
                const CMMCScBkupIndexEntry< TIdentifier > & entry = *iEntries[ i ];
                stream << entry;
                }
            //
            stream.CommitL();
            CleanupStack::PopAndDestroy(); // stream

            // Update our base class info with the offset to the index
            SetVector( archiveDataInterface.ADICurrentArchiveVectorInfo() );
            }

        /**
        *
        */
        inline void RestoreL(MMMCScBkupDriver& aDriver)
            {
            MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
            RReadStream stream( archiveDataInterface.ADIReadStreamUncompressedLC( Vector().Offset() ) );
            //    
            stream.ReadInt32L(); // EStreamFormatVersion1 
            stream.ReadInt32L(); // spare1
            stream.ReadInt32L(); // spare2
            stream.ReadInt32L(); // spare3
            //    
            const TInt count = stream.ReadInt32L();
            for(TInt i=0; i<count; i++)
                {
                CMMCScBkupIndexEntry< TIdentifier >* entry = CMMCScBkupIndexEntry< TIdentifier >::NewLC( stream );
                iEntries.AppendL( entry );
                CleanupStack::Pop( entry );
                }
            //
            CleanupStack::PopAndDestroy(); // stream
            //
            if  ( archiveDataInterface.ADICurrentArchiveVectorInfo().Length() > Vector().Length() )
                {
                // We've read too much!
                User::Leave( KErrCorrupt );
                }
            }

    private: // Internal methods

        /**
        *
        */
        inline CMMCScBkupIndexEntry< TIdentifier >* EntryByIdentifier( const TIdentifier& aIdentifier )
            {
            CMMCScBkupIndexEntry< TIdentifier >* ret = NULL;
            //
            const TInt count = iEntries.Count();
            for( TInt i=count-1; i>=0; i-- )
                {
                CMMCScBkupIndexEntry< TIdentifier >* entry = iEntries[ i ];
                if  ( entry->Identifier() == aIdentifier )
                    {
                    ret = entry;
                    break;
                    }
                }
            //
            return ret;
            }

        /**
        *
        */
        inline CMMCScBkupIndexEntry< TIdentifier >& EntryByIdentifierL( const TIdentifier& aIdentifier )
            {
            CMMCScBkupIndexEntry< TIdentifier >* ret = EntryByIdentifier( aIdentifier );
            if  ( !ret )
                {
                // Need to make a new entry
                ret = CMMCScBkupIndexEntry< TIdentifier >::NewLC( aIdentifier );
                iEntries.AppendL( ret );
                CleanupStack::Pop( ret );
                }
            //
            return *ret;
            }

    private: // Member Data

        //
        RMMCScBkupPointerArray< CMMCScBkupIndexEntry< TIdentifier > > iEntries;
    };






#endif // __CMMCSCBKUPINDEXWITHIDENTIFIER_H__

//End of File
