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

#ifndef LOGSEVENT_H
#define LOGSEVENT_H

//  INCLUDES
#include <QObject>
#include <QDateTime>
#include <logsexport.h>

// FORWARD DECLARATION
class LogsEventData;
class LogsEventStrings;
class CLogEvent;
class QDataStream;

// CLASS DECLARATION

/**
 *
 */
class LogsEvent
    {
public:
    
    // direction of the event
    enum LogsDirection
        {
        DirUndefined = -1, // undefined
        DirIn =  0,        // incoming
        DirOut = 1,        // outgoing
        DirMissed = 2      // missed
        };

    // type of the event
    enum LogsEventType
        {
        TypeUndefined = -1,
        TypeVoiceCall,
        TypeVideoCall,
        TypeVoIPCall
        };
    
    enum LogsEventState
        {
        EventAdded,
        EventUpdated,
        EventNotUpdated
        };
    
    public:

       /**
        * Constructor.
        */
        LogsEvent();
        
        /**
         * Copy constructor.
         */
        LogsEvent( const LogsEvent& event );
        
        /**
        * Destructor.
        */
        virtual ~LogsEvent();
              

    public:
        
        /**
         * Initialized the event with contents of given source event.
         * Event state and view status are updated automatically at
         * this phase.
         * @param source, source event
         * @param strings, list of strings used for comparison
         */
        void initializeEventL( const CLogEvent& source,
                               const LogsEventStrings& strings );
        
        /**
         * Check event's view status which tells whether the event is
         * still valid or not. If event is not anymore in view, it
         * is not probably needed anymore.
         */
        bool isInView() const;
        
        /**
         * Get state of the event. Event state is updated
         * always at initialization phase. See initializeEventL.
         */
        LogsEventState eventState() const;
        
        bool isRemotePartyPrivate() const;

		bool isRead() const;
        
        void setIsRead(bool isRead);
		
        QDateTime time() const;
        
        int logId() const;

        const QString& number() const;

        const QString& remoteParty() const;

        LogsDirection direction() const;

        int eventUid() const;

        LogsEventType eventType() const;

        int duplicates() const;

        LogsEventData* logsEventData() const;

        bool ALS() const; 

        bool CNAP() const { return false; };
        
        int ringDuration() const;
        
        int duration() const;
     
        /**
         * Sets current index of the event.
         */
        void setIndex(int index);
        
        /**
         * Returns last set index for the event.
         */
        int index() const;
        
        /**
         * Set view status of the event.
         */
        void setIsInView(bool isInView);
        
        /**
         * Set number of duplicates for this events (e.g. 2 missed calls)
         */
        void setDuplicates( int duplicates );
        
        /**
         * Set remote party name for the event.
         */
        void setRemoteParty( const QString& remoteParty );
        
        /**
        * Set indication of result of searching matching 
        * contact from contacts
        */
        void setContactMatched( bool value );
                
        /**
        * Get indication of result of searching matching 
        * contact from contacts
        */
        bool contactMatched();
        
        /**
         * Check if event is valid.
         * @return true if valid, otherwise false
         */
        bool validate();
        
        /**
         * Return direction as string
         */
        QString directionAsString() const;
        
        /**
         * Return event type as string
         */
        QString typeAsString() const;
        
        /**
         * 
         */
        QString getNumberForCalling();
        
        LogsEvent( QDataStream& serializedEvent );
        
        bool serialize( QDataStream& serializeDestination );
        
    private:                               

        bool setTime( const QDateTime& time );

        void setLogId( const int logId );
       
        bool setNumber( const QString& number );

        bool setDirection( LogsDirection aDirection );

        void setEventUid( int uid );

        bool setEventType( LogsEventType aEventType );

        void setLogsEventData( LogsEventData* logsEventData );

        void setALS( bool aALS ); 
        
        void setRingDuration( int ringDuration );     

        bool isEmergencyNumber( const QString& number );
        
        void setDuration( int duration );
        
    private:    // data
            
        int mLogId;        
        LogsDirection mDirection;
        LogsEventType mEventType;
        int mUid;
        LogsEventData* mLogsEventData;
        QString mRemoteParty;
        QString mNumber;
        int mDuplicates;               
        QDateTime mTime;       
        int mRingDuration;
	    bool mIsRead;  
	    bool mIsALS;
	    int mDuration;
	    
	    int mIndex;
	    bool mIsInView;
	    bool mContactMatched;
	   bool mIsPrivate;
	    LogsEventState mEventState;
	    
    private:
        
        friend class LogsEventParser;
        friend class LogsReaderStateFillDetails;
        
    private: // Testing related friend definitions

#ifdef LOGSDBCONNECTOR_SIMULATION
        friend class LogsDbConnector; 
#endif
        friend class UT_LogsFilter;
        friend class UT_LogsModel;
        friend class UT_LogsEvent;
        friend class UT_LogsDbConnector;
        friend class UT_LogsCall;
        friend class UT_LogsContact;
        friend class UT_LogsEventParser;
        friend class UT_LogsEventDataParser;
        friend class UT_LogsReaderStates;
        friend class UT_LogsDetailsModel;
        friend class UT_LogsMessage;
        friend class UT_LogsDetailsView;
    };

#endif      // LOGSEVENT_H



// End of File
