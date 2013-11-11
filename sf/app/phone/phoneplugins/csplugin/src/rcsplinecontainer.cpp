/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for ETel line subsessions.
*
*/



//  INCLUDES
#include    <rmmcustomapi.h>
#include    <mmtsy_names.h>  // tsy and phone names
#include    <cccpcallparameters.h>
#include    <cccecallparameters.h>

#include    "rcsplinecontainer.h"
#include    "csppanic.pan"
#include    "csplogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// Default c++ constructor
RCSPLineContainer::RCSPLineContainer()
    {
    }

// ---------------------------------------------------------
// RCSPLineContainer::Open
//
// ---------------------------------------------------------
//
TInt RCSPLineContainer::Open( RMobilePhone& aPhone, TUint32 aServiceId )
    {
    CSPLOGSTRING( CSPINT, "RCSPLineContainer::Open" );
    iServiceId = aServiceId;
    
    TInt error = iPrimaryLine.Open( aPhone, KMmTsyVoice1LineName);
    if (error != KErrNone)
        {
        CSPLOGSTRING2( CSPINT, "RCSPLineContainer::Open voice ERROR %d", error );
        return error;
        }

    // Ignore errors on the rest of the lines - they are non-essential
    error = iAls2Line.Open( aPhone, KMmTsyVoice2LineName );
    if ( error )
        {
        CSPLOGSTRING2( CSPINT, "RCSPLineContainer::Open voice2 ERROR %d", error );
        }
        
    error = iDataLine.Open( aPhone, KMmTsyDataLineName );
    if ( error )
        {
        CSPLOGSTRING2( CSPINT, "RCSPLineContainer::Open data ERROR %d", error );
        }
        
    error = iFaxLine.Open( aPhone, KMmTsyFaxLineName );
    if ( error )
        {
        CSPLOGSTRING2( CSPINT, "RCSPLineContainer::Open fax ERROR %d", error );
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// RCSPLineContainer::Close
//
// ---------------------------------------------------------
//
void RCSPLineContainer::Close()
    {
    CSPLOGSTRING( CSPINT, "RCSPLineContainer::Close" );
    iPrimaryLine.Close();
    iAls2Line.Close();
    iDataLine.Close(); 
    iFaxLine.Close(); 
    }

// ---------------------------------------------------------
// RCSPLineContainer::LineByType
//
// ---------------------------------------------------------
//
RMobileLine& RCSPLineContainer::LineByType( TCSPLineId aLine )
    {
    CSPLOGSTRING2( CSPINT, "RCSPLineContainer::LineByType %d", aLine );    
    switch(aLine)
        {
        case ECSPLineSpeech:
            CSPLOGSTRING2( CSPINT, "RCSPLineContainer::LineByType Speech %d", aLine );
            return iPrimaryLine;
        case ECSPLineAuxSpeech:
            CSPLOGSTRING2( CSPINT, "RCSPLineContainer::LineByType AuxSpeech %d", aLine );
            return iAls2Line;
        case ECSPLineData:
            CSPLOGSTRING2( CSPINT, "RCSPLineContainer::LineByType Data %d", aLine );
            return iDataLine;
        case ECSPLineFax:
            CSPLOGSTRING2( CSPINT, "RCSPLineContainer::LineByType Fax %d", aLine );
            return iFaxLine;
        default:
            CSPLOGSTRING2( CSPINT, "RCSPLineContainer::INVALID LINE ID %d", aLine );
            Panic( ECSPPanicInvalidParameter );
            return iPrimaryLine;
        }
    }

// ---------------------------------------------------------
// True if the line matching the line id is open.
// ---------------------------------------------------------
// 
TBool RCSPLineContainer::LineIsOpen( TCSPLineId aType )
    {
    CSPLOGSTRING2( CSPINT, "RCSPLineContainer::LineIsOpen %d", aType );
    RMobileLine& line = LineByType(aType);
    return (line.SubSessionHandle() != KNullHandle);
    }

// ---------------------------------------------------------
// Resolves line id based on service id, call type and line 
// type.
// ---------------------------------------------------------
// 
RCSPLineContainer::TCSPLineId RCSPLineContainer::ResolveLineIdL( 
            const CCCECallParameters& aCallParameters ) const
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::ResolveLineIdL");
    RCSPLineContainer::TCSPLineId lineId = RCSPLineContainer::ECSPLineSpeech;
    
    switch ( aCallParameters.CallType() )
        {
        case CCPCall::ECallTypeCSVoice:
            {
            if ( aCallParameters.LineType() == CCCECallParameters::ECCELineTypeAux )
                {
                lineId = RCSPLineContainer::ECSPLineAuxSpeech;
                }
            else if ( aCallParameters.LineType() == CCCECallParameters::ECCELineTypePrimary )
                {
                lineId = RCSPLineContainer::ECSPLineSpeech;
                }
            else
                {
                CSPLOGSTRING(CSPOBJECT, "CSProvider::ResolveLineIdL ERROR: Unknown line type.");
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case CCPCall::ECallTypeVideo: // Fall through
            {
            lineId = RCSPLineContainer::ECSPLineData;
            break;
            }
  
        default:
            {
            CSPLOGSTRING(CSPOBJECT, "CSProvider::ResolveLineIdL ERROR: Unknown call type");
            User::Leave( KErrNotSupported );
            }
        }
        
    return lineId;
    }
    
// ---------------------------------------------------------
// Resolves call type, service id and line type based on 
// line id.
// ---------------------------------------------------------
// 
TInt RCSPLineContainer::ResolveCallInfo( RCSPLineContainer::TCSPLineId aLineId, 
                                  TUint32& aServiceId,
                                  CCPCall::TCallType& aCallType,
                                  CCCECallParameters::TCCELineType& aLineType ) const
                                  
    {
    TInt err(KErrNone);
    aLineType = CCCECallParameters::ECCELineTypePrimary;    
    aServiceId = iServiceId;
    
    if ( aLineId == RCSPLineContainer::ECSPLineSpeech )
        {
        aCallType = CCPCall::ECallTypeCSVoice;            
        }
    else if ( aLineId == RCSPLineContainer::ECSPLineAuxSpeech )
        {
        aCallType = CCPCall::ECallTypeCSVoice;
        aLineType = CCCECallParameters::ECCELineTypeAux;
        }
    else if ( aLineId == RCSPLineContainer::ECSPLineData )
        {
        aCallType = CCPCall::ECallTypeVideo;
        }
    else
        {
        CSPLOGSTRING(CSPOBJECT, "RCSPLineContainer::ResolveCallInfo() ERROR Unknown line id");
        err = KErrArgument;
        }
    CSPLOGSTRING2(CSPINT, "RCSPLineContainer::ResolveCallInfo() aCallType=%d", aCallType );
    CSPLOGSTRING2(CSPINT, "RCSPLineContainer::ResolveCallInfo() aLineType=%d", aLineType );
    
    return err;
    }


//  End of File  

