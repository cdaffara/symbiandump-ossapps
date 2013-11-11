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
* Description:  
*
*/

#include <QtGui>
#include <QDebug>

//#define BUBBLETEST_ENABLE_TESTABILITY
#ifdef BUBBLETEST_ENABLE_TESTABILITY
    #include <QtPlugin>
    #include <QPluginLoader>
    #include <QLibraryInfo>
    #include "testabilityinterface.h"
#endif

#include <hbaction.h>
#include <hbtoolbar.h>
#include <bubblemanager2.h>
#include <hbmenu.h>
#include <hbmainwindow.h>
#include "bubbletestview.h"
#include <hbeffect.h>


BubbleTestView::BubbleTestView(HbMainWindow& window, QGraphicsItem *parent) :
    HbView (parent), mMainWindow(window), mMuted(0), mConfBubbleId(-1),
    mProvideJoin(false)
{
    setFlags (QGraphicsItem::ItemIsFocusable);
    setFocusPolicy (Qt::StrongFocus);
    setTitle("BubbleTest2");

    loadTestabilityPlugin();
    
    // the widget under the test
    mBubbleManager = new BubbleManager (this);
    setWidget(mBubbleManager);

    // init toolbar
    createToolBarActions();
    //toolBar()->setOrientation(mMainWindow.orientation());
    connect( &mMainWindow,SIGNAL(orientationChanged(Qt::Orientation)),
             this,SLOT(handleOrientationChange(Qt::Orientation)));
    
    // init menu
    createMenuActions();

    // create actions for push buttons
    createBubbleActions();

    // disable toolbar effects
    HbEffect::disable(toolBar());
}

BubbleTestView::~BubbleTestView()
{
    toolBar()->clearActions();
}

