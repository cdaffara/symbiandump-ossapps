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
* Description:  Phone UI's symbian adapter for Qt view.
*
*/

#ifndef PHONEUIQTVIEWADAPTER_H
#define PHONEUIQTVIEWADAPTER_H

#ifdef BUILD_PHONEUIQTVIEWADAPTER
#define PHONEUIQTVIEWADAPTER_EXPORT Q_DECL_EXPORT
#else
#define PHONEUIQTVIEWADAPTER_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QMap>
#include <apgwgnam.h>
#include "mphoneviewcommandhandle.h"
#include "phoneaction.h"

class PhoneUIQtViewIF;
class PhoneBubbleWrapper;
class CPhoneRingingToneController;
class PhoneResourceAdapter;
class PhoneUIQtButtonsController;
class PhoneNoteController;
class PhoneMessageController;
class PhoneUiCommandController;
class TelephonyService;
class QKeyEvent;
class PhoneIndicatorController;
class PhoneVisibilityHandler;
class PhoneAppLauncher;
class MPEEngineInfo;
class PhoneCallHeaderManager;

class PHONEUIQTVIEWADAPTER_EXPORT PhoneUIQtViewAdapter : 
    public QObject, 
    public MPhoneViewCommandHandle
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit PhoneUIQtViewAdapter (PhoneUIQtViewIF &view, QObject *parent = 0);
    
    /**
     * Destructor
     */
    virtual ~PhoneUIQtViewAdapter();

    
public: // From MPhoneViewCommandHandle

    /**
    * Execute Command with given Command Id
    * @param aCmdId Command Id
    * @return None
    */
    void ExecuteCommandL (TPhoneViewCommandId aCmdId);

    /**
    * Execute Command with given Command Id  and Call Id
    * @param aCmdId Command Id
    * @param aCallId Call Id
    * @return None
    */
    void ExecuteCommandL (TPhoneViewCommandId aCmdId, TInt aCallId);

     /**
    * Execute Command with given command id, call id and command parameter.
    * @param aCmdId Command Id
    * @param aCommandParam Command parameter
    * @return None
    */
    void ExecuteCommandL (TPhoneViewCommandId aCmdId, TPhoneCommandParam* aCommandParam);

     /**
    * Execute Command with given command id, call id and command parameter.
    * @param aCmdId Command Id
    * @param aCallId Call Id
    * @param aCommandParam Command parameter
    * @return None
    */
    void ExecuteCommandL (TPhoneViewCommandId aCmdId, TInt aCallId,
        TPhoneCommandParam* aCommandParam);

    /**
    * Execute Command with given command id, call id and message.
    * @param aCmdId Command Id
    * @param aCallId Call Id
    * @param aMessage Command message
    */
    void ExecuteCommandL (TPhoneViewCommandId aCmdId, TInt aCallId, TDesC& aMessage);

    /**
    * Handle Command with given Command Id
    * @param aCmdId Command Id
    * @return TPhoneViewResponseId
    */
    TPhoneViewResponseId HandleCommandL (TPhoneViewCommandId aCmdId);

    /**
    * Handle Command with given command id and command parameter.
    * @param aCmdId Command Id
    * @param aCommandParam Command parameter
    * @return TPhoneViewResponseId
    */
    TPhoneViewResponseId HandleCommandL (TPhoneViewCommandId aCmdId, 
        TPhoneCommandParam* aCommandParam);
        
    /**
    * Execute Command with given Command Id
    * @param aCmdId Command Id
    * @return None
    */
    void ExecuteCommand (TPhoneViewCommandId aCmdId);
        
    /**
    * Execute Command with given command id and command parameter.
    * @param aCmdId Command Id
    * @param aCommandParam Command parameter
    * @return None
    */
    void ExecuteCommand (TPhoneViewCommandId aCmdId, TPhoneCommandParam* aCommandParam);
   
    /**
    * Execute Command with given Command Id
    */     
    const TDesC& FetchContent ();
    
    /*!
        \fn void PhoneUIQtViewAdapter::noteController ()
        
        Returns pointer to PhoneNoteController
    */
    PhoneNoteController* noteController() const;

    /*!
        \fn void PhoneUIQtViewAdapter::setEngineInfo ()
        
        Sets engine info.
    */
    void setEngineInfo(MPEEngineInfo* engineInfo);

