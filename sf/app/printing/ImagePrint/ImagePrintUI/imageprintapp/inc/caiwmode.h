/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CAIWMODE_H
#define CAIWMODE_H

#include <e32def.h>
#include <e32base.h>

#include "muimode.h"
#include "mprotocolobserver.h"
#include "muimodeobserver.h"

class CUIProtocolController;
class CIFFactory;

//  CLASS DEFINITION
/**
 *
 * This class represents the embedded UI mode,
 * i.e. aiw-initilized mode
 *
 */
class CAiwMode : public CBase,
                 public MUIMode,
                 public MProtocolObserver
                 
    {
    public: // Constructors and destructors
                
		/**
         * Two-phase constructor
         * 
         * @param aUIModeObserver Ui mode observer
         * @param aIFFactory      Interface factory to be used
         *        
         * @return Initialized instance of the class
         */                
        static CAiwMode* NewL( MUIModeObserver* aUIModeObserver,
                               CIFFactory& aIFFactory );

		/**
         * Two-phase constructor
         * 
         * @param aUIModeObserver Ui mode observer
         * @param aIFFactory      Interface factory to be used
         *        
         * @return Initialized instance of the class
         */                
        static CAiwMode* NewLC( MUIModeObserver* aUIModeObserver,
                                CIFFactory& aIFFactory );
    
    protected:
    
       virtual ~CAiwMode();
    
    private:
    	
        CAiwMode( MUIModeObserver* aUIModeObserver );
    	
     	void ConstructL( CIFFactory& aIFFactory );   
     	
    public: // Methods derived from MUIMode    
        void SearchDefaultPrinterL();
        TBool ChangeDefaultPrinterL( TInt aProtocol );
        
    public: // Methods derived from MProtocolObserver
        void UsbCableDisconnected();
    
    private: 
    	
    	// Interface for callbacks
    	MUIModeObserver* iUIModeObserver;
    	
    	// Protocol controller for managing protocol changes etc.
    	CUIProtocolController* iUIProtocolController;
    	
        // Interface factory
        CIFFactory* iIFFactory;
   
    };

#endif  // CAIWMODE_H

//  End of File
