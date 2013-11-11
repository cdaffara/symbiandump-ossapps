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
* Description:  Actions and associated translated texts.
*
*/

#ifndef PHONEACTION_H
#define PHONEACTION_H

#include <hbicon.h>

#ifdef BUILD_PHONEUIQTVIEW
#define PHONEACTION_EXPORT Q_DECL_EXPORT
#else
#define PHONEACTION_EXPORT Q_DECL_IMPORT
#endif

class PHONEACTION_EXPORT PhoneAction
{
    
public:    

    class ToolBarItem {
    public:
        ToolBarItem(int commandId, bool enabled) : 
            mCommandId(commandId),mEnabled(enabled) {}
        
        int mCommandId;
        bool mEnabled;
    };
    
    enum ActionType {
        LeftButton = 1,
        RightButton,
        Text,
        ToolbarButton1,
        ToolbarButton2,
        ToolbarButton3,
        ToolbarButton4        
    };
    
    enum ActionRole {
        None,   // no specific role - default button
        Accept, // positive action - green button
        Decline // negative action - red button
    };

    PhoneAction ();
    ~PhoneAction ();
    void setText (QString text);
    QString text () const;
    void setCommand (int command);
    int command () const;
    HbIcon icon () const;
    void setIcon (HbIcon icon);
    void setDisabled(bool disabled);
    bool isDisabled() const;
    void setActionRole(ActionRole role);
    ActionRole actionRole() const;
    
private:

    QString m_text;
    int m_command;
    HbIcon m_icon;
    bool m_disabled;
    ActionRole m_role;
};

#endif // PHONEACTION_H
