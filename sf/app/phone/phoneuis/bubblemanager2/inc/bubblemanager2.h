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
* Description:  BubbleManager widget.
*
*/

// Bubble widget API

#ifndef BUBBLEMANAGER2_H
#define BUBBLEMANAGER2_H

#include <QtCore>
#include <hbwidget.h>
#include "bubblemanagerif.h"

#ifdef BUILD_BUBBLEMANAGER
#define BUBBLEMANAGER_EXPORT Q_DECL_EXPORT
#else
#define BUBBLEMANAGER_EXPORT Q_DECL_IMPORT
#endif

class BubbleHeader;
class BubbleConferenceHeader;
class BubbleWidgetManager;
class BubbleImageManager;
class HbAction;
class HbStackedLayout;
class BubbleHandler;
class BubbleEffectHandler;

static const int BUBBLE_MAX_CALL_COUNT = 7;
static const int BUBBLE_CONF_CALL_ID   = 10;

class BUBBLEMANAGER_EXPORT BubbleManager : public HbWidget, public BubbleManagerIF
{
    Q_OBJECT

public:
    explicit BubbleManager( QGraphicsItem *parent = 0 );
    ~BubbleManager();

public:

    // ====================================================
    // Must surround all the changes:
    // ====================================================
    //

    /**
    * Before making any changes to bubble headers, call this function
    * so that manager can prapare for them properly.
    */
    void startChanges();

    /**
    * After the changes to bubble headers call this function so manager
    * can prepare bubbles to right places and check the drawing order.
    * Memory for arrays has been allocated beforehand.
    */
    void endChanges();


    // ====================================================
    // For call headers and some for conference call
    // ====================================================
    //

    /**
    * Takes a new call header in use.
    * Returns bubble idenfication number.
    */
    int createCallHeader();

    /**
    * Removes call header from use
    */
    void removeCallHeader( int bubbleId );

    /**
    * Sets call state to header.
    */
    void setState( int bubbleId,
                   PhoneCallState state );

    /**
    * Sets text label to header. For conf also.
    * Text to be seen in bubble ( e.g. 'on hold' )
    */
    void setLabel(
        int bubbleId,
        const QString& text,
        Qt::TextElideMode clipDirection = Qt::ElideRight );

    /**
    * Sets caller's line identification ( name or number) to header.
    * For conf also. Caller's CLI ( e.g. 'Mother' )
    */
    void setCli( int bubbleId,
                 const QString& cliText,
                 Qt::TextElideMode clipDirection );

    /**
    * Updates caller's line identification ( name or number) to header.
    * Caller's CLI ( e.g. 'Daddy' )-
    */
    void updateCLI(
        int bubbleId,
        const QString& cliText,
        Qt::TextElideMode clipDirection );

    /**
    * Number or voip adress, when phonebook name takes Cli.
    */
    void setSecondaryCli(
        int bubbleId,
        const QString& cliText,
        Qt::TextElideMode clipDirection = Qt::ElideLeft );


    /**
    * Sets call time or cost text to header. For conf also.
    */
    void setCallTime(
        int bubbleId,
        const QString& callTime );

    /**
    * Updates call time or cost text to header.  For conf also.
    * Timer or cost text ( e.g. '00:12:34' or '£01.23' ).
    */
    void updateCallTime(
        int bubbleId,
        const QString& callTime );

    /**
    * Attach a call image to header.
    */
    void setCallObjectImage( int bubbleId,
                             const QString& fileName );

    /**
    * Attach the theme call image to header.
    */
    void setCallObjectFromTheme( int bubbleId );

    /**
    * Sets call flags to header.
    */
    void setCallFlags(
        int bubbleId,
        int flags );

    /**
    * Sets call flags to header.
    */
    void setCallFlag( int bubbleId,
                      PhoneCallFlags flag,
                      bool set );

    /**
    * Sets number type.
    */
    void setNumberType( int bubbleId,
                        PhoneNumberType type );

    // ====================================================
    // For all bubbles
    // ====================================================
    //

    /**
    * Sets phone muted/unmuted.
    * @param aIsMuted ETrue if phone is muted.
    */
    void setPhoneMuted( bool muted = true );


    // ====================================================
    // For conference call
    // ====================================================
    //

