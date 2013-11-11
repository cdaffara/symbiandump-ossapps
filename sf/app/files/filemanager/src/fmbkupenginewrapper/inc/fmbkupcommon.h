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
 * 
 * Description:
 *      The header file of the backup common class
 */

#ifndef FMBKUPCOMMON_H
#define FMBKUPCOMMON_H


//#define KFileManagerUID3 0x101F84EB

namespace FmBkupEngineCommon
{

enum TFileManagerBackupContentMask
    {
    EFileManagerBackupContentAll = 0x1,
    EFileManagerBackupContentSettings = 0x2,
    EFileManagerBackupContentMessages = 0x4,
    EFileManagerBackupContentContacts = 0x8,
    EFileManagerBackupContentCalendar = 0x10,
    EFileManagerBackupContentBookmarks = 0x20,
    EFileManagerBackupContentUserFiles = 0x40,
    EFileManagerBackupContentFirst = 0x2,
    EFileManagerBackupContentLast = 0x40
    };


	// Partial backup categories bitmask values
	enum MAEngineBackupCategories
		{
		EBUCatSettings  = 0x1,
		EBUCatMessages  = 0x2,
		EBUCatContacts  = 0x4,
		EBUCatCalendar  = 0x8,
		EBUCatBookmarks = 0x10,
		EBUCatUserFiles = 0x20,
		EBUCatAllInOne  = 0x8000000,
		EBUCatAllSeparately = 0x7FFFFFF 
		// Set as 0xFFFFFFF (EBUCatAllInOne+EBUCatAllSeparately) if archive containing 
		// data from all data owners needs to be created.
		};
	
	// Bitmask values for special ruling of which category data owner belongs to
    enum MAEngineBackupCatSpecFlag
    {
	    EBUCatSpecNone      =   0x0,
	    EBUCatSpecSystem    =   0x1,
        EBUCatSpecJava      =   0x2,
	    EBUCatSpecPublic    =   0x4,
	    EBUCatSpecAll       =   0x8
    };
	
	// Defines drive caterories for multiple drives to setup backup sources and restore targets
    enum MAEngineBackupDriveType
    {
	    EBkupDeviceMemories         =   0x1,
	    EBkupInternalMassStorages   =   0x2,
	    EBkupExternalMassStorages   =   0x4
    };

    enum TMMCScBkupOwnerDataType
    {
    // Relates to all data owners
    EMMCScBkupOwnerDataTypeDataOwner = 0,

    // Relates to java data for a particular owner
    EMMCScBkupOwnerDataTypeJavaData,

    // Relates to public data for a particular owner
    EMMCScBkupOwnerDataTypePublicData,

    // Relates to system data for a particular owner
    EMMCScBkupOwnerDataTypeSystemData,

    // Relates to active data for a particular owner
    EMMCScBkupOwnerDataTypeActiveData,

    // Relates to passive data for a particular owner
    EMMCScBkupOwnerDataTypePassiveData,

    // Always leave this last, don't assign it
    // a value. Don't use it either!
    EMMCScBkupOwnerDataTypeCount,

    // A generic 'any data type' value. Used by the drive-specific-request &
    // data sizer. Not a real SBE data type, hence it appears after
    // the last marker.
    EMMCScBkupOwnerDataTypeAny
    };

    const QString const_Bkup_EBUCatSettings     ( "EBUCatSettings" );
    const QString const_Bkup_EBUCatMessages     ( "EBUCatMessages" );
    const QString const_Bkup_EBUCatContacts     ( "EBUCatContacts" );
    const QString const_Bkup_EBUCatCalendar     ( "EBUCatCalendar" );
    const QString const_Bkup_EBUCatBookmarks    ( "EBUCatBookmarks" );
    const QString const_Bkup_EBUCatUserFiles    ( "EBUCatUserFiles" );
    const QString const_Bkup_EBUCatAllInOne     ( "EBUCatAllInOne" );
    const QString const_Bkup_EBUCatAllSeparately( "EBUCatAllSeparately" );

    const QString const_bkup_EBUCatSpecNone   ( "EBUCatSpecNone" );
    const QString const_bkup_EBUCatSpecSystem ( "EBUCatSpecSystem" );
    const QString const_bkup_EBUCatSpecJava   ( "EBUCatSpecJava" );
    const QString const_bkup_EBUCatSpecPublic ( "EBUCatSpecPublic" );
    const QString const_bkup_EBUCatSpecAll    ( "EBUCatSpecAll" );

    const QString const_bkup_EBkupDeviceMemories( 
        "EBkupDeviceMemories"
        );
    const QString const_bkup_EBkupInternalMassStorages(
        "EBkupInternalMassStorages"
        );
    const QString const_bkup_EBkupExternalMassStorages(
        "EBkupExternalMassStorages"
        );

