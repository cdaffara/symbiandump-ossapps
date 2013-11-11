/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CBtOnQuery class definition. This Implementation is used to avoid
*  				 User::WaitForRequest() function and ViewServer 11 panic when BT naming query is activated. 
*
*/


#ifndef BTONQUERY_H
#define BTONQUERY_H

#include <e32base.h>

class MBTOnNotify;

class CBtOnQuery : public CActive
    {
    public: // construction / destruction
   
        /**
        * NewL()
        * Create a CBtOnQuery object
        * @param aNotify notifier to the calling object
        * @return a pointer to the created instance of CBtOnQuery
        */
        static CBtOnQuery* NewL( MBTOnNotify& aNotify );  

        /**
        * NewLC()
        * Create a CBtOnQuery object
        * @param aNotify notifier to the calling object
        * @return a pointer to the created instance of CBtOnQuery
        */
        static CBtOnQuery* NewLC( MBTOnNotify& aNotify );  
        
       /**
        * ~CBtOnQuery()
        * Destroy the object and release all memory objects
        */
        ~CBtOnQuery();

    public: // new functions
        
        /**
        * IsBluetoothOn()
        * Starts this Active object
        */
        void IsBluetoothOn();

    protected: // from CActive 
  
        /**
        * RunL()
        * Respond to an event
        */
        void RunL();		
  
         /**
        * RunError()
        * Handles errors which is throwed by RunL() function 
        */ 
        void RunError();
        
        /**
        * DoCancel
        * Cancel any outstanding requests
        */
        void DoCancel();

    protected: // construction
 
        /**
        * CBtOnQuery()
        * @param aNotify notifier to the calling object
        */
        CBtOnQuery( MBTOnNotify& aNotify );
        
        /**
        * ConstructL()
        * Standard second phase construction
        */
        void ConstructL();

    private: // data
        MBTOnNotify &iNotify;    
       	RNotifier iNotif;
       	TPckgBuf<TBool> iResultPckg;	
    };

#endif // BTONQUERY_H

//  End of File
