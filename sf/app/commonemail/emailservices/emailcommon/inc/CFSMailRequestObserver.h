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
* Description: This file defines class CFSMailRequestObserver.
*
*/


#ifndef CFSMAILREQUESTOBSERVER_H
#define CFSMAILREQUESTOBSERVER_H

//  INCLUDES
//<cmail>
#include "MFSMailRequestObserver.h"
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailRequestHandler.h"

/**
 *  asynchronous request observer
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailRequestObserver ) : public CBase, public MFSMailRequestObserver
    {
    public:

    /**
     * Two-phased constructor.
     *
     * @param aPluginManager plugin manager pointer
     * @param aObserver request observer given by user
     */
     static CFSMailRequestObserver* NewL( CFSMailRequestHandler& aPluginManager,
                                          MFSMailRequestObserver& aObserver );
  	
    /**
     * Two-phased constructor.
     *
     * @param aPluginManager plugin manager pointer
     * @param aObserver request observer given by user
     */
  	 static CFSMailRequestObserver* NewLC( CFSMailRequestHandler& aPluginManager,
  	                                       MFSMailRequestObserver& aObserver );

    /**
     * C++ default constructor
     */
     CFSMailRequestObserver();

    /**
     * destructor
     */
     ~CFSMailRequestObserver();

    /**
     * asynchronous request response
     *
     * @param aEvent event description (load complete, partial fetch data, etc.)
     * @param aRequestId pending request id in plugin side
     */
     void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

    /**
     * set user observer pointer
     *
     * @param aObserver observer pointer
     */
     void SetUserObserver(MFSMailRequestObserver& aObserver);

 protected:

    /**
     * ConstructL
     */
     void ConstructL( CFSMailRequestHandler& iRequestHandler, 
                      MFSMailRequestObserver& aObserver );

 private: // data

   	 /** request handler pointer */
   	 CFSMailRequestHandler*    iRequestHandler;
	 
   	 /** user observer */
     MFSMailRequestObserver*  iObserver;
     
    };

#endif  

// End of File
