/*
* ============================================================================
*  Name        : 
*  Part of     : 
*  Description : 
*  Version     : %version:  6 % 
*
*  Copyright 2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

#ifndef MENUEXTENSION_STUB_H_
#define MENUEXTENSION_STUB_H_

#include <QObject>
#include <hbaction.h>

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

#include <xqteluicommandextension.h>


#ifdef BUILD_DLL
#define DLL_EXPORT Q_DECL_EXPORT
#else
#define DLL_EXPORT Q_DECL_IMPORT
#endif

class SvtMenu : public QObject, public XQTelUiCommandExtension
    {
    Q_OBJECT
    Q_INTERFACES(XQTelUiCommandExtension)

public:
	SvtMenu();
	
	virtual ~SvtMenu();

    /*!
        \fn void modifyMenuItemList(QList<int> &menuCmdList)
        
        Modifies menu command list. List contains current list
        of menu commands. Interface can add new commands which should be hanled
        by call handling or remove commands if list contains unsupported call
        commands.
    */
    virtual void modifyMenuCommandList(const QList<CallInfo> &callInfo,
            QList<int> &menuCmdList);
    
    /*!
        \fn void modifyButtonCommandList(QList<int> &menuCmdList)
        
        Modifies push button command list. CallInfo contains current call 
        information and buttonCmdList contains current list of push 
        button commands. 
        Interface can remove commands if list contains unsupported call
        commands (For example if service doesn't support conference call).
    */
    virtual void modifyPushButtonCommandList(const QList<CallInfo> &callInfo,
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
    void modifyToolBarCommandList(const QList<CallInfo> &callInfo,
                                  QList<XQTelUiCommandExtension::ToolBarCommand> &toolBarCmdList);
    
    /*!
        \fn void addMenuActions(QList<HbAction*> &menuActions)
        
        Adds actions to menu. Interface adds own actions to the list which
        should be added to the call menu. 
        Interface is the responsible for handling action triggers.
        Ownership of the actions in not transferred.
    */
    virtual void addMenuActions(const QList<CallInfo> &callInfo,QList<HbAction*> &menuActions);
    
        /*!
        \fn void getCallsL(QList<CallInfo> &calls)
        
        Releases menu actions. Method is called when menu is not needed
        anymore and interface can release own menu actions.
    */
    virtual void releaseMenu();
    
    /*!
        \fn void release()
        
        Releases plugin. Method is called when menu plugin is not needed
        anymore and interface can release all resources.
    */
    virtual void release();
    


    };

#endif /* MENUEXTENSION_STUB_H_ */
