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
* Description:  Adapter for UI commands from view to state.
*
*/

#ifndef PHONEUICOMMANDADAPTER_H
#define PHONEUICOMMANDADAPTER_H

#include <QObject>

class MPhoneMenuAndCbaEvents;

class PhoneUiCommandAdapter : public QObject
{
    Q_OBJECT
    
public:

    /*!
       Constructor
     */
    PhoneUiCommandAdapter (MPhoneMenuAndCbaEvents &commandHandler, QObject *parent = 0);
    
    /*!
       Destructor
     */
    virtual ~PhoneUiCommandAdapter ();
    
    
public slots:

    /*!
        \fn void handleCommand (int command)
    
        This method handles UI command by passing it phone state.
    */
    void handleCommand (int command);
    
private:

    MPhoneMenuAndCbaEvents &m_handler;
    
};

#endif // PHONEUICOMMANDADAPTER_H
