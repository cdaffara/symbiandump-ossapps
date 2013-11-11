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
#include <logsexport.h>
#include <QDateTime>
#include <qmobilityglobal.h>
#include <QList>

// FORWARD DECLARATION
class LogsEventData;
class LogsEventStrings;
class CLogEvent;
class QDataStream;

QTM_BEGIN_NAMESPACE
class QContactName;
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

// CLASS DECLARATION

/**
 * Logs event information.
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
   
    public: // Exported API
            
        /**
         * Copy constructor.
         */
        LOGSENGINE_EXPORT LogsEvent( const LogsEvent& event );
        
        /**
         * Destructor.
         */
        LOGSENGINE_EXPORT virtual ~LogsEvent();
        
        /**
         * Get time when event occurred.
         * @return date time
         */
        LOGSENGINE_EXPORT QDateTime time() const;
        
        /**
         * Get unique id of the event
         * @return id
         */
        LOGSENGINE_EXPORT int logId() const;

        /**
         * Get phone number associated with the event.
         * @return number
         */
        LOGSENGINE_EXPORT const QString& number() const;

        /**
         * Get remote party information.
         * @return remote party name
         */
        LOGSENGINE_EXPORT const QString& remoteParty() const;

        /**
         * Get direction of the event
         * @return direction
         */
        LOGSENGINE_EXPORT LogsDirection direction() const;

        /**
         * Get event type
         * @return event type
         */
        LOGSENGINE_EXPORT LogsEventType eventType() const;
        
        /**
         * Get number of duplicated events (e.g. 5 missing calls from x)
         * @return number of duplicates
         */
        LOGSENGINE_EXPORT int duplicates() const;
        
        /**
         * Check whether event was generated when using alternative line service
         * @return true if ALS was used
         */
        LOGSENGINE_EXPORT bool ALS() const; 
        
        /**
         * Get ringing duration
         * return rinding duration is seconds
         */
        LOGSENGINE_EXPORT int ringDuration() const;
        
        /**
         * Get duration of event
         * @return duration in seconds
         */
        LOGSENGINE_EXPORT int duration() const;

        /**
         * Number for calling to remote party of the event
         * @return number
         */
        LOGSENGINE_EXPORT QString getNumberForCalling();
        
        /**
         * Get contact id of associated contact
         * @return contact id, zero if no associated contact exist.
         */
        LOGSENGINE_EXPORT unsigned int contactLocalId() const;
        
        /**
         * Check whether event has been read already
         * @return true if event has been marked as read
         */
        LOGSENGINE_EXPORT bool isRead() const;
        
        /**
         * Construct event from serialized data stream
         */
        LOGSENGINE_EXPORT LogsEvent( QDataStream& serializedEvent );
        
        /**
         * Serialize event to data stream
         */
        LOGSENGINE_EXPORT bool serialize( QDataStream& serializeDestination );
        
        /**
        * Check whether event has private remoteparty
        * @return true if event has private remoteparty
        */
        LOGSENGINE_EXPORT bool isRemotePartyPrivate() const;
        
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
        
        bool setIsRead(bool isRead);
		
        int eventUid() const;

        LogsEventData* logsEventData() const;

        bool CNAP() const { return false; };
     
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
         * @param contactMatch, use true if remoteparty is set by contact matching
         */
        void setRemoteParty( const QString& remoteParty, bool contactMatch = false );
        
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
         * Search matching contact from contacts and update
         * event with possible match.
         * @param manager
         * @param contactNameStr, on return updated contact name
         * @return true if found matching contact, otherwise false
         */
        bool updateRemotePartyFromContacts(
            QContactManager& manager, QString& contactNameStr);
        
        /**
         * Prepares the event for contact matching.
         */
        void prepareForContactMatching();
        
        /**
         * Event has been marked as seen but database might not yet contain the change
         */
        void markedAsSeenLocally(bool markedAsSeen);
        
        /**
         * Check whether event is locally seen.
         */
        bool isSeenLocally() const;
        
        bool isRemotePartyUnknown() const;
        
        /**
        * Set remote party status as private of the event.
        */
        void setRemotePartyPrivate(bool markedAsPrivate);
        
        /**
        * Set remote party status as unknown of the event.
        */
        void setRemotePartyUnknown(bool markedAsUnknown);
        
        /**
         * Merge relevant information
         */
        void merge( LogsEvent& event );
        
        /**
         * Duplicate events
         */
        QList<LogsEvent>& mergedDuplicates();
        
        /**
         * Comparison
         */
        bool operator==(const LogsEvent& other);
        
        /**
         * Check if communication is possible.
         */
        bool isCommunicationPossible() const;
        
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
        
        void setContactLocalId( unsigned int id );

        QString stripPhoneNumber(const QString& num);
        
        QString parseContactName(const QContactName& name);
        
        bool isUnseenEvent( const LogsEvent& event ) const;
        
        
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
        LogsEventState mEventState;
        bool mIsLocallySeen;
        bool mIsPrivate;
        bool mIsUnknown;
		bool mContactMatched;
	    QList<LogsEvent> mMergedDuplicates;
	    
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
        friend class UT_LogsCustomFilter;
        friend class UT_LogsMatchesModel;
        friend class UT_LogsRemoveStates;
        friend class UT_LogsRemove;
        friend class UT_LogsDuplicateLookup;
    };

#endif      // LOGSEVENT_H



// End of File
