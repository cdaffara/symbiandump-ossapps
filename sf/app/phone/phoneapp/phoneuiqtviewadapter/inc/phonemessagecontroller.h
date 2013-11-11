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
* Description:  Handles message editor.
*
*/

#ifndef PHONEMESSAGECONTROLLER_H
#define PHONEMESSAGECONTROLLER_H

#include <QObject>
#include <QString>
#include "tphonecmdparamnote.h"

// FORWARD DECLARATION
class TPhoneCommandParam;
class PhoneAppLauncher;

class PhoneMessageController : public QObject
{
    Q_OBJECT
    
public:

    /*!
        \fn PhoneMessageController (QObject *parent)
    */
    PhoneMessageController(PhoneAppLauncher &appLauncher, QObject *parent = 0);
        
    /*!
        Destructor
     */
    virtual ~PhoneMessageController();
    
    /*!
        \fn void openSoftRejectMessageEditor()
        
        This method opens soft reject message editor.

    */
    void openSoftRejectMessageEditor(TPhoneCommandParam *commandParam);
    
private:
    PhoneAppLauncher &m_appLauncher;
};

#endif // PHONEMESSAGECONTROLLER_H

