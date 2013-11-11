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

#ifndef BUBBLETESTVIEW_H
#define BUBBLETESTVIEW_H

#include <hbview.h>
#include <bubblemanagerif.h>

class HbAction;
class HbMainWindow;
class QSignalMapper;
class HbToolBar;
class BubbleManager;

QT_BEGIN_NAMESPACE
class TestabilityInterface;
QT_END_NAMESPACE

class BubbleTestView : public HbView
{
    Q_OBJECT

public:

    BubbleTestView(HbMainWindow& window, QGraphicsItem *parent = 0);
    
    virtual ~BubbleTestView();
    
    BubbleManagerIF& bubbleManager();
    
private slots:
    void answerCall();
    void endCall();
    void endConferenceCall();
    void rejectCall();
    void toggleHold();
    void toggleHoldDelayed();
    void toggleConferenceHoldDelayed();
    void createIncomingCall();
    void createOutgoingCall();
    void setMuted();
    void joinToConference();
    void handlePrivate();
    void handleDrop();
    void replaceActiveCall();
    void updateUiControls();
    
    void switchOrientation();
    void handleOrientationChange(Qt::Orientation orientation);

private:
    void keyPressEvent(QKeyEvent *event);
    
    void setBubbleData(int bubble, BubbleManagerIF::PhoneCallState state);
    void setBubbleActions(int bubble, BubbleManagerIF::PhoneCallState state);
    void setExpandActions();
    void setCallObject(int bubble, const QString& filename);
    void createToolBarActions();
    void updateToolBarActions();
    void createMenuActions();
    void createBubbleActions();
    int bubbleIdByState(BubbleManagerIF::PhoneCallState state);
    bool initializingCallExists() const;
    bool conferenceCallExists() const;
    int  callCount() const;
    bool callIndexByState(BubbleManagerIF::PhoneCallState state, int& index);
    bool callIndexByBubbleId(int bubbleId, int& index);
    void conferenceWizard();
    void sendKeyEvent(int key);
    void toggleHoldConference();
    void loadTestabilityPlugin();

private:
    HbMainWindow&     mMainWindow;
    BubbleManager    *mBubbleManager;
    QList<HbAction*>  mBubbleActions;
    
    // Settings
    HbAction          *mCallDivert;
    HbAction          *mCipheringOff;
    HbAction          *mCallTimer;
    HbAction          *mEmergencyCall;
    HbAction          *mContactPicture;
    HbAction          *mContactName;

    // Toolbar actions
    HbAction          *mCallIn;
    HbAction          *mCallOut;
    HbAction          *mMute;
    HbAction          *mUnmute;
    HbAction          *mActivateLoudspeaker;
    HbAction          *mActivateHandset;
    HbAction          *mSendMessage;
    HbAction          *mSilence;
    HbAction          *mOpenDialer;
    HbAction          *mOpenContacts;

    // Bubble actions
    HbAction          *mAnswer;
    HbAction          *mReject;
    HbAction          *mHold;
    HbAction          *mUnhold;
    HbAction          *mSwap;
    HbAction          *mEndCall;
    HbAction          *mEndConference;
    HbAction          *mJoin;
    HbAction          *mPrivate;
    HbAction          *mDrop;
    HbAction          *mReplace;
    HbAction          *mUpdateUiControls;
    
    // Test call data
    struct TestCall {
        int bubbleId;
        int callState;
        bool isConf;
        bool isInConf;

        TestCall() {
            isConf = false;
            isInConf = false;
        }
    };  
    
    QList<TestCall> mCalls;
    bool mMuted;

    HbAction         *mConnectTester;
    QMap<QString, int> mTestBubbleIds;
    int mConfBubbleId;
    bool mProvideJoin;

    TestabilityInterface* mTestabilityInterface;
};

#endif // BUBBLETESTVIEW_H
