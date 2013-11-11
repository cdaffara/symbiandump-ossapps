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
* Description:  Stub class.
*
*/


#include "menuextension_stub.h"

//#include "xqplugin.h"
#include <xqphoneappcommands.h>

bool m_modifyMenuCommandListCalled;
bool m_modifyPushButtonCommandListCalled;
bool m_addMenuActionsCalled;
bool m_releaseMenuCalled;
bool m_releaseCalled;
bool m_modifyToolBarCommandListCalled;

SvtMenu::SvtMenu() 
:
QObject()
{ 

}
	
SvtMenu::~SvtMenu()
{ 

}
    
void SvtMenu::modifyMenuCommandList(const QList<CallInfo> &callInfo,QList<int> &menuCmdList)
{
    m_modifyMenuCommandListCalled = true;
}

void SvtMenu::modifyPushButtonCommandList(const QList<CallInfo> &callInfo,QList<int> &buttonCmdList)
{
    m_modifyPushButtonCommandListCalled = true;
}

void SvtMenu::modifyToolBarCommandList(const QList<CallInfo> &callInfo,QList<XQTelUiCommandExtension::ToolBarCommand> &toolBarCmdList)
{
    m_modifyToolBarCommandListCalled = true;
}

void SvtMenu::addMenuActions(const QList<CallInfo> &callInfo,QList<HbAction*> &menuActions)
{
    m_addMenuActionsCalled = true;
}

void SvtMenu::releaseMenu()
{
    m_releaseMenuCalled = true;
}

void SvtMenu::release()
{
    m_releaseCalled = true;
    delete this;
}


//XQ_EXPORT_PLUGIN2(testplugin, SvtMenu);
