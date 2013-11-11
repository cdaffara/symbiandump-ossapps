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
* Description:  Phone UI Qt view's interface.
*
*/

#ifndef PHONEUIQTVIEWIF_STUB_H
#define PHONEUIQTVIEWIF_STUB_H

#include <bubblemanagerif.h>
#include <phoneuiqtviewif.h>

class QKeyEvent;
class QString;
class HbIcon;
class HbAction;
class HbMenu;
class PhoneAction;

class PhoneUIQtViewIFStub : public PhoneUIQtViewIF
{

public:
    
    PhoneUIQtViewIFStub(BubbleManagerIF& bubbleManager):mBubbleManager(bubbleManager) {};
    
    virtual ~PhoneUIQtViewIFStub() {};
        
    /*!
        \fn void PhoneUIQtViewIF::bubbleManager ()
        
        Returns reference to BubbleManager's interface.
    */
    BubbleManagerIF& bubbleManager () {return mBubbleManager;};
    
    /*!
        \fn void PhoneUIQtViewIF::addBubbleCommand ()
        
        Adds new command to bubble. Command id will be emitted as
        a parameter from view's command(int) signal.
    */
    void addBubbleCommand ( 
        int bubbleId,
        const PhoneAction& action ) {};
    
    /*!
        \fn void PhoneUIQtViewIF::clearBubbleCommands ()
        
        Clears commands from bubble.
    */
    void clearBubbleCommands (int bubbleId){};

    /*!
        \fn void PhoneUIQtView::addParticipantListAction ()
        
        Adds new patricipant list command to conference bubble. 
    */
    void addParticipantListAction(
            int commandId,  
            const QString &text, 
            const HbIcon &icon) {};

    /*!
        \fn void PhoneUIQtView::clearBubbleCommands ()
        
        Clears participant list commands from conference bubble.
    */
    void clearParticipantListActions() {};
    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
        Sets toolbar actions.
    */
    void setToolbarActions(const QList<PhoneAction*>& actions) {};    
    
    /*!
        \fn void PhoneUIQtViewIF::hideToolbar ()
        
        Hides toolbar.
    */
    void hideToolbar () {};
    
    /*!
        \fn void PhoneUIQtViewIF::showToolbar ()
        
        Shows toolbar.
    */
    void showToolbar (){};
    
    /*!
        \fn int PhoneUIQtViewIF::volumeSliderValue ()
        
        Returns HbVolumeSliderPopup's volume value.
        If this volume slider isn't active -1 is returned.
    */
    int volumeSliderValue () {};
    
    /*!
        \fn void PhoneUIQtViewIF::removeVolumeSlider ()
        
        Removes HbVolumeSliderPopup.
    */
    void removeVolumeSlider () {};

    /*!
        \fn void PhoneUIQtViewIF::setVolumeSliderValue (int value)
        
        Sets slider value to HbVolumeSliderPopup.
    */
    void setVolumeSliderValue (
            int value, 
            int commandId, 
            int maxVolumeValue, 
            int minVolumeValue){};

    /*!
        \fn void PhoneUIQtViewIF::setExpandAction ()
        
        Sets expand action.
    */
    void setExpandAction(int bubbleId, int commandId) {};
    
    /*!
        \fn void PhoneUIQtViewIF::removeExpandAction ()
        
        Removes expand action.
    */      
    void removeExpandAction(int bubbleId){};

    /*!
        \fn void PhoneUIQtView::showDialpad()
        
        Shows dialpad widget.
    */    
    void showDialpad() {};
    
    /*!
        \fn void PhoneUIQtView::hideDialpad()
        
        Hides dialpad widget.
    */    
    void hideDialpad(){};
    
    /*!
        \fn void PhoneUIQtView::isDialpadVisible()
        
        Check is dialpad visible.
    */    
    bool isDialpadVisible() {};

    /*!
        \fn void PhoneUIQtView::dialpadString()
        
        Returns content of the dialpad.
    */  
    QString dialpadText(){};
    
    /*!
        \fn void PhoneUIQtView::dialpadString()
        
        Clears and hides dialpad.
    */
    void clearAndHideDialpad() {};
    
    /*!
        \fn void PhoneUIQtView::clearDialpad()
        
        Clears text from dialpad.
    */
    void clearDialpad(){};
    
    /*!
        \fn void PhoneUIQtView::bringToForeground()
        
        Brings to foreground.
    */
    void bringToForeground() {};
    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
       Sets menu actions.
    */
    void setMenuActions(const QList<PhoneAction*>& actions) {};
      
    /*!
        \fn void PhoneUIQtView::shutdownPhoneApp()
        
        Shutdown phone application.
    */
    void shutdownPhoneApp(){};
    
    /*
       \fn void PhoneUIQtView::setBackButtonVisible()
       
       Set the Back command visibility in TitleBar
    */
    void setBackButtonVisible(bool visible){};
	    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
       Returns HbMenu reference.
    */
    HbMenu &menuReference() {};
    
    /*!
        \fn void PhoneUIQtViewIF::captureKey()
        
        Captures key.
    */
    void captureKey(Qt::Key key, bool capture){};
    
    /*
        \fn void setRestrictedMode()
        
        Sets Ui to restricted mode. Decides which keyevent are allowed.
     */
    void setRestrictedMode(bool restrictedMode){};
    
    BubbleManagerIF& mBubbleManager;
};

#endif // PHONEUIQTVIEWIF_H