void BubbleTestView::keyPressEvent(QKeyEvent *event)
{
    switch( event->key() )
    {
    case '1': // create incoming call
    {
        if ( mEmergencyCall->isChecked() ) {
            return; // not supported
        }

        if ( mCalls.count() == 0 ) {
            bubbleManager().startChanges();
            int bubbleId = bubbleManager().createCallHeader();
            setBubbleData(bubbleId, BubbleManager::Incoming);
            setBubbleActions(bubbleId, BubbleManager::Incoming);
            bubbleManager().endChanges();
            
            // store this call
            TestCall call;
            call.bubbleId = bubbleId;
            call.callState = BubbleManager::Incoming;
            mCalls.append( call );

            updateToolBarActions();

        } else if ( !initializingCallExists() ) {
            // waiting call
            bubbleManager().startChanges();
            int bubbleId = bubbleManager().createCallHeader();
            setBubbleData(bubbleId, BubbleManager::Waiting);
            setBubbleActions( bubbleId, BubbleManager::Waiting );
            bubbleManager().setExpandedConferenceCallHeader(false);
            bubbleManager().endChanges();

            // store this call
            TestCall call;
            call.bubbleId = bubbleId;
            call.callState = BubbleManager::Waiting;
            mCalls.append( call );

            setExpandActions();
            updateToolBarActions();
        }
        break;
    }
    
    case '2': // create outgoing call
    {
        if ( !initializingCallExists() && (callCount()<3) ) {
            bubbleManager().startChanges();
            int bubbleId = bubbleManager().createCallHeader();
            setBubbleData(bubbleId, BubbleManager::Outgoing);
            setBubbleActions( bubbleId, BubbleManager::Outgoing );
            bubbleManager().endChanges();

            // store this call
            TestCall call;
            call.bubbleId = bubbleId;
            call.callState = BubbleManager::Outgoing;
            mCalls.append( call );

            bubbleManager().setBubbleSelectionDisabled(false);
            updateToolBarActions();
       }
       break;
    }
    
    case '3': // connect
    {
        int i,j;

        if (callIndexByState(BubbleManagerIF::Outgoing, i)) {
            // outgoing to alerting
            bubbleManager().startChanges();
            setBubbleData(mCalls[i].bubbleId, BubbleManager::Alerting);
            bubbleManager().endChanges();
            mCalls[i].callState = BubbleManager::Alerting;
            return;
        }

        if (callCount()==3) {
            // replace active call
            bubbleManager().startChanges();
            Q_ASSERT(callIndexByState(BubbleManagerIF::Active, j));
            bubbleManager().removeCallHeader(mCalls[j].bubbleId);
            mCalls.removeAt(j);
            Q_ASSERT(callIndexByState(BubbleManagerIF::Waiting, i));
            setBubbleData(mCalls[i].bubbleId, BubbleManager::Active);
            setBubbleActions(mCalls[i].bubbleId, BubbleManager::Active);
            bubbleManager().endChanges();
            mCalls[i].callState = BubbleManager::Active;
            setExpandActions();
            updateToolBarActions();
            bubbleManager().setBubbleSelectionDisabled(false);
        } else if (callIndexByState(BubbleManagerIF::Incoming, i)||
                   callIndexByState(BubbleManagerIF::Alerting, i)||
                   callIndexByState(BubbleManagerIF::Waiting, i)) {
            bubbleManager().startChanges();
            setBubbleData(mCalls[i].bubbleId, BubbleManager::Active);
            mProvideJoin = true; // bit hackish
            setBubbleActions(mCalls[i].bubbleId, BubbleManager::Active);
            mProvideJoin = false;
            if (callIndexByState(BubbleManagerIF::Active, j)) {
                setBubbleData(mCalls[j].bubbleId, BubbleManager::OnHold);
                setBubbleActions(mCalls[j].bubbleId, BubbleManager::OnHold);
                mCalls[j].callState = BubbleManager::OnHold;
            }
            bubbleManager().endChanges();
            mCalls[i].callState = BubbleManager::Active;
            setExpandActions();
            updateToolBarActions();
            bubbleManager().setBubbleSelectionDisabled(false);
        }
        break;
    }

    case '4': // hold
    {
        int i,j;

        if (bubbleManager().isConferenceExpanded()) {
            toggleHoldConference();
        } else if (callCount()==1) {
            // toggle hold
            bubbleManager().startChanges();
            if (callIndexByState(BubbleManagerIF::Active, i)||
                callIndexByState(BubbleManagerIF::OnHold, i)) {
                BubbleManagerIF::PhoneCallState state =
                    mCalls[i].callState == BubbleManagerIF::Active ?
                    BubbleManagerIF::OnHold : BubbleManagerIF::Active;

                setBubbleData(mCalls[i].bubbleId, state);
                setBubbleActions(mCalls[i].bubbleId, state );
                mCalls[i].callState=state;
            }
            bubbleManager().endChanges();
            updateToolBarActions();
        } else if (callIndexByState(BubbleManagerIF::Active, i)) {
            // swap calls
            bubbleManager().startChanges();
            if (callIndexByState(BubbleManagerIF::OnHold, j)) {
                setBubbleData(mCalls[j].bubbleId, BubbleManager::Active);
                setBubbleActions(mCalls[j].bubbleId, BubbleManager::Active );
                mCalls[j].callState=BubbleManager::Active;
            }
            setBubbleData(mCalls[i].bubbleId, BubbleManager::OnHold);
            setBubbleActions(mCalls[i].bubbleId, BubbleManager::OnHold );
            mCalls[i].callState=BubbleManager::OnHold;
            if (conferenceCallExists()) {
                bubbleManager().setExpandedConferenceCallHeader(
                    bubbleIdByState(BubbleManager::Active)==mConfBubbleId);
            }
            bubbleManager().endChanges();
            setExpandActions();
            updateToolBarActions();
        } else if (callIndexByState(BubbleManagerIF::Waiting, i)) {
            // activate held call
            bubbleManager().startChanges();
            if (callIndexByState(BubbleManagerIF::OnHold, j)) {
                setBubbleData(mCalls[j].bubbleId, BubbleManager::Active);
                setBubbleActions(mCalls[j].bubbleId, BubbleManager::Active );
                mCalls[j].callState=BubbleManager::Active;
            }
            bubbleManager().endChanges();
            setExpandActions();
            updateToolBarActions();
        }
        break;
    }
    
    case '5': // disconnect
    {
        int i,j;

        if (callIndexByState(BubbleManagerIF::Disconnected, i)) {
            bubbleManager().startChanges();
            bubbleManager().removeCallHeader(mCalls[i].bubbleId);
            // remove call
            mCalls.removeAt(i);

            if (mMuted) {
                setMuted(); // unmute
            }

            bool expand = true;
            QListIterator<TestCall> calls(mCalls);
            while(calls.hasNext()) {
                TestCall call = calls.next();
                  if (!call.isConf && !call.isInConf) {
                      expand = false;
                  }
            }
            bubbleManager().setExpandedConferenceCallHeader(expand);
            bubbleManager().endChanges();

            updateToolBarActions();
            return;
        }

        if (callCount()) {
            int expanded = bubbleManager().expandedBubble();
            callIndexByBubbleId(expanded,i);
            bubbleManager().startChanges();

            if (mCalls.at(i).isConf) {
                bubbleManager().removeConference();
                mConfBubbleId = -1;
                mCalls.removeAt(i);

                QMutableListIterator<TestCall> calls(mCalls);
                j=0;
                while(calls.hasNext()) {                  
                    if (mCalls[j].isInConf) {
                        bubbleManager().removeCallHeader(mCalls[j].bubbleId);
                        mCalls.removeAt(j);
                    } else {
                        j++;
                    }
                    calls.next();
                }
            } else {
                setBubbleData(mCalls[i].bubbleId, BubbleManager::Disconnected);
                setBubbleActions(mCalls[i].bubbleId, BubbleManager::Disconnected);
                mCalls[i].callState=BubbleManager::Disconnected;
            }
            bubbleManager().endChanges();
            updateToolBarActions();
        }
        break;
    }
    
    case '7': // create/join conference
    {
        int activeCallId = bubbleIdByState(BubbleManagerIF::Active);
        int heldCallId = bubbleIdByState(BubbleManagerIF::OnHold);
        int i;

        if (conferenceCallExists()&&(activeCallId!=-1)) {
            int j;
            callIndexByBubbleId(mConfBubbleId,j);
            callIndexByBubbleId(activeCallId,i);
            mCalls[i].isInConf = true;
            mCalls[i].callState = BubbleManagerIF::Active;
            mCalls[j].callState = BubbleManagerIF::Active;
            bubbleManager().startChanges();
            bubbleManager().addRowToConference(activeCallId);
            bubbleManager().setExpandedConferenceCallHeader(true);
            bubbleManager().setState(mConfBubbleId,BubbleManagerIF::Active);
            setBubbleActions(mConfBubbleId,BubbleManagerIF::Active);
            bubbleManager().endChanges();            
        } else if (activeCallId!=-1 && heldCallId!=-1) {
            bubbleManager().startChanges();
            mConfBubbleId = bubbleManager().createConference(heldCallId, activeCallId);
            bubbleManager().setExpandedConferenceCallHeader(true);
            bubbleManager().setState(mConfBubbleId,BubbleManagerIF::Active);
            bubbleManager().setCli(mConfBubbleId,"Conference call",Qt::ElideRight);
            if (mCallTimer->isChecked()) {
                bubbleManager().setCallTime( mConfBubbleId, "00:00:00" );
            }
            bubbleManager().addAction(mConfBubbleId, mMute);
            bubbleManager().addAction(mConfBubbleId, mEndConference);
            bubbleManager().clearParticipantListActions();
            bubbleManager().addParticipantListAction(mPrivate);
            bubbleManager().addParticipantListAction(mDrop);
            bubbleManager().setState(heldCallId,BubbleManagerIF::Active);
            bubbleManager().setCallFlag(mConfBubbleId, BubbleManager::NoCiphering,
                                        mCipheringOff->isChecked());
            bubbleManager().endChanges();

            callIndexByBubbleId(activeCallId,i);
            mCalls[i].isInConf = true;
            mCalls[i].callState = BubbleManagerIF::Active;
            callIndexByBubbleId(heldCallId,i);
            mCalls[i].isInConf = true;
            mCalls[i].callState = BubbleManagerIF::Active;

            // store the call
            TestCall call;
            call.bubbleId = mConfBubbleId;
            call.callState = BubbleManagerIF::Active;
            call.isConf = true;
            call.isInConf = false;
            mCalls.append(call);
        }
        break;
    }

    case '9': // conference shortcut
    {
        conferenceWizard();
        break;
    }

    case '0': // updates
    case Qt::Key_Space: // updates
    {
        if (mCallTimer->isChecked()) {
            foreach (TestCall call, mCalls) {
                if ( call.callState == BubbleManager::Active ||
                     call.callState == BubbleManager::OnHold ) {
                    bubbleManager().updateCallTime(call.bubbleId,"00:00:01");
                }
            }
        }
        break;
    }

    case 'C': { // ciphering
        mCipheringOff->setChecked(!mCipheringOff->isChecked());
        break;
    }

    case 'D': { // divert
        mCallDivert->setChecked(!mCallDivert->isChecked());
        break;
    }

    case 'I': { // image
        mContactPicture->setChecked(!mContactPicture->isChecked());
        break;
    }

    case 'N': { // remote name
        mContactName->setChecked(!mContactName->isChecked());
        break;
    }

    case 'T': { // timer
        mCallTimer->setChecked(!mCallTimer->isChecked());
        break;
    }

    default:
    break;
    }
}

