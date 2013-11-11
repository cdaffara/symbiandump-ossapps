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
* Description:  Videoteleng Camera Preferences extension 
*
*/


#ifndef CVTENGRECORD2FILE_H
#define CVTENGRECORD2FILE_H

//  INCLUDES
#include "VtEngPanic.h"
#include "CVtEngSettings.h"
#include "VtEngUtils.h"
#include "CVtEngOperation.h"

#include "cvtengr2ftoneplayer.h"

#include <cvtlogger.h>
#include <e32base.h>

#include <mvtengrecord2file.h>
#include "CVtEngExtensionBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CVtEngR2FTonePlayer;
/**
*  Videoteleng extension base class.
*
*  @lib videoteleng.lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS ( CVtEngRecord2File ): public CVtEngExtensionBase,
											 public MVtEngRecord2File
    {
    public: //Constructor and destructor.
        
        /**
        * Symbian two-phase constructor.
        * @return engine extension instance
        */
        static CVtEngRecord2File* NewL();

        /**
        * Destructor.
        */
        ~CVtEngRecord2File();
        
    public: // from base class CVtEngExtensionBase
        
       	/**
        * @see CVtEngExtensionBase::GetInterface
        */
        virtual TAny* GetInterface();
        
       	/**
        * @see CVtEngExtensionBase::HandleL
        */
        virtual TBool HandleL( CVtEngOperation* aOp );
        
        /**
        * @see CVtEngExtensionBase::ValidateCommand
        */
        virtual TBool ValidateCommand( const TVtEngCommandId aCommandId );
        
    public: // from base class MVtEngRecord2File
    
    	/**
    	* For testin puposes only
        * @see MVtEngRecord2File::IsTonePlaySupported
        */  
		virtual TInt IsTonePlaySupported( TBool& aSupported) const;
           
        
    private:
    
        /**
        * C++ default constructor.
        */        
        CVtEngRecord2File();
        
        /**
        * Symbian two-phase constructor.
        */
        void ConstructL();
    
    public: // data
    
    private: // data
        CVtEngR2FTonePlayer* iTonePlayer;
    };
        
#endif //CVTENGRECORD2FILE_H    