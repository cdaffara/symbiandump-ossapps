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
 *     The header file of the restore settings of file manager
 */

#ifndef FMRESTORESETTINGS_H
#define FMRESTORESETTINGS_H

#include "fmbkupengine_global.h"

#include <QDateTime>
#include <QString>
#include <QList>

class FmBkupDrivesAndOperation;
class FmBkupEngine;
// CLASS DECLARATION
/**
 *  This class is used for storing restore info
 */
class FmRestoreInfo
{
public:
    FmRestoreInfo( const quint32 content, const QDateTime &dateTime, const QString &drive ) :
        mContent( content ), mDateTime ( dateTime ), mDrive ( drive )
    {
    }
    ~FmRestoreInfo( void )
    {
    }

    quint32 content() const
    {
        return mContent;
    }

    QDateTime dateTime() const
    {
        return mDateTime;
    }
    QString drive() const
    {
        return mDrive;
    }

    FmRestoreInfo( const FmRestoreInfo &other )
    { 
        mContent  = other.mContent;
        mDateTime = other.mDateTime;
        mDrive    = other.mDrive;
    }

    FmRestoreInfo &operator =( const FmRestoreInfo &other )
    {
        if( this == &other ){
            return *this;
        }

        mContent  = other.mContent;
        mDateTime = other.mDateTime;
        mDrive    = other.mDrive;
        return *this;
    } 

private:
    quint32     mContent;   /// single content mask id
    QDateTime   mDateTime;
    QString     mDrive;
};

// CLASS DECLARATION
/**
 *  This class is used for storing restore entry which used in restore view
 *  Each row in restore view is a single entry
 */
class FmRestoreEntry
{
public:
    FmRestoreEntry( const QString& text, const FmRestoreInfo& info ) :
        mText( text ), mRestoreInfo( info )
    {
    }
    ~FmRestoreEntry( void )
    {
    }

    FmRestoreEntry( const FmRestoreEntry &other ) : mRestoreInfo( other.mRestoreInfo ), mText( other.mText )
    { 
    }

    FmRestoreEntry &operator =( const FmRestoreEntry &other )
    {
        if( this == &other ){
            return *this;
        }

        mText = other.mText;
        mRestoreInfo = other.mRestoreInfo;
        return *this;
    } 

    QString text() const
    {
        return mText;
    }

    FmRestoreInfo restoreInfo() const
    {
        return mRestoreInfo;
    }

private:
    QString         mText;  /// display text
    FmRestoreInfo   mRestoreInfo;  /// info 
};

// CLASS DECLARATION
/**
 *  This class is used for storing resore settings which used in restore view
 */
class FMBKUPENGINE_EXPORT FmRestoreSettings
{
public:

public:
    explicit FmRestoreSettings( FmBkupEngine& );
    ~FmRestoreSettings( void );

    /*
     * load and refresh data to entry list
     */
    void load( QList<FmBkupDrivesAndOperation* > drivesAndOperationList );

    /**
     * get restore entry list, please invoke load() first.
     * @return restore entry list
     */
    QList< FmRestoreEntry* > restoreEntryList();

    void GetSelectionL( QList< FmRestoreInfo >& infoArray ) const;
    void SetSelection( const quint64& aSelection );
    
private:
    /**
     * create restore entry that will be used in restore view.
     */
    FmRestoreEntry* CreateEntry( const FmRestoreInfo &info );

    /**
     * delete memory in the entry list
     */
    void resetAndDestoryRestoreEntry();

    /*
     * refresh data to entry list
     */
    void refreshList( QList<FmBkupDrivesAndOperation* > &drivesAndOperationList );

private:
    /**
     * Restore selection
     */
    quint32 iSelection;

    /**
     * Restore setting list items
     */
    QList< FmRestoreEntry* > mRestoreEntryList;

    FmBkupEngine& mEngine;
};

#endif //FMRESTORESETTINGS_H
