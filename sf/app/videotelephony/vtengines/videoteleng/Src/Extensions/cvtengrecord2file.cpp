/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Videoteleng  Record to file extension 
*
*/


// INCLUDE FILES
#include    "cvtengrecord2file.h"
#include    "VtEngPanic.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include 	"cvtengr2ftoneplayer.h"

#include    <CVtLogger.h>

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CVtEngRecord2File::CVtEngRecord2File
// C++ constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CVtEngRecord2File::CVtEngRecord2File():
                        CVtEngExtensionBase( 
                        KVtEngExtensionRecord2File )
    {
    }

// ----------------------------------------------------------------------------
// CVtEngRecord2File::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVtEngRecord2File::ConstructL()
    {
    __VTPRINTENTER( "CVtEngRecord2File.ConstructL" )
	
	iTonePlayer = CVtEngR2FTonePlayer::NewL();
    __VTPRINTEXIT( "CVtEngRecord2File.ConstructL" )
    }

// ----------------------------------------------------------------------------
// CVtEngRecord2File::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVtEngRecord2File* CVtEngRecord2File::NewL()
    {
    CVtEngRecord2File* self = new( ELeave ) CVtEngRecord2File;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CVtEngRecord2File::~CVtEngRecord2File
// Destructor. Cannot leave.
// ----------------------------------------------------------------------------
//
CVtEngRecord2File::~CVtEngRecord2File()
    {
    
    __VTPRINTENTER( "CVtEngRecord2File.~" )
    if ( iTonePlayer )
        {
        iTonePlayer->StopSound();
        delete iTonePlayer;
        }
    __VTPRINTEXIT( "CVtEngRecord2File.~" )
    }
// ----------------------------------------------------------------------------
// CVtEngRecord2File::GetInterface
// 
// ----------------------------------------------------------------------------
//    
TAny* CVtEngRecord2File::GetInterface() 
    {
    __VTPRINTENTER( "CVtEngRecord2File.GetInterface")
    // Cast first as API MVtEngRecord2File* class
    // and after that to TAny*
    __VTPRINTEXIT( "CVtEngRecord2File.GetInterface" )  
    return reinterpret_cast< TAny* >( ( MVtEngRecord2File* ) this );

    }

// ----------------------------------------------------------------------------
// CVtEngRecord2File::HandleL
// 
// ----------------------------------------------------------------------------
//    
TBool CVtEngRecord2File::HandleL( CVtEngOperation* aOp )
    {
    __VTPRINTENTER( "CVtEngRecord2File.HandleL" )
    const TVtEngCommandId id( aOp->Command() );
    __VTPRINT2( DEBUG_GEN, "CVtEngRecord2File.HandleL CMD ID[%d] ", id )
    
    // Check which command and call
    // active provider with operation params    
    switch ( id )
        {
        case KVtEngStartRecord:    
            {          
            
            iTonePlayer->PlaySound( CVtEngR2FTonePlayer::EVtR2FVideoStartSoundId ); 
            break;
            }
        case KVtEngStopRecord:    
            {          
            
            iTonePlayer->PlaySound( CVtEngR2FTonePlayer::EVtR2FVideoStopSoundId ); 
            break;
            }
        //Not supported command    
        default:
            __VTPRINTEXITR( 
            "CVtEngRecord2File.HandleL NOT SUPPORTED CMD[%d]", id )
            return EFalse;
        }
    __VTPRINTEXIT( "CVtEngRecord2File.HandleL" )  
    return ETrue;
    }        

// ----------------------------------------------------------------------------
// CVtEngRecord2File::ValidateCommand
// 
// ----------------------------------------------------------------------------

TBool CVtEngRecord2File::ValidateCommand( 
	const TVtEngCommandId aCommandId )
    {
    __VTPRINTENTER( "CVtEngRecord2File.ValidateCommand" )
    
    // local or hw support 
    // set as False
    TBool support = EFalse;
    
    // Check which command and check if
    // if provider and local variation supports
     __VTPRINT2( DEBUG_GEN, 
     	"CVtEngRecord2File.ValidateCommand CMD ID[%d] ", aCommandId ) 	
    switch ( aCommandId )
        {
        case KVtEngStartRecord:
	        {
	        support = ETrue;
            break;
	        }
        
        case KVtEngStopRecord:
	        {
	        support = ETrue;
            break;
	        }
        // Not supported command    
        default:
        	{
            __VTPRINT2( DEBUG_GEN, 
            	"CVtEngRecord2File.ValidateCommand NOT SUPPORTED CMD[%d]",
            	 aCommandId)
            support = EFalse;	 
        	}
        }
    __VTPRINTEXITR( "CVtEngRecord2File.ValidateCommand return value[%d]",
    	support)                   
    return support; 
    }          
    
// ----------------------------------------------------------------------------
// CVtEngRecord2File::IsTonePlaySupported
// FOR TESTING PURPOSES REMOVE!
// ----------------------------------------------------------------------------            

TInt CVtEngRecord2File::IsTonePlaySupported( TBool& aSupported) const
	{
	if (iTonePlayer)
		{
		aSupported = ETrue;
		}
	else
		{
		aSupported = EFalse;
		}
	__VTPRINT2( DEBUG_GEN, "CVtEngRecord2File.IsTonePlaySupported bool[%d] ", (TInt)aSupported ) 		
	return KErrNone;	
	}

//  End of File
