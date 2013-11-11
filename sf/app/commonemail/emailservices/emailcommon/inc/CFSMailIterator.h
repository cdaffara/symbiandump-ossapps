/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  email list iterator
*
*/


#ifndef CFSMAILITERATOR_H
#define CFSMAILITERATOR_H

//  INCLUDES
//<cmail>
#include "MFSMailIterator.h"
#include "CFSMailMessage.h"
//</cmail>

/**
 *  Email framework list iterator. First call should contain NULL id as begin id :
 *  eg. NextL(TFSMailId(), count, messages);
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailIterator ) : public CBase, public MFSMailIterator
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aIterator plugin iteraror 
     * @param aRequestHandler request handler for plugin requests
     */
     IMPORT_C static CFSMailIterator* NewL( MFSMailIterator& aIterator,
                                   CFSMailRequestHandler* aRequestHandler );
									
    /**
     * Two-phased constructor.
     *
     * @param aIterator plugin iteraror 
     * @param aRequestHandler request handler for plugin requests
     */
     IMPORT_C static CFSMailIterator* NewLC(	MFSMailIterator& aIterator,
									CFSMailRequestHandler* aRequestHandler );
									
    /**
     * see MFSMailIterator::NextL
     *
     */
     IMPORT_C TBool NextL( TFSMailMsgId aCurrentMessageId, 
                           TUint aCount, 
                           RPointerArray<CFSMailMessage>& aMessages);
                           
    /**
     * see MFSMailIterator::NextL
     *
     */
     IMPORT_C TBool NextL( const TDesC&  aStartWith, 
                           TUint aCount, 
                           RPointerArray<CFSMailMessage>& aMessages);
        
    /**
     * see MFSMailIterator::PreviousL
     *
     */
     IMPORT_C TBool PreviousL( TFSMailMsgId aCurrentMessageId, 
                               TUint aCount, 
                               RPointerArray<CFSMailMessage>& aMessages);
        
    /**
     * see MFSMailIterator::PreviousL
     *
     */
     IMPORT_C TBool PreviousL( const TDesC&  aStartWith, 
                              TUint aCount, 
                              RPointerArray<CFSMailMessage>& aMessages);
    /**
     * Destructor.
     */  
   	 ~CFSMailIterator();
        	
private:

  	/**
     * Two-phased constructor
     */
     void ConstructL( MFSMailIterator& aIterator,
                      CFSMailRequestHandler* aRequestHandler );
                                            
    /**
     * C++ default constructor.
     */
  	 CFSMailIterator();

    /**
     * sets request handler
     */
     void SetAsyncRequestHandler(RPointerArray<CFSMailMessage>& aMessages);
  	
private: // data
 
    /**
     * request handler for plugin requests
     */
     CFSMailRequestHandler*    iRequestHandler;	

    /**
     * plugin iterator
     */
     MFSMailIterator*          iIterator;
   	};

#endif  // M_FSMAILITERATOR_H

// End of File
