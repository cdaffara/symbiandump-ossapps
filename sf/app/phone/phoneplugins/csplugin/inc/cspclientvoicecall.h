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
* Description:  Implements CSPClientVoiceCall for CS Call Plug-in
*
*/


#ifndef CSPCLIENTVOICECALL_H
#define CSPCLIENTVOICECALL_H

#include "cspvoicecall.h" 

/**
* Client voice call class for CS Call Plug-in. 
*
*  @lib csplugin.dll
*/
class CSPClientVoiceCall : public CSPVoiceCall
    {
    
    public:
        
        /*
        * Creates a new CSPClientVoiceCall object.
        * @since S60 5.0
        * @param aName dialled number (MT) or call name (MO)
        * @param aLine line for opening the call
        * @param aParams call parameters 
        * @param aCommonInfo common information for call,
        * @return pointer to created call object
        */
        static CSPClientVoiceCall* NewL( const TDesC& aName,
                              RMobileLine& aLine,
                              const CCCECallParameters& aParams,
                              MCSPCommonInfo& aCommonInfo );

        /**
        * C++ default destructor
        */
        virtual ~CSPClientVoiceCall( );
      
    // from base class CSPVoiceCall

        /**
        * From CSPVoiceCall
        * Updates the state change and forwards the state for observer.
        * @param aState new state of the call
        */
        void NotifyCallStateChanged( MCCPCallObserver::TCCPCallState aState );

        /**
        * From CSPVoiceCall
        * Updates the state change and forwards the state for observer.
        * @param aState new state of the call
        */
        void NotifyCallStateChangedWithInband( MCCPCallObserver::TCCPCallState aState );
        
    private:    
        
        /*
        * Voice call constructor.
        * @param aLine open line ready for use
        * @param aName remote party or call name depending on direction
        * @param aCommonInfo reference to common info interface
        */
        CSPClientVoiceCall( RMobileLine& aLine,  
                 const TDesC& aName,
                 MCSPCommonInfo& aCommonInfo );
                 
        /*
        * Constructing 2nd phase.
        */
        void ConstructL( const CCCECallParameters& aParams );
        
        /*
        * Indicate client call. 
        * Indication is done in Dialling state and can be done only once 
        */
        void IndicateClientCall(); 
        
        /**
        * Open call handle.  
        * Opens an existing handle for client call.    
        */
        void OpenCallHandleL();
        
        /**
        * Update call info.  
        */
        void UpdateCallInfoImpl( const RMobileCall::TMobileCallInfoV7& aCallInfo );
        
    private: // Data
        
        /**
        * Defines whether client call has been already indicated for this call 
        * when Dialling state is entered. 
        */
        TBool iClientCallIndicated;
        
    };


#endif // CSPCLIENTVOICECALL_H