bool BubbleTestView::initializingCallExists() const
{
    bool result = false;
    foreach(TestCall call, mCalls) {
        if (call.callState>=BubbleManagerIF::Incoming) {
            result = true;
            break;
        }
    }

    return result;
}

bool BubbleTestView::conferenceCallExists() const
{
    bool result = false;
    QListIterator<TestCall> i(mCalls);
    while (i.hasNext()) {
        if (i.next().isConf) {
            result = true;
            break;
        }
    }
    return result;
}

int BubbleTestView::callCount() const
{
    int count = 0;
    foreach(TestCall call, mCalls) {
        if (!call.isInConf) {
            count++;
        }
    }

    return count;
}

bool BubbleTestView::callIndexByState(
    BubbleManagerIF::PhoneCallState state, int& index)
{
    bool result = false;
    int i = 0;
    foreach(TestCall call, mCalls) {
        if (call.callState==state && !call.isInConf) {
            result = true;
            index = i;
            break;
        }
        i++;
    }

    return result;
}

bool BubbleTestView::callIndexByBubbleId(int bubbleId, int& index)
{
    bool result = false;
    int i = 0;
    foreach(TestCall call, mCalls) {
        if (call.bubbleId==bubbleId) {
            result = true;
            index = i;
            break;
        }
        i++;
    }

    return result;
}

