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
* Description:  Wraps phone menu extensions.
*
*/

#ifndef PHONECOMMANDEXTENSIONWRAPPER_H
#define PHONECOMMANDEXTENSIONWRAPPER_H

#include <QObject>
#include <QString>
#include <hbaction.h>
#include <xqteluicommandextension.h>


class PhoneCommandExtensionWrapper : public QObject
{
    Q_OBJECT
    
public:

    /*!
        \fn PhoneMenuExtensionWrapper (QObject *parent)
    */
    PhoneCommandExtensionWrapper(int pluginUid);
        
    /*!
        Destructor
     */
    virtual ~PhoneCommandExtensionWrapper();
    
    /*!
        \fn void showGlobalNote ()
        
        This method shows global note.

    */
    int pluginUid();

    /*!
        \fn void modifyMenuItemList(QList<int> &menuCmdList)
        
        Modifies menu command list. List contains current list
        of menu commands. Interface can add new commands which should be hanled
        by call handling or remove commands if list contains unsupported call
        commands.
    */
    void modifyMenuCommandList(
            const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
            QList<int> &menuCmdList);

    /*!
        \fn void modifyButtonCommandList(QList<int> &menuCmdList)
        
        Modifies push button command list. CallInfo contains current call 
        information and buttonCmdList contains current list of push 
        button commands. 
        Interface can remove commands if list contains unsupported call
        commands (For example if service doesn't support conference call)
        or add commads which should be handled by telephony call 
        handling (maximum amount of push button commands is two).
    */
    void modifyPushButtonCommandList(
            const QList<XQTelUiCommandExtension::CallInfo> &callInfo, 
            QList<int> &buttonCmdList);
 
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
    void modifyToolBarCommandList(
            const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
            QList<XQTelUiCommandExtension::ToolBarCommand> &toolBarCmdList);
    
    /*!
        \fn void addMenuActions(QList<HbAction*> &menuActions)
        
        Adds actions to menu. Interface adds own actions to the list which
        should be added to the call menu. 
        Interface is the responsible for handling action triggers.
        Ownership of the actions in not transferred.
    */
    void addMenuActions(
            const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
            QList<HbAction*> &menuActions);
    
    /*!
        \fn void releaseMenu()
        
        Releases menu actions. Method is called when menu is not needed
        anymore and interface can release own menu actions.
    */
    void releaseMenu();
    
    /*!
        \fn void release()
        
        Releases plugin. Method is called when menu plugin is not needed
        anymore and interface can release all resources.
    */
    void release();

    /*!
        \fn void getCallInfoList()
        
        Gets call list information by call state and
        service id map.
    */
    void getCallInfoList( QList<XQTelUiCommandExtension::CallInfo> &callInfo,
                          QMap<int,int> callStates, 
                          QMap<int,int> serviceIds, 
                          int expandedCall);
    
private:
        
    /*!
        \fn void mapCallState()
        
        Converts phoneengine call state to ui command extension
        call state.
        
        returns converted call state.
    */
    XQTelUiCommandExtension::CallState mapCallState( int callState );
    
private:

    XQTelUiCommandExtension *m_plugin;
    int m_pluginUid;
    
};

#endif // PHONECOMMANDEXTENSIONWRAPPER_H
