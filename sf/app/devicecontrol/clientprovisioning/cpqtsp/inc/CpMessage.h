/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef CPMESSAGE_H_
#define CPMESSAGE_H_

#include <QObject>
#include <HbAction>

class CWPBioControl;

class CpMessage : public QObject
{
    Q_OBJECT

public:
    CpMessage(CWPBioControl* bio);
    ~CpMessage();
    void inValidPinMessage();
    void saveSettingMessage();
    void settingNotSaved();
    void setAsDefault();
    void autheticationFailure();
    void senderUnknown();
public slots:
    void saveSettingDialogClosed(HbAction *action);
    void setAsDefaultDialogClosed(HbAction *action);
    void autheticationFailureDialogClosed(HbAction *action);
    void senderUnknownDialogClosed(HbAction *action);
private:
    CWPBioControl* iBioControl;
};

#endif /* CPMESSAGE_H_ */