BubbleManagerIF& BubbleTestView::bubbleManager()
{
    return *mBubbleManager;
}

void BubbleTestView::setBubbleData(int bubble, BubbleManagerIF::PhoneCallState state)
{
    bubbleManager().setState(bubble, state);

    switch (state) {
    case BubbleManager::Incoming:
        {
        if ( mContactName->isChecked() ) {
            bubbleManager().setCli(bubble, "Gloria Andersson", Qt::ElideRight);
            bubbleManager().setSecondaryCli( bubble, "+35850123456789" );
        } else {
            bubbleManager().setCli(bubble, "+35850123456789", Qt::ElideLeft);
        }

        setCallObject(bubble, ":resources/contactpic.jpg");
        bubbleManager().setNumberType(bubble, BubbleManager::Mobile);
        bubbleManager().setCallFlag(bubble, BubbleManager::Diverted, mCallDivert->isChecked());
        bubbleManager().setCallFlag(bubble, BubbleManager::NoCiphering, mCipheringOff->isChecked());
        bubbleManager().setLabel(bubble, "calling", Qt::ElideRight);
        break;
        }

    case BubbleManager::Waiting:
        {
        if ( mContactName->isChecked() ) {
            if (callCount()==2) {
                bubbleManager().setCli(bubble, "Mary Poppins", Qt::ElideRight);
                bubbleManager().setSecondaryCli(bubble, "+35840987654321");
            } else {
                bubbleManager().setCli(bubble, "John Doe", Qt::ElideRight);
                bubbleManager().setSecondaryCli(bubble, "+35840987654321");
            }
        } else {
            if (callCount()==2) {
                bubbleManager().setCli(bubble, "+35850232323232", Qt::ElideLeft);
            } else {
                bubbleManager().setCli(bubble, "+35840987654321", Qt::ElideLeft);
            }

        }
        setCallObject(bubble, ":resources/contactpic3.png");
        bubbleManager().setNumberType(bubble, BubbleManager::Mobile);
        bubbleManager().setCallFlag(bubble, BubbleManager::Diverted, mCallDivert->isChecked());
        bubbleManager().setCallFlag(bubble, BubbleManager::NoCiphering, mCipheringOff->isChecked());
        bubbleManager().setLabel(bubble, "waiting", Qt::ElideRight);
        break;
        }

    case BubbleManager::Outgoing:
        {
        bubbleManager().setCallFlag(bubble, BubbleManager::NoCiphering, mCipheringOff->isChecked());

        if (mEmergencyCall->isChecked()) {
            bubbleManager().setCli( bubble, "emergency call", Qt::ElideRight );
            bubbleManager().setLabel( bubble, "Attempting", Qt::ElideRight );
            bubbleManager().setCallFlag( bubble, BubbleManager::EmergencyCall, true);
            bubbleManager().setCallObjectFromTheme(bubble);
        } else {
            if ( mContactName->isChecked() ) {
                bubbleManager().setCli( bubble, "Bart Simpson", Qt::ElideRight );
                bubbleManager().setSecondaryCli( bubble, "+35890987654321" );
            } else {
                bubbleManager().setCli( bubble, "+35890987654321", Qt::ElideRight );
            }
            setCallObject(bubble, ":resources/contactpic2.jpg");
            bubbleManager().setLabel( bubble, "Calling", Qt::ElideRight );
        }
        break;
        }

    case BubbleManager::Alerting:
        {
        if ( mEmergencyCall->isChecked() ) {
            bubbleManager().setCli( bubble, "Emergency call", Qt::ElideRight );
        }
        break;
        }

    case BubbleManager::Active:
        {
        if ( mCallTimer->isChecked() ) {
            bubbleManager().setCallTime( bubble, "00:00:00" );
        }
        break;
        }

    case BubbleManager::OnHold:
        {
        bubbleManager().setLabel( bubble, "on hold", Qt::ElideRight );
        break;
        }

    case BubbleManager::Disconnected:
        {
        bubbleManager().setLabel( bubble, "disconnected", Qt::ElideRight );
        break;
        }

    default:
        {
        break;
        }
    }
}