private slots:

    /*!
        \fn void PhoneUIQtViewAdapter::dialpadClosed ()
        
        PhoneView calls this method when dialpad
        is going to be closed.
    */
    void dialpadClosed();
    
    /*!
        \fn void PhoneUIQtViewAdapter::keyPressed ()
        
        PhoneView calls this method when key press is released.
    */
    void keyReleased(QKeyEvent *event);
    
    /*!
        \fn void PhoneUIQtViewAdapter::handleWindowActivated ()
        
        PhoneView calls this method when window is activated;
    */
    void handleWindowActivated();
    
    /*!
        \fn void PhoneUIQtViewAdapter::handleWindowDeactivated ()
        
        PhoneView calls this method when window is deactivated.
    */
    void handleWindowDeactivated();
    
    /*!
       \ fn void PhoneUIQtViewAdapter::onFocusLost();
     
         This method is called when the foreground application changes.
     */
    void onFocusLost();
    
    /*!
       \ fn void PhoneUIQtViewAdapter::onFocusGained();
       
       This method is called when the foreground application changes.
     */
    void onFocusGained();
    
private:

    /*!
        \fn int PhoneUIQtViewAdapter::createCallHeader()
        
        This method creates new call header by call id.
    */
    void createCallHeader(int callId);

    /*!
        \fn int PhoneUIQtViewAdapter::createCallHeader()
        
        This method creates new emergency call header.
    */
    void createEmergencyCallHeader(int callId);

    /*!
        \fn int PhoneUIQtViewAdapter::updateCallHeader()
        
        This method updates call header.
    */
    void updateCallHeader(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::handleCipheringInfoChange()
        
        This method updates ciphering indicators.
    */
    void handleCipheringInfoChange(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::callIdByState()
        
        This method writes to commandParam the call id matching
        given call state.
        
        Returns EPhoneViewResponseSuccess operation is succesful
        otherwise EPhoneViewResponseFailed.
    */
    TPhoneViewResponseId callIdByState (TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setTouchButtons (TPhoneCmdParam *commandParam)
        
        This method sets buttons to call bubble.        
    */
    void setTouchButtons (TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setToolbarButtons (TPhoneCmdParam *commandParam)
        
        This method sets buttons to toolbar.        
    */
    void setToolbarButtons (TPhoneCommandParam *commandParam);

    /*!
        \fn void PhoneUIQtViewAdapter::writeAudioVolumeLevel (TPhoneCmdParam *commandParam)
        
        This method writes audio volume value into given command parameter.
    */
    void writeAudioVolumeLevel (TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setAudioVolumeSliderValue (TPhoneCmdParam *commandParam)
        
        This method sets volume value to view.
    */
    void setAudioVolumeSliderValue (TPhoneCommandParam *commandParam);

    /*!
        \fn void PhoneUIQtViewAdapter::setMuteIndication (TPhoneCmdParam *commandParam)
        
        This method sets muted indication to view.
    */
    void setMuteIndication (TPhoneCommandParam *commandParam);
    
    
    /*!
        \fn void PhoneUIQtViewAdapter::callHoldFlag (TPhoneCmdParam *commandParam)
        
        This method retrieves number of calls. Conference call counts one call, 
        participant calls are not count.
    */
    void activeCallCount (TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setAudioPath (TPhoneCommandParam *commandParam)
        
        This method sets audio paths
    */
    void setAudioPath (TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::expandedBubbleCallId(TPhoneCommandParam *commandParam)
        
        This method gets call id of the expanded call bubble.
    */
    void expandedBubbleCallId(TPhoneCommandParam *commandParam);

    /*!
        \fn void openDialpad()
        
        This method opens dialpad.
    */
    void openDialpad();

    /*!
        \fn int PhoneUIQtViewAdapter::createConference()
        
        This method creates conference bubble.
    */
    void createConferenceBubble(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::conferenceCallId()
        
        This method checks if call id belongs to conference call.
    */
    void conferenceCallId(int callId, TPhoneCommandParam *commandParam);

    /*!
        \fn int PhoneUIQtViewAdapter::removeConferenceBubble()
        
        This method removes conference bubble.
    */
    void removeConferenceBubble();

    /*!
        \fn int PhoneUIQtViewAdapter::isConference()
        
        This method checks is conference bubble active.
    */
    void isConference(TPhoneCommandParam *commandParam);

    /*!
        \fn int PhoneUIQtViewAdapter::removeCallFromConference()
        
        This method removes a call from conference.
    */
    void removeCallFromConference(int callId);

    /*!
        \fn TPhoneViewResponseId PhoneUIQtViewAdapter::getSelectedConferenceMember()
        
        This method gets call id of selected conference member.
        Used in drop and private commands. 
    */
    TPhoneViewResponseId getSelectedConferenceMember(
            TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setPrivateFromConference()
        
        This method removes call from converence bubble.
        Used in private command. 
    */   
    void setPrivateFromConference(int callId);

    /*!
        \fn void PhoneUIQtViewAdapter::setExpandActions()
        
        This method sets expand action if needed. 
    */ 
    void setExpandActions();

    /*!
        \fn void PhoneUIQtViewAdapter::setParticipantListActions()
        
        This method sets participant list actions if needed. 
    */
    void setParticipantListActions();

    /*!
        \fn void PhoneUIQtViewAdapter::setExpandedConferenceCallHeader()
        
        This method sets conference call header expanded if needed. 
    */
    void setExpandedConferenceCallHeader();
    
    /*!
        \fn void PhoneUIQtViewAdapter::bringToForeground()
        
        This method brings application to foreground.
    */
    void bringToForeground();
    
    /*!
        \fn void PhoneUIQtViewAdapter::hideDeviceDialogs()
        
        This method brings application top of devicedialogs.
    */
    void hideDeviceDialogs(TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::showGlobalNote()
        
        This method shows global note.
    */
    void showGlobalNote(TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::callsExists()
        
        This method checks if there are calls in given states.
    */
    bool callsExists(int call1State, int call2State);
 
    /*!
        \fn void PhoneUIQtViewAdapter::setDialpadVisibility()
        
        This method sets dialpad visibility.
    */
    void setDialpadVisibility(TPhoneCommandParam *commandParam);
 
    /*!
        \fn void PhoneUIQtViewAdapter::removeAllCallHeaders()
        
        This method removes all call headers.
    */
    void removeAllCallHeaders();
 
    /*!
        \fn void PhoneUIQtViewAdapter::getNumberFromDialpad()
        
        This method gets number from dialpad.
    */
    void getNumberFromDialpad(TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::getNumberEntryStringLength()
        
        This method gets text length from the dialpad.
    */
    void getDialpadStringLength(TPhoneCommandParam *commandParam);
    
    /*!
        \fn void PhoneUIQtViewAdapter::getNumberEntryStringLength()
        
        This method clears and removes dialpad from the view.
    */
    void removeDialpad();
    
    void clearDialpad();
    
    /*!
        \fn void PhoneUIQtViewAdapter::addToConference()
        
        This method adds call to conference.
    */
    void addToConference();
    
    /*!
        \fn void PhoneUIQtViewAdapter::setMenu()
        
        This method sets actions for menu.
    */
    void setMenu();
  
    /*!
        \fn void PhoneUIQtViewAdapter::setMenu()
        
        This method sets actions for menu in call handling view.
    */
    void setCallMenu();
    
    /*!
        \fn void PhoneUIQtViewAdapter::setMenu()
        
        This method sets actions for menu in dialpad view.
    */
    void setDialpadMenu();
    
    /*!
        \fn int PhoneUIQtViewAdapter::showNote()
        
        This method shows a note by command params.
    */
    void showNote(TPhoneCommandParam *commandParam);
    
	/*!
	    /fn QString convertDuration(int)
		This method returns the localized call duration
    */
    QString convertDuration(int seconds);
 
    /*!
        /fn void setBubbleSelectionFlag()
        This method sets bubble selection flag.
    */
    void setBubbleSelectionFlag();
    
    /*!
        /fn void setHidden()
        This method sets hidden status.
    */    
    void setHidden(bool hidden);
    
    /*!
        /fn void SetHiddenL()
        This method sets hidden status.
    */
    void SetHiddenL(bool hidden);
    
    /*!
        /fn void openContacts()
        This method opens contacts application.
    */    
    void openContacts();
 
    /*!
        /fn void openLogs()
        This method opens logs/dialer application.
    */ 
    void openLogs(TPhoneCommandParam *commandParam);

    /*!
        /fn void setCommonButtonFlags()
        This method sets common button flags.
    */
    void setCommonButtonFlags();
    
    /*!
        /fn void convertKey()
        This method converts key.
    */
    bool convertKey(TKeyCode symbianKey, Qt::Key &qtKey);
    
private:

    PhoneUIQtViewIF &m_view;
    int m_idleUid;
    PhoneBubbleWrapper *m_bubbleWrapper;
    CPhoneRingingToneController *m_ringingtonecontroller;
    PhoneResourceAdapter *m_resourceAdapter;
    PhoneNoteController *m_noteController;
    TelephonyService *m_telephonyService;
    PhoneUiCommandController *m_uiCommandController;
    PhoneMessageController *m_messageController;
    PhoneIndicatorController *m_indicatorController;
    PhoneCallHeaderManager *m_phoneCallHeaderManager;
    bool m_dialpadAboutToClose;
    bool m_homeScreenToForeground;
	PhoneVisibilityHandler *m_visibilityHandler;
	PhoneAppLauncher *m_appLauncher;
	bool m_clearDialpadOnClose;
	bool m_speakerAsDefaultButton;
	bool m_ringingTonePlaying;
    MPEEngineInfo*  m_engineInfo;
};

#endif // PHONEUIQTVIEWADAPTER_H
