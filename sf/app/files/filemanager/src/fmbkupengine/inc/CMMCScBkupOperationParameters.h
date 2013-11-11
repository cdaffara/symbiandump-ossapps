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
* Description: Declaration for CMMCScBkupOpParamsBase
*     
*
*/

#ifndef __CMMCSCBKUPOPERATIONPARAMETERS_H__
#define __CMMCSCBKUPOPERATIONPARAMETERS_H__

// System includes
#include <connect/sbdefs.h>
#include <connect/sbtypes.h>
#include <barsread.h>
#include <babitflags.h>

// User includes
#include "MMCScBkupOperations.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "RMMCScBkupPointerArray.h"

// Namespaces
using namespace conn;

// Classes referenced
class CMMCScBkupDriveAndOperationTypeManager;
class CMMCScBkupArchiveInfo;

_LIT( KBackUpFolder, "\\Backup\\" );
_LIT( KBackUpFiles, "*.arc" );
const TInt KCArrayGranularity = 10;

class TBkupDrivesAndOperation
{
public:
	TBkupDrivesAndOperation(){}
	TBkupDrivesAndOperation( unsigned int drvCategories, unsigned int ownerDataType );
	TBkupDrivesAndOperation( const TBkupDrivesAndOperation &other )
    { 
        mDrvCategories  = other.mDrvCategories;
        mOwnerDataType  = other.mOwnerDataType;
    }

    void setDrvCategories( unsigned int drvCategories ){ mDrvCategories = drvCategories; }
    void setOwnerDataType( unsigned int ownerDataType ){ mOwnerDataType = ownerDataType; }

    unsigned int drvCategories(){ return mDrvCategories; }
    unsigned int ownerDataType(){ return mOwnerDataType; }
private:
    unsigned int mDrvCategories;
    unsigned int mOwnerDataType;
};

class CBkupCategory: CBase
{
private:
      
	CBkupCategory()
	{
	}
public:
	
	static CBkupCategory* NewL()
	{
	CBkupCategory* self = NewLC();
		CleanupStack::Pop(self);
		return self;
	}
	static CBkupCategory* NewLC()
	{
	    CBkupCategory* self = new(ELeave) CBkupCategory();
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
	}
	void ConstructL()
	    {
	    mUids = new ( ELeave ) CArrayFixFlat<unsigned int>( KCArrayGranularity );
	    mExclude_uids = new ( ELeave ) CArrayFixFlat<unsigned int>( KCArrayGranularity );
	    mArchive_name = 0;
	    }
	
    ~CBkupCategory()
    {
        mUids->Reset();
        mExclude_uids->Reset();
        delete mArchive_name;
    }

    void setCategory( unsigned int category )
    {
        mCategory = category;
    }

    void setArchive_name( TDesC16& archive_name )
    {
        if ( mArchive_name != 0 )
            {
            delete mArchive_name;            
            }		
        TRAP_IGNORE( mArchive_name = HBufC::NewL( archive_name.Length() ) );
        *mArchive_name = archive_name;
    }

    void setSpecial_flags( unsigned int special_flags )
    {
        mSpecial_flags = special_flags;
    }

    void setExclude_special_flags( unsigned int exclude_special_flags )
    { 
        mExclude_special_flags = exclude_special_flags; 
    }

    void addUids( unsigned int uid )
    {
        TRAP_IGNORE( mUids->AppendL( uid, sizeof(unsigned int) ) );
    }

    void addExclude_uids( unsigned int exclude_uid )
    {
        TRAP_IGNORE( mExclude_uids->AppendL( exclude_uid, sizeof(unsigned int) ) ); 
    }

    unsigned int        category()              { return mCategory; }
    HBufC*              archive_name()         { return mArchive_name; }
    unsigned int        special_flags()         { return mSpecial_flags; }
    unsigned int        exclude_special_flags() { return mExclude_special_flags; }
    CArrayFixFlat<unsigned int>* uids()         { return mUids; }
    CArrayFixFlat<unsigned int>* exclude_uids() { return mExclude_uids; }

private:
    unsigned int                     mCategory;
    HBufC*                           mArchive_name;
    unsigned int                     mSpecial_flags;
    unsigned int                     mExclude_special_flags;
    CArrayFixFlat<unsigned int>*     mUids;
    CArrayFixFlat<unsigned int>*     mExclude_uids;
};