void BubbleTestView::setCallObject(int bubble, const QString& filename)
{
    if (mContactPicture->isChecked()) {
        bubbleManager().setCallObjectImage(bubble, filename);
    } else {
        bubbleManager().setCallObjectFromTheme(bubble);
    }
}

void BubbleTestView::setBubbleActions(int bubble, BubbleManagerIF::PhoneCallState state )
{
    if (bubble==mConfBubbleId) {
        bubbleManager().clearActions(bubble);
        bubbleManager().addAction(bubble, mMute);
        bubbleManager().addAction(bubble, mEndConference);
        return;
    }

    bubbleManager().clearActions(bubble);

    switch (state) {
    case BubbleManager::Incoming:
    case BubbleManager::Waiting:
        {
        if (callCount()==2) {
            bubbleManager().addAction( bubble, mReplace );
        } else {
            bubbleManager().addAction( bubble, mAnswer );
            bubbleManager().addAction( bubble, mReject );
        }
        break;
        }
    case BubbleManager::Outgoing:
    case BubbleManager::Alerting: // flow through
        {
        bubbleManager().addAction( bubble, mMute );
        bubbleManager().addAction( bubble, mEndCall );
        break;
        }
    case BubbleManager::OnHold:
        {
        if (mMuted) {
            bubbleManager().addAction( bubble, mUnmute );
        } else {
            bubbleManager().addAction( bubble, mMute );
        }
        bubbleManager().addAction( bubble, mEndCall );
        }
        break;
    default: // Active call
        {
        if (mMuted) {
            bubbleManager().addAction( bubble, mUnmute );
        } else {
            bubbleManager().addAction( bubble, mMute );
        }
        bubbleManager().addAction( bubble, mEndCall );
        break;
        }
    }
}

void BubbleTestView::setExpandActions()
{
    if (callCount() >= 2) {
        int i;
        int j;
        int heldBubble = -1;
        if (callIndexByState(BubbleManagerIF::Active,i) &&
            callIndexByState(BubbleManagerIF::OnHold,j)) {
            heldBubble = mCalls[j].bubbleId;
            bubbleManager().setExpandAction(heldBubble, mSwap);
        } else if (callIndexByState(BubbleManagerIF::Waiting,i) &&
                   callIndexByState(BubbleManagerIF::OnHold,j)) {
            heldBubble = mCalls[j].bubbleId;
            bubbleManager().setExpandAction(heldBubble, mUnhold);
        }

        // for rest set update toolbar action
        QListIterator<TestCall> calls(mCalls);
        while (calls.hasNext()) {
            TestCall call = calls.next();
            if (!call.isInConf && call.bubbleId!=heldBubble) {
                bubbleManager().setExpandAction(call.bubbleId, mUpdateUiControls);
            }
        }
    }
}

void BubbleTestView::answerCall()
{
    QKeyEvent event( QEvent::KeyPress, '3', 0 );
    keyPressEvent(&event);
}

void BubbleTestView::endCall()
{
    QKeyEvent event( QEvent::KeyPress, '5', 0 );
    keyPressEvent(&event);
    keyPressEvent(&event);
}

void BubbleTestView::endConferenceCall()
{
    QKeyEvent event( QEvent::KeyPress, '5', 0 );
    keyPressEvent(&event);
}

void BubbleTestView::rejectCall()
{
    QKeyEvent event( QEvent::KeyPress, '5', 0 );
    keyPressEvent(&event);
    keyPressEvent(&event);
}

void BubbleTestView::toggleHold()
{
    if (callCount() > 1) {
        QTimer::singleShot(1000, this, SLOT(toggleHoldDelayed()));
    } else {
        QKeyEvent event( QEvent::KeyPress, '4', 0 );
        keyPressEvent(&event);
    }
}

void BubbleTestView::toggleHoldDelayed()
{
    QKeyEvent event( QEvent::KeyPress, '4', 0 );
    keyPressEvent(&event);
}

void BubbleTestView::createIncomingCall()
{
    QKeyEvent event( QEvent::KeyPress, '1', 0 );
    keyPressEvent(&event);
    setFocus(); // hack because toolbar steals the focus
}

void BubbleTestView::createOutgoingCall()
{
    QKeyEvent event( QEvent::KeyPress, '2', 0 );
    keyPressEvent(&event);
    setFocus(); // hack because toolbar steals the focus
}