    /**
    * Creates a conference call based upon two calls. Bubbles must be
    * created first.
    */
    int createConference( int bubble1,
                          int bubble2 );

    /**
    * Splits conference call into invidual two calls. Call headers stays
    * in use. Headers' state will not be changed.
    */
    void removeConference();

    /**
    * Adds new call to conference call.
    */
    void addRowToConference( int bubbleId );

    /**
    * Takes specified call out of conference. Use RemoveConference if
    * conference has only two calls in it. Header's state is same as
    * before adding it to conference (if not changed inside the conf).
    */
    void removeRowFromConference( int bubbleId );

    /**
    * Counts calls in conference call.
    */
    int conferenceRowCount() const;

    /**
    * Sets highlight to specified line in conference.
    */
    void setSelectionInConference( int rowNumber );

    /**
    * Sets highlight to specified bubble id in conference.
    */
    void setSelectionIdInConference( int bubbleId );

    /**
    * Gets highlighted item in conference.
    */
    int selectionInConference() const;

    /**
    * Gets highlighted item in conference.
    */
    int selectionIdInConference() const;

    /**
    * Moves highligh one up if possible
    */
    void moveHighlightOneUpInConference();

    /**
    * Moves highligh one down if possible
    */
    void moveHighlightOneDownInConference();

    /**
    * Use this function to expand or shrink conference bubble.
    */
    void setExpandedConferenceCallHeader(
        bool expanded = true );

    /**
    * Query: is conference expanded?
    */
    bool isConferenceExpanded( ) const;

    /**
    * Number of headers shown on the screen.
    */
    int shownHeaderCount() const;

    /**
    * Set CLI used in participant list (text or phonenumber).
    */
    void setParticipantListCli(
        int aBubbleId,
        ParticipantListCli aParticipantCli );


    // ====================================================
    // Call bubble actions
    // ====================================================
    //

    /**
    * Appends new action to call bubble
    */
    void addAction ( int bubbleId, HbAction* action );

    /**
    * Removes all actions from call bubble
    */
    void clearActions( int bubbleId );

    /**
    * Gets graphics widget for action
    */
    QGraphicsWidget* graphicsWidgetForAction(
        HbAction* action ) const;

    /**
    * Appends new action to participant list in conference call bubble.
    */
    void addParticipantListAction(HbAction *action);

    /**
    * Removes participant list actions from conference call bubble.
    */
    void clearParticipantListActions();


    // ====================================================
    // Bubble state related (multicall)
    // ====================================================
    //

    /**
    * Bubble shown in expanded position.
    */
    int expandedBubble() const;

    /**
    * Sets action that is triggered when user taps collapsed
    * call bubble and it changes to expanded. Setting null
    * pointer removes expand action.
    */
    void setExpandAction(int bubbleId, HbAction* action);

    /**
    * Bubble state (expanded/collapsed) cannot be changed when
    * disabled is set true.
    */
    void setBubbleSelectionDisabled(bool disabled);

public slots:
    void handleOrientationChange(Qt::Orientation orientation);

private:
    QGraphicsWidget* selectView();
    bool addToLayout(QGraphicsWidget* widget);
    void removeFromLayout(QGraphicsWidget* widget);
    void connectSignals(QGraphicsWidget* widget);
    void setViewData(QGraphicsWidget* view);

    bool findActiveHeader(int bubbleId, BubbleHeader*& header);
    void findNextDrawableHeader(int& index) const;
    void polishEvent();
    void releaseImageIfNotUsed(const QString& imageFileName);
    void releaseNonCachedViews();

private slots:
    void showExpanded( int bubbleId );

private:
    QVector<BubbleHeader*> mCallHeaders;
    BubbleConferenceHeader* mConferenceHeader;
    QVector<BubbleHeader*> mActiveHeaders;
    QVector<BubbleHandler*> mActiveHandlers;

    int mIsReadyToDraw;
    bool mMuted;
    QGraphicsWidget* mMutedIcon;
    bool mSortHeaders;
    BubbleWidgetManager* mWidgetManager;
    BubbleImageManager* mBubbleImageManager;
    HbStackedLayout* mMainLayout;
    bool mInitialized;
    bool mBubbleSelectionDisabled;
    BubbleEffectHandler* mEffectHandler;
};

#endif /* BUBBLEMANAGER2_H */



















