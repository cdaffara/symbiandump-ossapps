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
* Description:
*
*/

#include "MemSpySettings.h"

// System includes
#include <e32svr.h>
#include <s32file.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginelogger.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>

#include <memspysession.h>

// Constants
_LIT( KMemSpySettingsFileName, "settings.dat" ); //file or avkon client settings

// Version 03 dumped some of the system wide memory tracker settings
const TInt KMemSpySettingsFileFormatVersion = 6;

CMemSpySettings::CMemSpySettings( RFs& aFsSession, RMemSpySession& aSession )
:   iFsSession( aFsSession ), iMemSpySession( aSession )
    {	
    }


CMemSpySettings::~CMemSpySettings()
    {	
    TRACE( RDebug::Printf( "CMemSpySettings::~CMemSpySettings() - START" ) );
    TRAP_IGNORE( StoreSettingsL() );
    TRACE( RDebug::Printf( "CMemSpySettings::~CMemSpySettings() - END" ) );    	
    }


void CMemSpySettings::ConstructL()
    {	
    TRACE( RDebug::Print( _L("CMemSpySettings::ConstructL() - START") ) );

    TRAP_IGNORE( RestoreSettingsL() );

    TRACE( RDebug::Print( _L("CMemSpySettings::ConstructL() - END") ) );    
    }

