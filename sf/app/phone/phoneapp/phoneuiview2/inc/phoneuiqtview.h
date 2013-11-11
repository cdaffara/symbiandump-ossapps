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
* Description:  Phone UI's Qt view.
*
*/

#ifndef PHONEUIQTVIEW_H
#define PHONEUIQTVIEW_H

#include <QObject>
#include <hbview.h>
#include "phoneuiqtviewif.h"
#include <bubblemanagerif.h>
#include <qsysteminfo.h>

class BubbleManager;
class HbAction;
class QSignalMapper;
class HbToolBar;
class HbVolumeSliderPopup;
class Dialpad;
class XQKeyCapture;
class DialpadKeyHandler;

#ifdef BUILD_PHONEUIQTVIEW
#define PHONEUIQTVIEW_EXPORT Q_DECL_EXPORT
#else
#define PHONEUIQTVIEW_EXPORT Q_DECL_IMPORT
#endif

using namespace QtMobility;


class PHONEUIQTVIEW_EXPORT PhoneUIQtView : public HbView, public PhoneUIQtViewIF
{
    Q_OBJECT

public:

    /*!
       Constructor
    */
    PhoneUIQtView (HbMainWindow &window, QGraphicsItem *parent = 0);
    
    /*!
       Destructor
    */
    virtual ~PhoneUIQtView ();
            
    /*!
        \fn void PhoneUIQtView::bubbleManager ()
        
        Returns BubbleManager interface.
    */
    BubbleManagerIF& bubbleManager();
    
    /*!
        \fn void PhoneUIQtView::addBubbleCommand ()
        
        Adds new command to bubble. Command id will be emitted as
        a parameter from view's command(int) signal.
    */
    void addBubbleCommand (int bubbleId, const PhoneAction& action);

    /*!
        \fn void PhoneUIQtView::clearBubbleCommands ()
        
        Clears commands from bubble.
    */
    void clearBubbleCommands (int bubbleId);
    
    /*!
        \fn void PhoneUIQtView::addParticipantListAction ()
        
        Adds new patricipant list command to conference bubble. 
    */
    void addParticipantListAction(int commandId, const QString &text, const HbIcon &icon);

    /*!
        \fn void PhoneUIQtView::clearBubbleCommands ()
        
        Clears participant list commands from conference bubble.
    */
    void clearParticipantListActions();
    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
       Sets toolbar actions.
    */
    void setToolbarActions(const QList<PhoneAction*>& actions);
    
    /*!
        \fn void PhoneUIQtView::hideToolbar ()
        
        Hides toolbar.
    */
    void hideToolbar ();
    
    /*!
        \fn void PhoneUIQtView::showToolbar ()
        
        Shows toolbar.
    */
    void showToolbar ();

    /*!
        \fn void PhoneUIQtView::volumeSliderValue ()
        
        Returns HbVolumeSliderPopup's volume value.
        If this volume slider isn't active -1 is returned.
    */
    int volumeSliderValue ();

    /*!
        \fn void PhoneUIQtView::setVolumeSliderValue (int value)
        
        Sets slider value to HbVolumeSliderPopup.
    */
    void setVolumeSliderValue (
            int value,
            int commandId, 
            int maxVolumeValue, 
            int minVolumeValue);

    /*!
        \fn void PhoneUIQtView::removeVolumeSlider ()
        
        Removes HbVolumeSliderPopup.
    */
    void removeVolumeSlider ();

    /*!
        \fn void PhoneUIQtView::setExpandAction ()
        
        Sets expand action.
    */
    void setExpandAction(int bubbleId, int commandId);
    
        /*!
        \fn void PhoneUIQtView::removeExpandAction ()
        
        Removes expand action.
    */    
    void removeExpandAction(int bubbleId);
    
    /*!
        \fn void PhoneUIQtView::showDialpad()
        
        Shows dialpad widget.
    */    
    void showDialpad();
    
    /*!
        \fn void PhoneUIQtView::hideDialpad()
        
        Hides dialer widget.
    */    
    void hideDialpad();
    
    /*!
        \fn void PhoneUIQtView::isDialpadVisible()
        
        Checks is dialpad visible.
    */    
    bool isDialpadVisible();
    
    /*!
        \fn void PhoneUIQtView::dialpadText()
        
        Returns content of the dialpad.
    */  
    QString dialpadText();
    
    /*!
        \fn void PhoneUIQtView::dialpadString()
        
        Clears and hides dialpad.
    */
    void clearAndHideDialpad();
    
    /*!
        \fn void PhoneUIQtView::clearDialpad()
        
        Clears text from dialpad.
    */
    void clearDialpad();
    
    /*!
        \fn void PhoneUIQtView::bringToForeground()
        
        Brings to foreground.
    */
    void bringToForeground();
    
    /*!
        \fn void PhoneUIQtViewIF::setToolbarActions()
        
       Sets menu actions for call handling.
    */
    void setMenuActions(const QList<PhoneAction*>& actions);
    
