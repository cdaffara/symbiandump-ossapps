/*!
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
* Description:  Bubble manager's interface.
*
*/

#ifndef BUBBLEMANAGERIF_H
#define BUBBLEMANAGERIF_H

#include <QtCore>

class HbAction;
class QGraphicsWidget;

class BubbleManagerIF
{

public:

    // Call states
    enum PhoneCallState
        {
        None           = 0, // Default on creation
        OnHold,
        Disconnected,
        Active,
        Incoming,
        Waiting,
        AlertToDisconnected,
        Outgoing,
        Alerting
        };

    // Call Flags
    enum PhoneCallFlag
        {
        Normal         = 0x00,     // Default
        NoCiphering    = 0x08,
        Line2          = 0x10,
        Muted          = 0x20,
        Diverted       = 0x40,
        Video          = 0x80,
        VoIPCall       = 0x100,
        EmergencyCall  = 0x200
        };
    Q_DECLARE_FLAGS(PhoneCallFlags, PhoneCallFlag)

    // Number types
    enum PhoneNumberType
        {
        NotSet    = 0, // Default
        Home,
        Mobile,
        Work,
        Phone,
        FaxNumber,
        Pager,
        Car,
        Assistant
        };

    // Cli shown in conference participant list
    enum ParticipantListCli
        {
        ParticipantListCliText,        // Name or Call %N
        ParticipantListPhoneNumber     // Phonenumber
        };


public:

    // ====================================================
    // Must surround all the changes:
    // ====================================================
    //

    /**
    * Before making any changes to bubble headers, call this function
    * so that manager can prapare for them properly.
    */
    virtual void startChanges () = 0;

    /**
    * After the changes to bubble headers call this function so manager
    * can prepare bubbles to right places and check the drawing order.
    * Memory for arrays has been allocated beforehand.
    */
    virtual void endChanges () = 0;


    // ====================================================
    // For call headers and some for conference call
    // ====================================================
    //

    /**
    * Takes a new call header in use.
    * Returns bubble idenfication number.
    */
    virtual int createCallHeader () = 0;

    /**
    * Removes call header from use
    */

    virtual void removeCallHeader (int bubbleId) = 0;

    /**
    * Sets call state to header.
    */
    virtual void setState (int bubbleId, PhoneCallState state) = 0;

    /**
    * Sets text label to header. For conf also.
    * Text to be seen in bubble ( e.g. 'on hold' )
    */
    virtual void setLabel (
        int bubbleId,
        const QString& text,
        Qt::TextElideMode clipDirection) = 0;

    /**
    * Sets caller's line identification ( name or number) to header.
    * For conf also. Caller's CLI ( e.g. 'Mother' )
    */
    virtual void setCli (int bubbleId,
                 const QString& cliText,
                 Qt::TextElideMode clipDirection) = 0;

    /**
    * Updates caller's line identification ( name or number) to header.
    * Caller's CLI ( e.g. 'Daddy' )-
    */
    virtual void updateCLI (
        int bubbleId,
        const QString& cliText,
        Qt::TextElideMode clipDirection) = 0;

    /**
    * Number or voip adress, when phonebook name takes Cli.
    */
    virtual void setSecondaryCli (
        int bubbleId,
        const QString& cliText,
        Qt::TextElideMode clipDirection = Qt::ElideLeft) = 0;

    /**
    * Sets call time or cost text to header. For conf also.
    */
    virtual void setCallTime (int bubbleId, const QString& callTime) = 0;

    /**
    * Updates call time or cost text to header.  For conf also.
    * Timer or cost text ( e.g. '00:12:34' or '£01.23' ).
    */
    virtual void updateCallTime (int bubbleId, const QString& callTime) = 0;

    /**
    * Attach a call image to header.
    */
    virtual void setCallObjectImage (int bubbleId, const QString &fileName) = 0;

    /**
    * Attach the theme call image to header.
    */
    virtual void setCallObjectFromTheme (int bubbleId) = 0;

    /**
    * Sets call flags to header.
    */
    virtual void setCallFlags (int bubbleId, int flags) = 0;

