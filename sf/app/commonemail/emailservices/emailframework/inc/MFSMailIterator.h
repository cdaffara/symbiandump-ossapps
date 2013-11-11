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


#ifndef MFSMAILITERATOR_H
#define MFSMAILITERATOR_H

//  INCLUDES
//<cmail>
#include "CFSMailMessage.h"
//</cmail>

/**
 *  Email framework list iterator. First call should contain NULL id as begin id :
 *  eg. NextL(TFSMailId(), count, messages);
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class MFSMailIterator
    {

public:
                                    
    /**
     * Lists user given count of email objects located after user
     * given start point.
     *
     * @param aCurrentMessageId start point is id of the first message
     *        to be listed
     * @param aCount email object count to be listed
     * @param aMessages user given and owned table where plugin stores
     *        email objects
     *
     * @return true if more email objects follows after listed objects
     */
     virtual TBool NextL( TFSMailMsgId aCurrentMessageId, 
                           TUint aCount, 
                           RPointerArray<CFSMailMessage>& aMessages) = 0;
                           
    /**
      * Lists user given count of email objects located after user
      * given start point.
     *
      * @param aStartWith start point is user given string to match
      * @param aCount email object count to be listed
      * @param aMessages user given and owned table where
      *        plugin stores email objects
     *
      * @return true if more email objects follows after listed objects
     */
     virtual TBool NextL( const TDesC&  aStartWith, 
                           TUint aCount, 
                           RPointerArray<CFSMailMessage>& aMessages) = 0;
        
    /**
      * Lists user given count of email objects located before user
      * given point.
     *
      * @param aCurrentMessageId end pointer is id of the last message
      *        to be listed
      * @param aCount email object count to be listed
      * @param aMessages user given and owned table where plugin stores
      *        email objects
     *
      * @return true if more email objects exists before listed objects
     */
     virtual TBool PreviousL( TFSMailMsgId aCurrentMessageId, 
                               TUint aCount, 
                               RPointerArray<CFSMailMessage>& aMessages) = 0;
        
    /**
      * Lists user given count of email objects located before user
      * given point.
     *
      * @param aStartWith end point is user given string to match
      * @param aCount email object count to be listed
      * @param aMessages user given and owned table where plugin stores email
      *        objects
     *
      * @return true if more email objects exists before listed objects
     */
     virtual TBool PreviousL( const TDesC&  aStartWith, 
                              TUint aCount, 
                              RPointerArray<CFSMailMessage>& aMessages) = 0;
    /**
     * Destructor.
     */  
     virtual ~MFSMailIterator() { }; 
            
    };

#endif  // MFSMAILITERATOR_H


// End of File