    /*!
        \fn void PhoneUIQtView::setBackButtonVisible()
        
        Set the Back command visibility in TitleBar
    */
    void setBackButtonVisible(bool visible);
    
    /*!
        \fn void PhoneUIQtViewIF::menuReference()
        
        Returns HbMenu reference.
    */
    HbMenu &menuReference();

    /*!
        \fn void PhoneUIQtViewIF::captureKey()
        
        Captures key.
    */
    void captureKey(Qt::Key key, bool capture);
    
    /*
        \fn void setRestrictedMode()
        
        Sets Ui to restricted mode. Decides which keyevent are allowed.
     */
    void setRestrictedMode(bool restrictedMode);
    
public slots:

    /*!
        \fn void PhoneUIQtView::volumeSliderClosed ()
        
        HbVolumeSliderPopup calls this method when slider
        is going to be closed.
    */
    void volumeSliderClosed ();
    
    /*!
        \fn void PhoneUIQtView::volumeSliderChanged (int value)
        
        HbVolumeSliderPopup calls this method when it's value is changed.
    */
    void volumeSliderChanged(int value);

    /*!
        \fn void PhoneUIQtView::handleOrientationChange(Qt::Orientation orientation)
        
        HbMainWindow calls this method when orientation is changed.
    */
    void handleOrientationChange(Qt::Orientation orientation);
        
    /*!
        \fn void PhoneUIQtView::backButtonClicked(Qt::Orientation orientation)
        
        HbMainWindow calls this method when back softkey is clicked.
    */    
    void backButtonClicked();
    
    /*!
        \fn void PhoneUIQtView::onEditorContentChanged()
        
        Dialpad calls this method when dialpad content is changed.
    */
    void onEditorContentChanged();
    
    /*!
        \fn void dialpadClosed()
        
        Dialpad calls this method when dialpad is closed.
    */
    void dialpadClosed();

    /*!
        \fn void PhoneUIQtView::shutdownPhoneApp()
        
        Shutdown phone application.
    */
    void shutdownPhoneApp();
    
    /*!
        \fn void PhoneUIQtView::networkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &netName)
        
        Network name change is informed via this method.
    */
    void networkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &netName);
    
   
signals:

    /*!
        \fn void keyPressed (QKeyEvent *event)
        
        This signal is emitted when key is pressed down.
    */
    void keyPressed (QKeyEvent *event);

    /*!
        \fn void keyPressed (QKeyEvent *event)
        
        This signal is emitted when key is released.
    */
    void keyReleased (QKeyEvent *event);
    
    /*!
        \fn void command(int command)
        
        This signal is emitted when UI action is handled.
    */
    void command (int command);
    
    /*!
        \fn void dialpadIsAboutToClose()
        
        This signal is emitted when dialpad is about to close.
    */
    void dialpadIsAboutToClose();
    
    /*!
         \fn void windowActivated();
         
         This signal is emitted when window is activated
     */
    void windowActivated();
    
    /*!
         \fn void windowDeactivated();
         
         This signal is emitted when window is deactivated
     */
    void windowDeactivated();
    
protected:

    /*!
        \fn void eventFilter(int command)
        
        Handles key events.
    */
    bool eventFilter(QObject * watched, QEvent * event);
    
private:

    /*!
        \fn void setDialpadPosition(int command)
        
        Sets dialpad position.
    */
    void setDialpadPosition();

    /*!
        \fn void setActionRole()
        
        Sets action role.
    */    
    static void setActionRole(const PhoneAction& pa, HbAction& action);

    /*!
        \fn void createToolBarActions()
        
        Creates tool bar actions.
    */
    void createToolBarActions();
    
    /*!
        \fn void updateMenuVisibility()
     */
    void updateMenuVisibility();
    
private:
    HbMainWindow        &m_window;
    BubbleManager       *m_bubbleManager;
    QMap<int , QSignalMapper *> m_bubbleMap;
    QMap<int , QList<HbAction *> *> m_bubbleActionMap;
    QSignalMapper       *m_signalMapper;
    QList<HbAction *>   m_toolbarActions;
    HbVolumeSliderPopup *m_volumeSlider;
    QSignalMapper       *m_expandSignalMapper;
    QMap<int , HbAction*> m_expandActionMap;
    QSignalMapper       *m_participantListSignalMapper;
    QList<HbAction *>   m_participantListActions;
    int                 m_volumeCommandId;
    HbAction            *m_backAction;
    Dialpad             *m_dialpad;
    QSignalMapper       *m_menuSignalMapper;
    XQKeyCapture        *m_keyCapture;
    QSystemNetworkInfo  *m_networkInfo;
    QList<Qt::Key>      m_keyCaptures;
    DialpadKeyHandler   *m_dialpadKeyHandler;
    bool                m_restrictedMode;
    HbMenu              *m_optionsMenu;
};

#endif // PHONEUIQTVIEW_H