typedef CArrayFixFlat<TBkupDrivesAndOperation> BkupDrivesAndOperationList;
typedef RPointerArray<CBkupCategory>          BkupCategoryList;

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupOpParamsBase) : public CBase
    {
    public:

        /**
        *
        */
        IMPORT_C ~CMMCScBkupOpParamsBase();

    protected:

        /**
        *
        */
        CMMCScBkupOpParamsBase( );

        /**
        *
        */
        void ConstructL( BkupDrivesAndOperationList *aDriveList, TBitFlags aCategories );

    public: // Common mandatory framework methods

        /**
        *
        */
        virtual TMMCScBkupOperationType AssociatedOpType() const = 0;

        /**
        *
        */
        virtual TBURPartType PartType() const = 0;

        /**
        *
        */
        virtual TBackupIncType IncrementType() const = 0;

        /**
        *
        */
        virtual TTransferDataType PassiveTransferType() const = 0;

        /**
        *
        */
        virtual TTransferDataType ActiveTransferType() const = 0;

        /**
        *
        */
        virtual TPackageDataType PackageTransferType() const = 0;

    public: // Common attributes

        /**
        *
        */
        IMPORT_C const CMMCScBkupDriveAndOperationTypeManager& DriveAndOperations() const;
        
        /**
        *
        */
        IMPORT_C void SetArchiveInfosL(RPointerArray<CMMCScBkupArchiveInfo>& aInfos);
        
        /**
        *
        */
        RMMCScBkupPointerArray<CMMCScBkupArchiveInfo>& ArchiveInfos();
        
        /**
        *
        */
        const CMMCScBkupArchiveInfo& ArchiveInfo(TBitFlags aCategory) const;

        /**
        *
        */
        const TDesC& FileName(TInt aIndex) const;

        /**
        *
        */
        TBitFlags Categories() { return iCategories; }
        
    private: // Data members

        //
        CMMCScBkupDriveAndOperationTypeManager* iDriveAndOperations;
        //
        RMMCScBkupPointerArray< CMMCScBkupArchiveInfo > iArchiveInfos;
        //
        TBitFlags iCategories;
    };



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupOpParamsBackupFull) : public CMMCScBkupOpParamsBase
    {
    public:

        /**
        *
        */
        IMPORT_C static CMMCScBkupOpParamsBackupFull* NewL( 
        		BkupDrivesAndOperationList *aDriveList, 
        		BkupCategoryList *aCategoryList,
            TDriveNumber aDrive, TBitFlags aCategories );

        /**
        *
        */
        IMPORT_C ~CMMCScBkupOpParamsBackupFull();

    private:

        /**
        *
        */
        CMMCScBkupOpParamsBackupFull( TDriveNumber aDrive );

        /**
        *
        */
        void ConstructL( BkupDrivesAndOperationList *aDriveList, 
        		BkupCategoryList *aCategoryList, TBitFlags aCategories );

        /**
        *
        */
        void ReadFromResourceL( BkupCategoryList *aCategoryList );
        
    public: // From CMMCScBkupOpParamsBase
        IMPORT_C TMMCScBkupOperationType AssociatedOpType() const;
        IMPORT_C TBURPartType PartType() const;
        IMPORT_C TBackupIncType IncrementType() const;
        IMPORT_C TTransferDataType PassiveTransferType() const;
        IMPORT_C TTransferDataType ActiveTransferType() const;
        IMPORT_C TPackageDataType PackageTransferType() const;

    private: // Data members

        //
        TDriveNumber iDrive;
    };




/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupOpParamsRestoreFull) : public CMMCScBkupOpParamsBase
    {
    public:

        /**
        *
        */
        IMPORT_C static CMMCScBkupOpParamsRestoreFull* NewL( 
        		BkupDrivesAndOperationList *aDriveList, TBitFlags aCategories );

        /**
        *
        */
        IMPORT_C ~CMMCScBkupOpParamsRestoreFull();

    private:

        /**
        *
        */
        CMMCScBkupOpParamsRestoreFull();

    public: // From CMMCScBkupOpParamsBase
        IMPORT_C TMMCScBkupOperationType AssociatedOpType() const;
        IMPORT_C TBURPartType PartType() const;
        IMPORT_C TBackupIncType IncrementType() const;
        IMPORT_C TTransferDataType PassiveTransferType() const;
        IMPORT_C TTransferDataType ActiveTransferType() const;
        IMPORT_C TPackageDataType PackageTransferType() const;
    };





#endif // __CMMCSCBKUPOPERATIONPARAMETERS_H__

//End of File