void BubbleTestView::setMuted()
{
    mMuted = !mMuted;
    int bubble = bubbleManager().expandedBubble();
    QListIterator<TestCall> calls(mCalls);
    while (calls.hasNext()) {
        TestCall call = calls.next();
        if (call.bubbleId==bubble) {
            bubbleManager().startChanges();
            setBubbleActions(bubble, (BubbleManagerIF::PhoneCallState)call.callState);
            bubbleManager().endChanges();
        }
    }

    bubbleManager().setPhoneMuted( mMuted );
    setFocus(); // hack because toolbar steals the focus
}

void BubbleTestView::joinToConference()
{
    QKeyEvent event( QEvent::KeyPress, '7', 0 );
    keyPressEvent(&event);
}

void BubbleTestView::handlePrivate()
{
    int i;
    qDebug() << "Handle private";
    int selection = bubbleManager().selectionIdInConference();
    qDebug() << "bubble:" << selection;
    bubbleManager().startChanges();
    if (bubbleManager().conferenceRowCount()>2) {
        bubbleManager().removeRowFromConference(selection);
        bubbleManager().setExpandedConferenceCallHeader(false);
        callIndexByBubbleId(selection,i);
        mCalls[i].isInConf = false;
        setBubbleActions(mCalls[i].bubbleId,BubbleManagerIF::Active);
        setBubbleData(mConfBubbleId,BubbleManagerIF::OnHold);
        setExpandActions();
    } else {
        bubbleManager().removeConference();
        callIndexByBubbleId(mConfBubbleId,i);
        mCalls.removeAt(i);
        mConfBubbleId = -1;
        callIndexByBubbleId(selection,i);
        int held = (i==0) ? 1 : 0; // ??
        setBubbleActions(selection,BubbleManagerIF::Active);
        setBubbleData(selection,BubbleManagerIF::Active);
        setBubbleActions(held,BubbleManagerIF::OnHold);
        setBubbleData(held,BubbleManagerIF::OnHold);
        mCalls[i].isInConf = false;
        mCalls[i].callState = BubbleManagerIF::Active;
        int callId;
        callIndexByBubbleId(held, callId);
        mCalls[callId].callState = BubbleManagerIF::OnHold;
        mCalls[callId].isInConf = false;
        setExpandActions();
    }
    bubbleManager().endChanges();
}

void BubbleTestView::handleDrop()
{
    int i;
    qDebug() << "Handle drop";
    int selection = bubbleManager().selectionIdInConference();
    qDebug() << "bubble:" << selection;
    bubbleManager().startChanges();
    if (bubbleManager().conferenceRowCount()>2) {
        bubbleManager().removeRowFromConference(selection);
        bubbleManager().removeCallHeader(selection);
        callIndexByBubbleId(selection,i);
        mCalls.removeAt(i);
    } else {
        bubbleManager().removeConference();
        callIndexByBubbleId(mConfBubbleId,i);
        mCalls.removeAt(i);
        mConfBubbleId = -1;
        bubbleManager().removeCallHeader(selection);
        callIndexByBubbleId(selection,i);
        mCalls.removeAt(i);
        mCalls[0].isInConf = false;
        setBubbleActions(mCalls[0].bubbleId,BubbleManagerIF::Active);
        setExpandActions();
    }
    bubbleManager().endChanges();
}

void BubbleTestView::replaceActiveCall()
{
    QKeyEvent event( QEvent::KeyPress, '3', 0 );
    keyPressEvent(&event);
}

void BubbleTestView::updateUiControls()
{
    if (conferenceCallExists()) {
        bubbleManager().startChanges();
        bubbleManager().setExpandedConferenceCallHeader(
            bubbleManager().expandedBubble()==mConfBubbleId );
        bubbleManager().endChanges();
    }

    updateToolBarActions();
}

void BubbleTestView::createToolBarActions()
{
    mCallIn = new HbAction("Call in", this);
    connect( mCallIn, SIGNAL(triggered()), this, SLOT(createIncomingCall()), Qt::QueuedConnection );

    mCallOut = new HbAction("Call out", this);
    connect( mCallOut, SIGNAL(triggered()), this, SLOT(createOutgoingCall()), Qt::QueuedConnection );

    mMute = new HbAction(HbIcon("qtg_mono_mic_mute"), "", this);
    connect( mMute, SIGNAL(triggered()), this, SLOT(setMuted()), Qt::QueuedConnection );

    mUnmute = new HbAction(HbIcon("qtg_mono_mic_unmute"), "", this);
    connect( mUnmute, SIGNAL(triggered()), this, SLOT(setMuted()), Qt::QueuedConnection );

    // not connected to anywhere
    mActivateLoudspeaker = new HbAction(HbIcon("qtg_mono_speaker"), "", this);
    connect( mActivateLoudspeaker, SIGNAL(triggered()), this, SLOT(createIncomingCall()), Qt::QueuedConnection );
    mActivateHandset = new HbAction(HbIcon("qtg_mono_mobile"), "", this);
    mSendMessage = new HbAction(HbIcon("qtg_mono_send"), "", this);
    mSilence = new HbAction(HbIcon("qtg_mono_speaker_off"), "", this);
    mOpenDialer = new HbAction(HbIcon("qtg_mono_dialer"), "", this);
    mOpenContacts = new HbAction(HbIcon("qtg_mono_contacts"), "", this);

    updateToolBarActions();
}

