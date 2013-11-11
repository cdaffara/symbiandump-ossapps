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

#ifndef PHONEUIQTVIEWIF_H
#define PHONEUIQTVIEWIF_H

#include <bubblemanagerif.h>

class QKeyEvent;
class QString;
class HbIcon;
class HbAction;
class HbMenu;
class PhoneAction;

class PhoneUIQtViewIF
{

public:
        
    /*!
        \fn void PhoneUIQtViewIF::bubbleManager ()
        
        Returns reference to BubbleManager's interface.
    */
    virtual BubbleManagerIF& bubbleManager () = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::addBubbleCommand ()
        
        Adds new command to bubble. Command id will be emitted as
        a parameter from view's command(int) signal.
    */
    virtual void addBubbleCommand ( 
        int bubbleId,
        const PhoneAction& action ) = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::clearBubbleCommands ()
        
        Clears commands from bubble.
    */
    virtual void clearBubbleCommands (int bubbleId) = 0;

    /*!
        \fn void PhoneUIQtView::addParticipantListAction ()
        
        Adds new patricipant list command to conference bubble. 
    */
    virtual void addParticipantListAction(
            int commandId,  
            const QString &text, 
            const HbIcon &icon) = 0;

    /*!
        \fn void PhoneUIQtView::clearBubbleCommands ()
        
        Clears participant list commands from conference bubble.
    */
    virtual void clearParticipantListActions() = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
        Sets toolbar actions.
    */
    virtual void setToolbarActions(const QList<PhoneAction*>& actions) = 0;    
    
    /*!
        \fn void PhoneUIQtViewIF::hideToolbar ()
        
        Hides toolbar.
    */
    virtual void hideToolbar () = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::showToolbar ()
        
        Shows toolbar.
    */
    virtual void showToolbar () = 0;
    
    /*!
        \fn int PhoneUIQtViewIF::volumeSliderValue ()
        
        Returns HbVolumeSliderPopup's volume value.
        If this volume slider isn't active -1 is returned.
    */
    virtual int volumeSliderValue () = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::removeVolumeSlider ()
        
        Removes HbVolumeSliderPopup.
    */
    virtual void removeVolumeSlider () = 0;

    /*!
        \fn void PhoneUIQtViewIF::setVolumeSliderValue (int value)
        
        Sets slider value to HbVolumeSliderPopup.
    */
    virtual void setVolumeSliderValue (
            int value, 
            int commandId, 
            int maxVolumeValue, 
            int minVolumeValue) = 0;

    /*!
        \fn void PhoneUIQtViewIF::setExpandAction ()
        
        Sets expand action.
    */
    virtual void setExpandAction(int bubbleId, int commandId) = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::removeExpandAction ()
        
        Removes expand action.
    */      
    virtual void removeExpandAction(int bubbleId) = 0;

    /*!
        \fn void PhoneUIQtView::showDialpad()
        
        Shows dialpad widget.
    */    
    virtual void showDialpad() = 0;
    
    /*!
        \fn void PhoneUIQtView::hideDialpad()
        
        Hides dialpad widget.
    */    
    virtual void hideDialpad() = 0;
    
    /*!
        \fn void PhoneUIQtView::isDialpadVisible()
        
        Check is dialpad visible.
    */    
    virtual bool isDialpadVisible() = 0;

    /*!
        \fn void PhoneUIQtView::dialpadString()
        
        Returns content of the dialpad.
    */  
    virtual QString dialpadText() = 0;
    
    /*!
        \fn void PhoneUIQtView::dialpadString()
        
        Clears and hides dialpad.
    */
    virtual void clearAndHideDialpad() = 0;
    
    /*!
        \fn void PhoneUIQtView::clearDialpad()
        
        Clears text from dialpad.
    */
    virtual void clearDialpad() = 0;
    
    /*!
        \fn void PhoneUIQtView::bringToForeground()
        
        Brings to foreground.
    */
    virtual void bringToForeground() = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
       Sets menu actions.
    */
    virtual void setMenuActions(const QList<PhoneAction*>& actions) = 0;
      
    /*!
        \fn void PhoneUIQtView::shutdownPhoneApp()
        
        Shutdown phone application.
    */
    virtual void shutdownPhoneApp() = 0;
    
    /*
       \fn void PhoneUIQtView::setBackButtonVisible()
       
       Set the Back command visibility in TitleBar
    */
    virtual void setBackButtonVisible(bool visible) = 0;
	    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
       Returns HbMenu reference.
    */
    virtual HbMenu &menuReference() = 0;
    
    /*!
        \fn void PhoneUIQtViewIF::captureKey()
        
        Captures key.
    */
    virtual void captureKey(Qt::Key key, bool capture) = 0;
    
    /*
        \fn void setRestrictedMode()
        
        Sets Ui to restricted mode. Decides which keyevent are allowed.
     */
    virtual void setRestrictedMode(bool restrictedMode) = 0;
};

#endif // PHONEUIQTVIEWIF_H