    /**
    * Sets call flags to header.
    */
    virtual void setCallFlag (int bubbleId, BubbleManagerIF::PhoneCallFlags callFlags, bool set) = 0;

    /**
    * Sets number type.
    */
    virtual void setNumberType (int bubbleId, PhoneNumberType type) = 0;

    // ====================================================
    // For all bubbles
    // ====================================================
    //

    /**
    * Sets phone muted/unmuted.
    * @param aIsMuted ETrue if phone is muted.
    */
    virtual void setPhoneMuted (bool muted) = 0;


    // ====================================================
    // For conference call
    // ====================================================
    //

    /**
    * Creates a conference call based upon two calls. Bubbles must be
    * created first.
    */
    virtual int createConference (int bubble1, int bubble2) = 0;

    /**
    * Splits conference call into invidual two calls. Call headers stays
    * in use. Headers' state will not be changed.
    */
    virtual void removeConference () = 0;

    /**
    * Adds new call to conference call.
    */
    virtual void addRowToConference (int bubbleId) = 0;

    /**
    * Takes specified call out of conference. Use RemoveConference if
    * conference has only two calls in it. Header's state is same as
    * before adding it to conference (if not changed inside the conf).
    */
    virtual void removeRowFromConference (int bubbleId) = 0;

    /**
    * Counts calls in conference call.
    */
    virtual int conferenceRowCount () const = 0;

    /**
    * Sets highlight to specified line in conference.
    */
    virtual void setSelectionInConference (int rowNumber) = 0;

    /**
    * Sets highlight to specified bubble id in conference.
    */
    virtual void setSelectionIdInConference (int bubbleId) = 0;

    /**
    * Gets highlighted item in conference.
    */
    virtual int selectionInConference () const = 0;

    /**
    * Gets highlighted item in conference.
    */
    virtual int selectionIdInConference () const = 0;

    /**
    * Moves highligh one up if possible
    */
    virtual void moveHighlightOneUpInConference () = 0;

    /**
    * Moves highligh one down if possible
    */
    virtual void moveHighlightOneDownInConference () = 0;

    /**
    * Use this function to expand or shrink conference bubble.
    */
    virtual void setExpandedConferenceCallHeader (bool expanded) = 0;

    /**
    * Query: is conference expanded?
    */
    virtual bool isConferenceExpanded () const = 0;

    /**
    * Number of headers shown on the screen.
    */
    virtual int shownHeaderCount () const = 0;

    /**
    * Set CLI used in participant list (text or phonenumber).
    */
    virtual void setParticipantListCli (int aBubbleId, ParticipantListCli aParticipantCli) = 0;


    // ====================================================
    // Call bubble actions
    // ====================================================
    //

    /**
    * Appends new action to call bubble
    */
    virtual void addAction (int bubble, HbAction *action) = 0;

    /**
    * Removes all actions from call bubble
    */
    virtual void clearActions (int bubble) = 0;
    
    /**
    * Gets graphics widget for action
    */
    virtual QGraphicsWidget* graphicsWidgetForAction (HbAction *action) const = 0;

    /**
    * Appends new action to participant list in conference call bubble.
    */
    virtual void addParticipantListAction (HbAction *action) = 0;

    /**
    * Removes participant list actions from conference call bubble.
    */
    virtual void clearParticipantListActions () = 0;


    // ====================================================
    // Bubble state related (multicall)
    // ====================================================
    //

    /**
    * Bubble shown in expanded position.
    */
    virtual int expandedBubble() const = 0;

    /**
    * Sets action that is triggered when user taps collapsed
    * call bubble and it changes to expanded. Setting null
    * pointer removes expand action.
    */
    virtual void setExpandAction(int bubble, HbAction* action) = 0;

    /**
    * Bubble state (expanded/collapsed) cannot be changed when
    * disabled is set true.
    */
    virtual void setBubbleSelectionDisabled(bool disabled) = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BubbleManagerIF::PhoneCallFlags)

#endif // BUBBLEMANAGERIF_H