void BubbleTestView::createMenuActions()
{
    // HbMenu* typeMenu = menu()->addMenu("Call type"); current submenu layout sucks
    menu()->addAction("Switch orientation",this,SLOT(switchOrientation()));
    mCallDivert = menu()->addAction("Diverted call");
    mCallTimer = menu()->addAction("Call timer");
    mContactName = menu()->addAction("Contact name");
    mContactPicture = menu()->addAction("Contact picture");
    mCipheringOff = menu()->addAction("Ciphering off");
    mEmergencyCall = menu()->addAction( "Emergency call" );    
    HbAction* exit = menu()->addAction( "Exit" );
    mCallDivert->setCheckable(true);
    mCallTimer->setCheckable(true);
    mContactPicture->setCheckable(true);
    mContactName->setCheckable(true);
    mCipheringOff->setCheckable(true);
    mEmergencyCall->setCheckable(true);
    connect( exit, SIGNAL(triggered()), qApp, SLOT(quit()) );
}

void BubbleTestView::updateToolBarActions()
{
    toolBar()->clearActions();

    if (!mCalls.count()) {
        toolBar()->addAction(mCallIn);
        toolBar()->addAction(mCallOut);
        return;
    }

    int bubble = bubbleManager().expandedBubble();
    int i;
    callIndexByBubbleId(bubble,i);
    int state = mCalls[i].callState;

    if ( state == BubbleManagerIF::Incoming ||
         state == BubbleManagerIF::Waiting ) {
        toolBar()->addAction(mSendMessage);
        toolBar()->addAction(mSilence);
    } else {
        toolBar()->addAction(mActivateLoudspeaker);

        if (callCount()==1) {
            toolBar()->addAction(mHold);
        } else {
            toolBar()->addAction(mSwap);            
        }

        if (callCount()==1) {
            toolBar()->addAction(mOpenContacts);
        } else {
            toolBar()->addAction(mJoin);
        }

        toolBar()->addAction(mOpenDialer);
    }
}

void BubbleTestView::createBubbleActions()
{
    mAnswer = new HbAction( HbIcon("qtg_mono_call"),QString(), this);
    mAnswer->setSoftKeyRole(QAction::PositiveSoftKey);
    connect( mAnswer, SIGNAL( triggered() ), this, SLOT( answerCall() ) );

    mReject= new HbAction( HbIcon("qtg_mono_reject_call"),QString(), this);
    mReject->setSoftKeyRole(QAction::NegativeSoftKey);
    connect( mReject, SIGNAL( triggered() ), this, SLOT( rejectCall() ) );

    mHold = new HbAction( HbIcon("qtg_mono_hold_call"),QString(), this);
    connect( mHold, SIGNAL( triggered() ), this, SLOT( toggleHold() ) );

    mUnhold = new HbAction( HbIcon("qtg_mono_call"),QString(), this);
    connect( mUnhold, SIGNAL( triggered() ), this, SLOT( toggleHold() ) );

    mSwap = new HbAction( HbIcon("qtg_mono_hold_call"),QString(), this);
    connect( mSwap, SIGNAL( triggered() ), this, SLOT( toggleHold() ) );

    mEndCall = new HbAction(HbIcon("qtg_mono_end_call"),QString(), this);
    mEndCall->setSoftKeyRole(QAction::NegativeSoftKey);
    connect( mEndCall, SIGNAL( triggered() ), this, SLOT( endCall() ) );

    mEndConference = new HbAction(HbIcon("qtg_mono_end_call"),QString(), this);
    mEndConference->setSoftKeyRole(QAction::NegativeSoftKey);
    connect( mEndConference, SIGNAL( triggered() ), this, SLOT( endConferenceCall() ) );

    mJoin = new HbAction(HbIcon("qtg_mono_join_call"),QString(), this);
    connect( mJoin, SIGNAL( triggered() ), this, SLOT(joinToConference()) );

    mPrivate = new HbAction(HbIcon("qtg_mono_private_call"),QString(), this);
    connect( mPrivate, SIGNAL( triggered() ), this, SLOT(handlePrivate()) );

    mDrop = new HbAction(HbIcon("qtg_mono_drop_call"),QString(), this);
    connect( mDrop, SIGNAL( triggered() ), this, SLOT(handleDrop()) );

    mReplace = new HbAction(HbIcon("qtg_mono_replace_call"),QString(), this);
    connect( mReplace, SIGNAL( triggered() ), this, SLOT(replaceActiveCall()) );

    mUpdateUiControls = new HbAction(QString(), this);
    connect( mUpdateUiControls, SIGNAL(triggered()), this, SLOT(updateUiControls()) );
}

