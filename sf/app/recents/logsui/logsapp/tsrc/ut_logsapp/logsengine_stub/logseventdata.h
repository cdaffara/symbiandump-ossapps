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
* Description:
*
*/

#ifndef LOGSEVENTDATA_H
#define LOGSEVENTDATA_H

//  INCLUDES
#include <QString>

// FORWARD DECLARATION
class CLogEvent;

// CLASS DECLARATION

/**
 * 
 */
class LogsEventData
    {
    
    public:  
        
        /**
         * Constructor.
         */
        LogsEventData();
        
        /**
         * Copy constructor.
         */
        LogsEventData( const LogsEventData& data );

       /**
        * Destructor.
        */
        virtual ~LogsEventData();


    public:  

        /**
         * Parses event data from given data
         * @param source data
         * @return 0 if parsed succesfully
         **/
        int parse( const CLogEvent& source );
        
        bool isCNAP() const;
        bool isVT() const;
        bool isPoC() const;
        bool isVoIP() const;
        long long dataReceived() const; 
        long long dataSent() const;
        bool isEmerg() const;
        int msgPartsNumber() const;
        unsigned int serviceId() const;      
        QString remoteUrl() const;
        QString localUrl() const;
        
        /**
         * Checks whether event data can be used in CS context.
         * @return true if CS compatible.
         */
        bool isCsCompatible();
            
        unsigned int contactLocalId() const;
        
    private:
        
        void setContactLocalId( unsigned int id );
        
    private:    // data

        // Calling Name Presentation service flag
        bool mIsCNAP;
        bool mIsVT;
        bool mIsPoC;
        bool mIsVoIP;

        //Flag that indicates whether this is an emergency related event (i.e emerg call)
        bool mIsEmerg;

        //Sent and received data where applicable (in bytes)
        long long mDataSent;
        long long mDataReceived;
        
        //Message parts. Meaningful only for sms            
        int mMsgPartsNumber;            

        unsigned int mServiceId;
        unsigned int mContactLocalId;
        
        QString mRemoteUrl;
        QString mLocalUrl;
        
    private:
        
        friend class LogsEventDataParser;
        friend class LogsEvent;

    private:  // Testing related friend definitions
        
        friend class UT_LogsEventData;
        friend class UT_LogsEvent;
        friend class UT_LogsEventParser;
        friend class UT_LogsEventDataParser;
        friend class UT_LogsCall;
        friend class UT_LogsContact;
        friend class UT_LogsMessage;
        friend class UT_LogsDetailsModel;
        friend class UT_LogsModel;
        friend class UT_LogsCustomFilter;
        
    };

#endif      // LOGSEVENTDATA_H


// End of File
