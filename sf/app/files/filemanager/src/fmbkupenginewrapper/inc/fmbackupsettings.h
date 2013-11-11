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
 *      The header file of the backup seetings of file manager
 */
#ifndef FMBACKUPSETTINGS_H
#define FMBACKUPSETTINGS_H

#include "fmbkupengine_global.h"

#include <QTime>
#include <QString>
#include <QList>

#include <hbglobal.h>

class FmBkupEngine;

// CLASS DECLARATION
/**
 *  This class is used for storing backup entry which used in backup view
 *  Each row in backup view is a single entry
 *
 */
class FmBackupEntry
{
public:
    /**  Backup setting type */
    enum TSettingType
    {
        ENone = 0,
        EContents,
        EScheduling,
        EWeekday,
        ETime,
        ETarget,
        EBackupdate
    };

public:
    FmBackupEntry( const QString& title, const QString& tips, const TSettingType type ) :
        mTitle( title ), mTips( tips ), mType( type )
    {
    }
    ~FmBackupEntry( void )
    {
    }

    FmBackupEntry( const FmBackupEntry &other )
    { 
        mTitle = other.mTitle;
        mTips  = other.mTips;
        mType  = other.mType;
    }

    FmBackupEntry &operator =( const FmBackupEntry &other )
    {
        if( this == &other ){
            return *this;
        }

        mTitle = other.mTitle;
        mTips  = other.mTips;
        mType  = other.mType;
        return *this;
    } 

    QString title()
    {
        return mTitle;
    }

    QString tips()
    {
        return mTips;
    }

    FmBackupEntry::TSettingType type()
    {
        return mType;
    }


private:
    /**
     * first label for item in backup view
     */
    QString mTitle;

    /**
     * second label for item in backup view
     */
    QString mTips;

    /**
     * type for item in backup view
     */
    TSettingType mType;
};

// CLASS DECLARATION
/**
 *  This class is used for storing backup settings
 *
 */
class FMBKUPENGINE_EXPORT FmBackupSettings
{
public:
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

    /** Contains the possible values of KFileManagerBackupScheduling key. */
    enum TFileManagerBackupSchedule
    {
        EFileManagerBackupScheduleNever = 0,
        EFileManagerBackupScheduleDaily,
        EFileManagerBackupScheduleWeekly
    };

    /** Contains the possible values of KFileManagerBackupScheduling key. */
    enum TFileManagerBackupWeekday
    {
        EFileManagerBackupWeekdayMonday = 0,
        EFileManagerBackupWeekdayTuesday,
        EFileManagerBackupWeekdayWednesday,
        EFileManagerBackupWeekdayThursday,
        EFileManagerBackupWeekdayFriday,
        EFileManagerBackupWeekdaySaturday,
        EFileManagerBackupWeekdaySunday,
    };

public:
    explicit FmBackupSettings( FmBkupEngine *aFmBkupEngine );
    ~FmBackupSettings( void );


   /**
     * Sets backup contents
     * @param aContent Content bitmask
     */
    void setContent( const quint32 aContent );

    /**
     * Sets backup scheduling
     * @param aScheduling Scheduling type
     */
    void setScheduling( const TFileManagerBackupSchedule aScheduling );

    /**
     * Sets backup weekday
     * @param weekday Backup weekday
     */
    void setWeekday( const TFileManagerBackupWeekday weekday );

    /**
     * Sets backup time
     * @param aTime Time from 00:00
     */
    void setTime( const QTime& aTime );

    /**
     * Sets backup target drive
     * @param aDrive Target drive (See TDriveNumber)
     */
    void setTargetDrive( const QString& aDrive );

    /**
     * Gets backup contents
     * @return Content bitmask
     * @see TFileManagerBackupContentMask
     */
    quint32 content() const;

    /**
     * Gets backup scheduling
     * @return Scheduling type
     */
    FmBackupSettings::TFileManagerBackupSchedule scheduling() const;

    /**
     * Gets backup weekday
     * @return Backup weekday
     */
    FmBackupSettings::TFileManagerBackupWeekday weekday() const;

    /**
     * Gets backup time
     * @return Backup time
     */
    const QTime& time() const;

    /**
     * Gets backup target drive
     * @return Backup time
     */
    QString targetDrive() const;

    /**
     * Gets available backup target drive
     * if targetDrive exist, return targetDrive
     * otherwise return other first available backup drive
     * @return Available backup target drive, empty QString for null
     */
    QString availableTargetDrive() const;
    
    /**
     * Loads saved backup settings
     */
    void load();

    /**
     * Saves backup settings
     */
    void save();

    /**
     * get backup entry list
     * @return backup entry list
     */
    QList< FmBackupEntry* > backupEntryList();

    /**
     * Gets String from single content bit
     * @param aContent content bit
     * @return Textid
     */
    static QString contentToString( const quint32 content );

    
    /**
     * Gets String from weekday
     * @param weekday 
     * @return string 
     */
    static QString weekdayToString( const TFileManagerBackupWeekday weekday );

    /**
     * Gets String from scheduling
     * @param scheduling
     * @return string 
     */
    QString schedulingToString( const TFileManagerBackupSchedule scheduling );
    /**
     * Gets String from targetDrive
     * @param targetDrive drive name string
     * @return string
     */
    QString targetDriveToString( const QString& targetDrive );
    
    /**
     * Updates the backup date
     */
    void updateBackupDate();
private:
    /**
     * Gets the count of contents selected
     */
    int contentsSelected() const;

    FmBackupEntry* CreateEntry( const QString& title, const QString& tips, const FmBackupEntry::TSettingType type );

    ///////////////////////////////////////////////////////////////////////
    /**
     * Creates backup setting entry from all contents bit
     */
    FmBackupEntry* createContentsEntry();

    /**
     * Creates backup setting entry from scheduling
     */
    FmBackupEntry* createSchedulingEntry();

    /**
     * Creates backup setting entry from weekday
     */
    FmBackupEntry* createWeekdayEntry();

    /**
     * Creates backup setting entry from time
     */
    FmBackupEntry* createTimeEntry();

    /**
     * Creates backup setting entry from time
     */
    FmBackupEntry* createTargetDriveEntry();

    /**
     * Creates backup date entry
     */
    FmBackupEntry* createBackupDateEntry();
    
    void refreshList();
    void resetAndDestoryBackupEntry();
private:
   /**
     * Content bitmask
     * @see TFileManagerBackupContentMask
     */
    quint32 mContent;

    /**
     * Scheduling type
     */
    TFileManagerBackupSchedule mScheduling;

    /**
     * Scheduled backup weekday
     */
    TFileManagerBackupWeekday mWeekday;

    /**
     * Scheduled backup time from 00:00
     */
    QTime mTime;

    /**
     * Backup target drive
     */
    QString mTargetDrive;

    /**
     * Backup date
     */
    QDate mDate;
    
    /**
     * Backup setting list items
     */
    QList< FmBackupEntry* > mBackupEntryList;
    
    
    FmBkupEngine *mBkupEngine;
};

#endif //FMBACKUPSETTINGS_H