    const QString const_bkup_EMMCScBkupOwnerDataTypeDataOwner(
        "EMMCScBkupOwnerDataTypeDataOwner" 
        );
    const QString const_bkup_EMMCScBkupOwnerDataTypeJavaData(
        "EMMCScBkupOwnerDataTypeJavaData"
        );
    const QString const_bkup_EMMCScBkupOwnerDataTypePublicData(
        "EMMCScBkupOwnerDataTypePublicData"
        );
    const QString const_bkup_EMMCScBkupOwnerDataTypeSystemData(
        "EMMCScBkupOwnerDataTypeSystemData"
        );
    const QString const_bkup_EMMCScBkupOwnerDataTypeActiveData(
        "EMMCScBkupOwnerDataTypeActiveData"
        );
    const QString const_bkup_EMMCScBkupOwnerDataTypePassiveData(
        "EMMCScBkupOwnerDataTypePassiveData"
        );

}

class ConfigStringConverter
{
public:
    static unsigned int drivesAndOperations_DriveToUInt( bool* ok, QString paramString )
    {
        unsigned int ret = 0;
        *ok = false;
        if ( paramString.contains( 
            FmBkupEngineCommon::const_bkup_EBkupDeviceMemories,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBkupDeviceMemories;
        }
        if ( paramString.contains( 
            FmBkupEngineCommon::const_bkup_EBkupInternalMassStorages,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBkupInternalMassStorages;
        }
        if ( paramString.contains( 
            FmBkupEngineCommon::const_bkup_EBkupExternalMassStorages,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBkupExternalMassStorages;
        }
        return ret;
    }

    static unsigned int drivesAndOperations_TypeToUInt(
        bool* ok, QString paramString )
    {
        unsigned int ret = 0;
        *ok = false;
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EMMCScBkupOwnerDataTypeDataOwner,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EMMCScBkupOwnerDataTypeDataOwner;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EMMCScBkupOwnerDataTypeJavaData,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EMMCScBkupOwnerDataTypeJavaData;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EMMCScBkupOwnerDataTypePublicData,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EMMCScBkupOwnerDataTypePublicData;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EMMCScBkupOwnerDataTypeSystemData,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EMMCScBkupOwnerDataTypeSystemData;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EMMCScBkupOwnerDataTypeActiveData,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EMMCScBkupOwnerDataTypeActiveData;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EMMCScBkupOwnerDataTypePassiveData,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EMMCScBkupOwnerDataTypePassiveData;
        }
        return ret;
    }

    static unsigned int backupCategory_CategoryToUInt(
        bool* ok, QString paramString )
    {
        unsigned int ret = 0;
        *ok = false;
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatSettings,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatSettings;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatMessages,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatMessages;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatContacts,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatContacts;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatCalendar,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatCalendar;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatBookmarks,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatBookmarks;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatUserFiles,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatUserFiles;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatAllInOne,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatAllInOne;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_Bkup_EBUCatAllSeparately,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatAllSeparately;
        }
        return ret;
    }

    static unsigned int backupCategory_Special_flagsToUInt(
        bool* ok, QString paramString )
    {
        unsigned int ret = 0;
        *ok = false;
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EBUCatSpecNone,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatSpecNone;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EBUCatSpecSystem,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatSpecSystem;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EBUCatSpecJava,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatSpecJava;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EBUCatSpecPublic,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatSpecPublic;
        }
        if ( paramString.contains(
            FmBkupEngineCommon::const_bkup_EBUCatSpecAll,
            Qt::CaseInsensitive ) ){
                *ok = true;
                ret |= FmBkupEngineCommon::EBUCatSpecAll;
        }
        return ret;
    }
};
class FmBkupDrivesAndOperation
{
public:
    FmBkupDrivesAndOperation(){}
    FmBkupDrivesAndOperation( unsigned int drvCategories, unsigned int ownerDataType );
    FmBkupDrivesAndOperation( const FmBkupDrivesAndOperation &other )
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

class FmBkupBackupCategory
{
public:
    FmBkupBackupCategory(){}
    FmBkupBackupCategory( const FmBkupBackupCategory &other )
    { 
        mCategory               = other.mCategory;
        mArchive_name           = other.mArchive_name;
        mSpecial_flags          = other.mSpecial_flags;
        mExclude_special_flags  = other.mExclude_special_flags;
        mUids                   = other.mUids;
        mExclude_uids           = other.mExclude_uids;
    }
    ~FmBkupBackupCategory()
    {
        mUids.clear();
        mExclude_uids.clear();
    }

    void setCategory( unsigned int category )
    {
        mCategory = category;
    }

    void setArchive_name( QString archive_name )
    {
        mArchive_name = archive_name;
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
        mUids.append( uid);
    }

    void addExclude_uids( unsigned int exclude_uid )
    {
        mExclude_uids.append( exclude_uid ); 
    }

    unsigned int        category()              { return mCategory; }
    QString             archive_name()         { return mArchive_name; }
    unsigned int        special_flags()         { return mSpecial_flags; }
    unsigned int        exclude_special_flags() { return mExclude_special_flags; }
    QList<unsigned int> *uids()                 { return &mUids; }
    QList<unsigned int> *exclude_uids()         { return &mExclude_uids; }

private:
    unsigned int        mCategory;
    QString             mArchive_name;
    unsigned int        mSpecial_flags;
    unsigned int        mExclude_special_flags;
    QList<unsigned int> mUids;
    QList<unsigned int> mExclude_uids;
};


#endif  //FMBKUPCOMMON_H