CMemSpySettings* CMemSpySettings::NewL( RFs& aFsSession, RMemSpySession& aSession )
    {
    CMemSpySettings* self = new(ELeave) CMemSpySettings( aFsSession, aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CMemSpySettings::GetSettingsFileNameL( TDes& aFileName )
    {
    GetSettingsPathL( aFileName );
    aFileName.Append( KMemSpySettingsFileName );
    TRACE( RDebug::Print( _L("CMemSpySettings::GetSettingsFileNameL() - aFileName: %S"), &aFileName ) );
    }


void CMemSpySettings::GetSettingsPathL( TDes& aPath )
    {
    TRACE( RDebug::Print( _L("CMemSpySettings::GetSettingsPathL() - START") ) );
    aPath.Zero();

    // Get private data cage path
    TInt err = iFsSession.PrivatePath( aPath );
    TRACE( RDebug::Print( _L("CMemSpySettings::GetSettingsPathL() - priv path err: %d"), err ) );
    User::LeaveIfError( err );

    // Combine with C: drive
    const TDriveUnit cDrive( EDriveC );
    const TDriveName cDriveName( cDrive.Name() );
    aPath.Insert( 0, cDriveName );

    iFsSession.MkDirAll( aPath );
    TRACE( RDebug::Print( _L("CMemSpySettings::GetSettingsPathL() - END - %S"), &aPath ) );
    }

RFile CMemSpySettings::SettingsFileLC( TBool aReplace )
    {
    TRACE( RDebug::Print( _L("CMemSpySettings::SettingsFileLC() - START - aReplace: %d"), aReplace ) );

    TFileName* fileName = new(ELeave) TFileName();
    CleanupStack::PushL( fileName );
    GetSettingsFileNameL( *fileName );
    TRACE( RDebug::Print( _L("CMemSpySettings::SettingsFileLC() - fileName: %S"), fileName ) );

    RFile file;
    TInt error = KErrNone;
    //
    if  ( aReplace )
        {
        error = file.Replace( iFsSession, *fileName, EFileWrite );
        TRACE( RDebug::Print( _L("CMemSpySettings::SettingsFileLC() - replace err: %d"), error ) );
        }
    else
        {
        error = file.Open( iFsSession, *fileName, EFileWrite );
        TRACE( RDebug::Print( _L("CMemSpySettings::SettingsFileLC() - open err: %d"), error ) );
        //
        if  ( error == KErrNotFound )
            {
            error = file.Create( iFsSession, *fileName, EFileWrite );
            }
        }
    //
    User::LeaveIfError( error );
    CleanupStack::PopAndDestroy( fileName );
    CleanupClosePushL( file );
    //
    TRACE( RDebug::Print( _L("CMemSpySettings::SettingsFileLC() - END") ) );
    return file;
    }


void CMemSpySettings::StoreSettingsL()
    {
    TRACE( RDebug::Printf( "CMemSpySettings::StoreSettingsL() - START" ) );

    RFile file = SettingsFileLC( ETrue );
    RFileWriteStream stream( file );
    CleanupStack::Pop(); // file
    CleanupClosePushL( stream );
    
    // Verion info
    stream.WriteInt32L( KMemSpySettingsFileFormatVersion );
        
    iMemSpySession.GetOutputSink( iSinkType );
    stream.WriteUint8L( iSinkType );
        
    // Get SWMT config    
    GetSwmtConfig( iSwmtConfig );
    
    stream.WriteInt32L( iSwmtConfig.iTimerPeriod.Int() );
    stream.WriteUint8L( iSwmtConfig.iDumpData );
    
    // Write memory tracking auto-start process list
    /*
    const RArray<TUid>& processUidList = iEngine.HelperProcess().MemoryTrackingAutoStartProcessList();
    stream.WriteInt32L( processUidList.Count() );
    for( TInt i=0; i<processUidList.Count(); i++ )
        {
        const TUid uid = processUidList[ i ];
        TRACE( RDebug::Printf( "CMemSpySettings::StoreSettingsL() - process tracker uid[%02d]: 0x%08x", i, uid.iUid ) );
        stream << uid;
        }
    */        
    
    // Write memory tracking categories    
    stream.WriteInt32L( iSwmtConfig.iEnabledCategories );
    
    // Write heap tracking thread name filter    
    stream.WriteInt32L( iSwmtConfig.iThreadNameFilter.Length() );
        
    if ( iSwmtConfig.iThreadNameFilter.Length() > 0 )
    	{
		stream.WriteL( iSwmtConfig.iThreadNameFilter, iSwmtConfig.iThreadNameFilter.Length() );
    	}
        
    // Write mode
    stream.WriteInt32L( iSwmtConfig.iMode );
    
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream ); // Closes file    
    }

void CMemSpySettings::RestoreSettingsL()
    {  
    RFile file = SettingsFileLC();
    RFileReadStream stream( file );
    CleanupStack::Pop(); // file
    CleanupClosePushL( stream ); 
    
    // Version info
    const TInt version = stream.ReadInt32L(); // discarded for now
    TRACE( RDebug::Printf( "CMemSpySettings::RestoreSettingsL() - version: %d", version ) );

    // Engine settings
    TMemSpySinkType type = static_cast< TMemSpySinkType >( stream.ReadUint8L() );
    TRACE( RDebug::Printf( "CMemSpySettings::RestoreSettingsL() - read sinkType: %d", type ) );
           
    iMemSpySession.SwitchOutputSinkL( type ); //set value in engine
    
    // Set SWMT config
    TMemSpyEngineHelperSysMemTrackerConfig swmtConfig;
    swmtConfig.iTimerPeriod = TTimeIntervalMicroSeconds32( stream.ReadInt32L() ); 
    swmtConfig.iDumpData = static_cast< TBool >( stream.ReadUint8L() );

    if  ( version < 3 )
        {
        // Restore but ignore old delta tracker settings which aren't used anymore
        //
        static_cast< TBool >( stream.ReadUint8L() );
        static_cast< TBool >( stream.ReadUint8L() );
        static_cast< TBool >( stream.ReadUint8L() );
        }
    
    // Restore memory tracking auto-start process uids if file format supports it...
    if ( version >= 2 )
        {
		/*
        RArray<TUid> list;		
        CleanupClosePushL( list );
        //
        const TInt count = stream.ReadInt32L();
        
        CArrayFixFlat<TUid>* list = new(ELeave)CArrayFixFlat<TUid>(count);
        CleanupStack::PushL(list );               
        
        for( TInt i=0; i<count; i++ )
            {
            TUid processUid;
            stream >> processUid;
            //           
            User::LeaveIfError( list.Append( processUid ) );            
            //list->AppendL( processUid );
            }
        //
        //CMemSpyEngineHelperProcess& processHelper = iEngine.HelperProcess();
        //processHelper.SetMemoryTrackingAutoStartProcessListL( list );        
        //iMemSpySession.SetSwmtAutoStartProcessList( list );
        iUidList = list; 	//TODO: to get it into the engine
        
        CleanupStack::PopAndDestroy( &list );
        */
        }
    
    // Restore memory tracking categories 
    if ( version > 3 )
        {
        swmtConfig.iEnabledCategories = stream.ReadInt32L();
        }
    
    // Write heap tracking thread name filter 
    if ( version > 4 )
        {
        TInt len = stream.ReadInt32L();
        if ( len > 0 && len < KMaxName )
            {			
            stream.ReadL( swmtConfig.iThreadNameFilter, len );
            }        
        }

    // Write mode
    if ( version > 5 )
        {
        swmtConfig.iMode = (TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode)stream.ReadInt32L();
        }
        
    iSwmtConfig = swmtConfig;
    
    SetSwmtConfig( iSwmtConfig ); //set values in engine
    
    CleanupStack::PopAndDestroy( &stream ); // Closes file    
    }

void CMemSpySettings::GetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
	{
	TInt categories = 0;
	iMemSpySession.GetSwmtCategoriesL( categories );
	aConfig.iEnabledCategories = categories;
		
	TInt timerPeriod = 0;
	iMemSpySession.GetSwmtTimerIntervalL( timerPeriod );
	aConfig.iTimerPeriod = timerPeriod;
		
	TBool dumpData = EFalse;
	iMemSpySession.GetSwmtHeapDumpsEnabledL( dumpData );
	aConfig.iDumpData = dumpData;
			     
	TName threadNameFilter;
	iMemSpySession.GetSwmtFilter( threadNameFilter );
	aConfig.iThreadNameFilter = threadNameFilter;
			
	TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode mode;
	iMemSpySession.GetSwmtMode( mode );
	aConfig.iMode = mode;	    
	}

void CMemSpySettings::SetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
	{
	iMemSpySession.SetSwmtMode( aConfig.iMode );
	iMemSpySession.SetSwmtCategoriesL( aConfig.iEnabledCategories );
	iMemSpySession.SetSwmtTimerIntervalL( aConfig.iTimerPeriod.Int() );
	iMemSpySession.SetSwmtHeapDumpsEnabledL( aConfig.iDumpData );
	iMemSpySession.SetSwmtFilter( aConfig.iThreadNameFilter );
	}