int BubbleTestView::bubbleIdByState(BubbleManagerIF::PhoneCallState state)
{
    int bubbleId=-1;
    foreach(TestCall call, mCalls) {
        if ((call.callState==state) && !call.isInConf) {
            bubbleId = call.bubbleId;
            break;
        }
    }

    return bubbleId;
}

void BubbleTestView::conferenceWizard() {
    bubbleManager().startChanges();

    for (int i=0; i<5; i++) {
        sendKeyEvent('1');
        sendKeyEvent('3');
        sendKeyEvent('7');
    }

    bubbleManager().endChanges();
}

void BubbleTestView::sendKeyEvent(int key)
{
    QKeyEvent event( QEvent::KeyPress, key, 0 );
    keyPressEvent(&event);
}

void BubbleTestView::switchOrientation()
{
    if (mMainWindow.orientation()==Qt::Vertical) {
        mMainWindow.setOrientation(Qt::Horizontal);

    } else {
        mMainWindow.setOrientation(Qt::Vertical);
    }
}

void BubbleTestView::handleOrientationChange(Qt::Orientation orientation)
{
    if (orientation==Qt::Horizontal) {
        toolBar()->setOrientation(Qt::Horizontal);
    }

    mBubbleManager->handleOrientationChange(orientation);
}

void BubbleTestView::toggleHoldConference()
{
    int i;
    if (callIndexByBubbleId(mConfBubbleId, i) ) {
        BubbleManagerIF::PhoneCallState state =
            (mCalls[i].callState == BubbleManagerIF::Active) ?
            BubbleManagerIF::OnHold : BubbleManagerIF::Active;
        bubbleManager().startChanges();
        setBubbleData(mCalls[i].bubbleId, state);
        setBubbleActions(mCalls[i].bubbleId, state );
        qDebug() << "toggleHoldConference:" << state;
        mCalls[i].callState=state;
        bubbleManager().endChanges();
        // update participants
        QTimer::singleShot(500, this, SLOT(toggleConferenceHoldDelayed()));
    }
}

void BubbleTestView::toggleConferenceHoldDelayed()
{
    int i;
    callIndexByBubbleId(mConfBubbleId, i);
    BubbleManagerIF::PhoneCallState confState =
        (BubbleManagerIF::PhoneCallState)mCalls[i].callState;
    // qDebug() << "toggleConferenceHoldDelayed state:" << confState;

    QListIterator<TestCall> calls(mCalls);
    int j=0;
    while(calls.hasNext()) {
        TestCall call = calls.next();
        // qDebug() << "toggleConferenceHoldDelayed callState:" << call.callState;
        if (call.isInConf && call.callState!=confState) {
            bubbleManager().startChanges();
            setBubbleData(call.bubbleId, confState);
            setBubbleActions(call.bubbleId, confState);
            mCalls[j].callState=confState;
            bubbleManager().endChanges();
            // next
            QTimer::singleShot(500, this, SLOT(toggleConferenceHoldDelayed()));
            break;
        }
        j++;
    }
}

void BubbleTestView::loadTestabilityPlugin()
{
#ifdef BUBBLETEST_ENABLE_TESTABILITY
    QString testabilityPlugin = "testability/testability";
    QString testabilityPluginPostfix = ".dll";

    testabilityPlugin = QLibraryInfo::location(QLibraryInfo::PluginsPath) +
                                               QObject::tr("/") + testabilityPlugin +
                                               testabilityPluginPostfix;

    QPluginLoader loader(testabilityPlugin.toLatin1().data());

    QObject *plugin = loader.instance();
    if (plugin) {
            qDebug("Testability plugin loaded successfully!");
            mTestabilityInterface = qobject_cast<TestabilityInterface *>(plugin);

            if (mTestabilityInterface) {
                    qDebug("Testability interface obtained!");
                    mTestabilityInterface->Initialize();
            } else {
                    qDebug("Failed to get testability interface!");
            }
    } else {
            qDebug("Testability plugin %s load failed with error:%s",
                   testabilityPlugin.toLatin1().data(),
                   loader.errorString().toLatin1().data());
    }
#endif
}

