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


#ifndef C_IRNMSLOGGER_H
#define C_IRNMSLOGGER_H

#include <e32base.h>

#include "irsessionparams.h"

class RReadStream;
class RWriteStream;

/**
* This class can hold data for Nms log 
*/

NONSHARABLE_CLASS( CIRNmsLogger ) : public CBase
    {
public:
    /**
    * Function: NewL
    * Two phase constructor returns the instance of CIRNmsLogger
    * @returns instance of CIRNmsLogger
    */
    static CIRNmsLogger* NewL();

    /**
    * Function: NewLC
    * Two phase constructor creates the instance of CIRNmsLogger
    * @returns instance of CIRNmsLogger
    */
    static CIRNmsLogger* NewLC(); 

    /**
    * Function: ~CIRNmsLogger
    * default destructor
    */    
    ~CIRNmsLogger();
    
private:
    /**
    * Function: ConstructL
    * Two phase constructor
    */    
    void ConstructL();
    
    /**
    * Function: CIRNmsLogger
    * default constructor
    */    
    CIRNmsLogger();    

public:
    

    /**
    * Function: ResetResource
    * reset data from channel server
    */
    void ResetResource();


    /**
    * Function: UpdateChannelID
    * updates the channel id
    * @param channel id
    */    
    void UpdateChannelID( TInt aChanneldID );
    
    /**
    * Function: UpdateCurrentNetwork
    * updates the current network
    * @param current network
    */
    void UpdateCurrentNetwork( TInt aCurrentNetwork );
    
    /**
    * Function: UpdateHomeOperator
    * updates the home network
    * @param home network
    */
    void UpdateHomeOperator( TInt aHomeOperator );

    /**
    * Function : UpdateNmsStartGMTTime
    * function updates Nms start time with current GMT time
    */
    void UpdateNmsStartGMTTime();
    
    /**
    * Function: UpdateNmsType
    * updates the nms type
    * @param nms type
    */
    void UpdateNmsType( const TDesC& aNmsType );

    /**    
    * Function: ChannelID
    * return the channel id
    * @return channel id
    */    
    TInt ChannelID() const;
    
    /**
    * Function: CurrentNetwork
    * returns the Current Network
    * @return Current Network
    */    
    TInt CurrentNetwork() const;
    
    /**
    * Function: HomeOperator
    * returns the home network
    * @return home network
    */
    TInt HomeOperator() const;
    
    
    /**
    * Function: StartTime
    * returns the start time
    * @return start time
    */    
    TDes& StartTime();
    
    /**
    * Function: NmsType
    * returns the nms type
    * @return nms type
    */    
    TDesC& NmsType();

    /**
    * Function: NmsLogId
    * returns the nmslog id
    * @return n nmslog id
    */    
    
    TInt NmsLogId() const;
    
    
    /**
    * Function: FileName
    * returns file name
    * @return file name
    */
    TFileName FileName();
    
    /** 
    * Function: DateTime
    * returns Nms start time
    * @return Nms start time
    */
    TTime DateTime();
    
    /**
    * Function: GetBackedUp
    * Back up the given Nms, copies the data of the Nms
    * @param Nms logs instance which to be copied
    */
    void GetBackedUp( CIRNmsLogger& aNms );
    
    /**
    * Function: ExternalizeL
    * externialize the stream to the given stream
    * @param write stream
    */
    void ExternalizeL( RWriteStream& aWriteStream );
    
    /**
    * Function: InternalizeL
    * gets the data from the stream
    * @param read stream
    */
    void InternalizeL( RReadStream& aReadStream );
    

private:  
    /**                                    
    * file name 
    */                                    
    TFileName iFilePath;
    
    /**
    * Nms start time
    */
    TTime iStartNmsTime;
    
    /**
    * time current Nms become active started
    */
    TTime iActiveNmsTime;
    
    
    /**
    * start time is stored
    */
    TBuf<64> iStartTime;
    

    /**
    * NmsLog id
    */
    TInt iNmsLogId;
    
    /**
    * channel id
    */
    TInt iChannelId;
    
    /**
    * home operator mcc+mnc
    */
    TInt iHomeOperator;
    
    /**
    * current operator mcc+mnc
    */
    TInt iNmsCurrentNetwork;
    
    /**
    * nms type information    
    */
	TBuf<16> iNmsType;
    };

#endif //C_IRNMSLOGGER_H
