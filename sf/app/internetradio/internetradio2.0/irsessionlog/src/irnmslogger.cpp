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
* Description:  The implementation for presentation elements.
*
*/


#include <s32strm.h>

#include "irdebug.h"
#include "irreportsettings.h"
#include "irnmslogger.h"

//const TInt KMaxUrlLength = 256

// ============================== Function declaration =======================

// ---------------------------------------------------------------------------
// Function: NewL
// Two phase constructor returns the instance of CIRNmsLogger
// ---------------------------------------------------------------------------
//
CIRNmsLogger* CIRNmsLogger::NewL()
    {
    IRLOG_DEBUG( "CIRNmsLogger::NewL" );
    CIRNmsLogger* self = CIRNmsLogger::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRNmsLogger::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// Function: NewLC
// Two phase constructor creates the instance of CIRNmsLogger
// ---------------------------------------------------------------------------
//
CIRNmsLogger* CIRNmsLogger::NewLC()
    {
    IRLOG_DEBUG( "CIRNmsLogger::NewLC" );
    CIRNmsLogger* self = new ( ELeave ) CIRNmsLogger;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRNmsLogger::NewLC - Exiting." );
    return self;
    }    

// ---------------------------------------------------------------------------
// Function: ~CIRNmsLogger
// default destructor
// ---------------------------------------------------------------------------
//
CIRNmsLogger::~CIRNmsLogger()    
    {
    }

// ---------------------------------------------------------------------------
// Function: ConstructL
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::ConstructL()        
    {
    //No implementation
    //return;
    }

// ---------------------------------------------------------------------------
// Function: CIRNmsLogger
// default constructor
// ---------------------------------------------------------------------------
//
CIRNmsLogger::CIRNmsLogger()    
    {
    //No implementation
    }

    
// ---------------------------------------------------------------------------
// Function: ResetResource
// reset data from channel server
// ---------------------------------------------------------------------------
//    
void CIRNmsLogger::ResetResource()
    {
    IRLOG_DEBUG( "CIRNmsLogger::ResetResource" );
    //reseting of resource
    }


    
// ---------------------------------------------------------------------------
// Function: UpdateChannelID
// updates the channel id
// ---------------------------------------------------------------------------
//    
void CIRNmsLogger::UpdateChannelID( TInt aChanneldID )
    {
    IRLOG_DEBUG( "CIRNmsLogger::UpdateChannelID" );
    iChannelId = aChanneldID;    
    }

// ---------------------------------------------------------------------------
// Function: UpdateCurrentNetwork
// updates the current network
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::UpdateCurrentNetwork( TInt aCurrentNetwork )
    {
    IRLOG_DEBUG( "CIRNmsLogger::UpdateCurrentNetwork" );
    iNmsCurrentNetwork = aCurrentNetwork;
    }

// ---------------------------------------------------------------------------
// Function: UpdateHomeOperator
// updates the home network
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::UpdateHomeOperator( TInt aHomeOperator )
    {
    IRLOG_DEBUG( "CIRNmsLogger::UpdateHomeOperator" );
    iHomeOperator = aHomeOperator;
    }

// ---------------------------------------------------------------------------
// Function : UpdateNmsStartGMTTime
// function updates Nms start time with current GMT time
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::UpdateNmsStartGMTTime()
    {
    iStartNmsTime.UniversalTime();    
    }
// ---------------------------------------------------------------------------
// CIRNmsLogger: UpdateNmsType
// updates the Nms type
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::UpdateNmsType( const TDesC& aNmsType )
    {
    IRLOG_DEBUG( "CIRNmsLogger::UpdateNmsType" );
    iNmsType.Copy( aNmsType );
    }

// ---------------------------------------------------------------------------
// Function: ChannelID
// returns the channel id
// ---------------------------------------------------------------------------
//    
TInt CIRNmsLogger::ChannelID() const
    {
    IRLOG_DEBUG( "CIRNmsLogger::ChannelID" );
    return iChannelId;
    }

// ---------------------------------------------------------------------------
// Function: CurrentNetwork
// returns the Current Network
// ---------------------------------------------------------------------------
//    
TInt CIRNmsLogger::CurrentNetwork() const
    {
    IRLOG_DEBUG( "CIRNmsLogger::CurrentNetwork" );
    return iNmsCurrentNetwork;
    }

// ---------------------------------------------------------------------------
// Function: HomeOperator
// returns the home network
// ---------------------------------------------------------------------------
//
TInt CIRNmsLogger::HomeOperator() const
    {
    IRLOG_DEBUG( "CIRNmsLogger::HomeOperator" );
    return iHomeOperator;
    }


// ---------------------------------------------------------------------------
// Function: StartTime
// returns the start time
// ---------------------------------------------------------------------------
//    
TDes& CIRNmsLogger::StartTime()
    {
    IRLOG_DEBUG( "CIRNmsLogger::StartTime" );
    return iStartTime;    
    }
// ---------------------------------------------------------------------------
// Function: NmsLogId
// returns the NmsLog id
// ---------------------------------------------------------------------------
//    
TInt CIRNmsLogger::NmsLogId() const
    {
    IRLOG_DEBUG( "CIRSessionLogger::NmsLogId" );
    return iNmsLogId;
    }
// ---------------------------------------------------------------------------
// Function: NmsType
// returns the Nms type
// ---------------------------------------------------------------------------
//    
TDesC& CIRNmsLogger::NmsType()
    {
    IRLOG_DEBUG( "CIRNmsLogger::ConnectionType" );
    return iNmsType;
    }

// ---------------------------------------------------------------------------
// Function: FileName
// returns file name
// ---------------------------------------------------------------------------
//
TFileName CIRNmsLogger::FileName()
    {
    IRLOG_DEBUG( "CIRNmsLogger::FileName" );
    return iFilePath;    
    }

// ---------------------------------------------------------------------------
// Function: DateTime
// returns Nms start time
// ---------------------------------------------------------------------------
//
TTime CIRNmsLogger::DateTime()
    {
    return iStartNmsTime;    
    }
    

    
// ---------------------------------------------------------------------------
// Function: GetBackedUp
// Back up the Nms log of a Nms
// ---------------------------------------------------------------------------
//    
void CIRNmsLogger::GetBackedUp( CIRNmsLogger& aNms )
    {
    //Nms start time
    iStartNmsTime = aNms.DateTime();
    //start time 
    iStartTime.Copy( aNms.StartTime() );
    //channel id
    iChannelId = aNms.ChannelID();
    //home operator
    iHomeOperator = aNms.HomeOperator();
    //current network
    iNmsCurrentNetwork = aNms.CurrentNetwork();
    // Newly added    
    ResetResource();
    }
            
// ---------------------------------------------------------------------------
// Function: ExternalizeL
// externialize the stream to the given stream
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::ExternalizeL( RWriteStream& aWriteStream )
    {
    IRLOG_DEBUG( "CIRNmsLogger::ExternalizeL" );
    TInt length;
    //start time
    length = iStartTime.Length();
    aWriteStream.WriteInt32L( length );
    aWriteStream.WriteL( iStartTime, length );
    
    //channel id
    aWriteStream.WriteInt32L( iChannelId );

    //Type
    length = iNmsType.Length();
    aWriteStream.WriteInt32L( length );
    aWriteStream.WriteL( iNmsType, length );
    
    //current network
    aWriteStream.WriteInt32L( iNmsCurrentNetwork );
    
    //home operator
    aWriteStream.WriteInt32L( iHomeOperator );

    IRLOG_DEBUG( "CIRNmsLogger::ExternalizeL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// Function: InternalizeL
// gets the data from the stream
// ---------------------------------------------------------------------------
//
void CIRNmsLogger::InternalizeL( RReadStream& aReadStream )
    {
    IRLOG_DEBUG( "CIRNmsLogger::InternalizeL" );
    TInt length;
    
    //Start Time
    length = aReadStream.ReadInt32L();
    aReadStream.ReadL( iStartTime, length );
    
    
    //Channel Id
    iChannelId = aReadStream.ReadInt32L();

    //Nms Type
    length = aReadStream.ReadInt32L();
    aReadStream.ReadL( iNmsType, length );

    //Nms Current Network
    iNmsCurrentNetwork = aReadStream.ReadInt32L();
    
    //Home Operator
    iHomeOperator = aReadStream.ReadInt32L();
            
    IRLOG_DEBUG( "CIRNmsLogger::InternalizeL - Exiting." );
    }
