/*!
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines interface for manipulating telephony call commands.
*
*/

#ifndef XQTELUICOMMANDEXTENSION_H
#define XQTELUICOMMANDEXTENSION_H

#include <QObject>
#include <hbaction.h>


#ifdef BUILD_DLL
#define DLL_EXPORT Q_DECL_EXPORT
#else
#define DLL_EXPORT Q_DECL_IMPORT
#endif

/*!
    XQTelUiCommandExtension
    Defines interface for manipulating telephony call commands.
    
*/
class DLL_EXPORT XQTelUiCommandExtension 
{
    
public:
        
    /*!
       Specifies call states.
    */
    enum CallState
        {
        None = 0,
        Disconnecting = 1, // Call is disconnecting
        Active = 2, // Call is active
        OnHold = 3, // Call is on hold
        Incoming = 4, // Call is ringing
        Outgoing = 5 // Call is dialling or connecting
        };
        
    /*!
       Capsulates call info.
    */
    class CallInfo
        {
    public:
        
        // Service provider settings item Id
        int mServiceId;
        
        // Calls state
        int mCallState;
        
        // Is call header expanded
        bool mIsExpanded;

        };
    
    /*!
       Capsulates Tool Bar command info.
    */
    class ToolBarCommand
        {
    public:
        
        // Tool Bar command Id
        int mCommandId;

        // Is command enabled
        bool mIsEnabled;

        };
    
    /*!
        \fn void modifyMenuItemList(QList<int> &menuCmdList)
        
        Modifies menu command list. CallInfo contains current call information and 
        menuCmdList contains current list of menu commands. 
        Interface can add new commands which should be hanled by telephony
        call handling or remove commands if list contains unsupported call
        commands.
    */
    virtual void modifyMenuCommandList(const QList<CallInfo> &callInfo,
                                       QList<int> &menuCmdList) = 0;
                                       
    /*!
        \fn void modifyButtonCommandList(QList<int> &menuCmdList)
        
        Modifies push button command list. CallInfo contains current call 
        information and buttonCmdList contains current list of push 
        button commands. 
        Interface can remove commands if list contains unsupported call
        commands (For example if service doesn't support conference call)
        or add commads which should be handled by telephony call 
        handling (maximum amount of push button commands is 2 and list
        index 0 is used for right button).
    */
    virtual void modifyPushButtonCommandList(const QList<CallInfo> &callInfo,
                                             QList<int> &buttonCmdList) = 0;
    
                                             
    /*!
        \fn void modifyToolBarCommandList(QList<int> &menuCmdList)
        
        Modifies tool bar command list. CallInfo contains current call 
        information and toolBarCmdList contains current list of tool 
        bar commands. 
        Interface can remove/disable commands if the command list contains 
        unsupported call commands (For example if service doesn't support 
        conference call) or add commads which should be handled by telephony  
        call handling (maximum amount of tool bar commands is 4 and list
        index 0 is used for right button).
    */
    virtual void modifyToolBarCommandList(const QList<CallInfo> &callInfo,
                                          QList<ToolBarCommand> &toolBarCmdList) = 0;
    
    /*!
        \fn void addMenuActions(QList<HbAction*> &menuActions)
        
        Adds custom actions to the call menu. CallInfo contains current call 
        information. Interface could add own actions to the menu actions 
        list and actions will be added to the call menu. 
        Interface is the responsible for handling action triggers.
        Ownership of the actions in not transferred.
    */
    virtual void addMenuActions(const QList<CallInfo> &callInfo,
                                QList<HbAction*> &menuActions) = 0;
    
    /*!
        \fn void releaseMenu()
        
        Releases menu actions. Method is called when menu is not needed
        anymore and interface can release own menu actions.
    */
    virtual void releaseMenu() = 0;
    
    /*!
        \fn void release()
        
        Releases plugin. Method is called when menu extension plugin is 
        not needed anymore and interface can release all resources and
        plugin itself.
    */
    virtual void release() = 0;
    
};

Q_DECLARE_INTERFACE(XQTelUiCommandExtension, "com.nokia.telephony.XQTelUiCommandExtension/1.0" );

#endif // XQTELUICOMMANDEXTENSION_H

