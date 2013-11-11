/*
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
 * Description:This class is for sms message center view 
 *
 */

#ifndef MSGSMSCENTERVIEW_H_
#define MSGSMSCENTERVIEW_H_

#include "msgbaseview.h"

class MsgSMSCenterSettingsForm;

class MsgSMSCenterView : public MsgBaseView
{
Q_OBJECT

public:
    explicit MsgSMSCenterView(int view = -1, QGraphicsItem *parent = 0);
    ~MsgSMSCenterView();
    
signals:
    void smsCenterEditViewClosed();
    
private slots:
    void onBackAction();
    void onCloseMessageCenterView();
    
private:

    MsgSMSCenterSettingsForm* mSMSCenterSettingsForm;
};

#endif /* MSGSMSCENTERVIEW_H_ */
