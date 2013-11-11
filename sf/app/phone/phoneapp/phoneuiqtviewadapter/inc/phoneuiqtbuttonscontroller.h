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
* Description:  Buttons controller for storing and getting
*               buttons' states
*
*/

#ifndef PHONEUIQTBUTTONSCONTROLLER_H
#define PHONEUIQTBUTTONSCONTROLLER_H

#include <QObject>

class PhoneUIQtButtonsController : public QObject
{
    Q_OBJECT

public:
    
    enum PhoneButtonFlag
    {
        Hold            = 0x02,  // Hold
        Ihf             = 0x04,  // Ihf
        Wired           = 0x08,  // Wired
        Btaa            = 0x10,  // Btaa
        DisableJoin     = 0x20,  // Should join option be disabled
        Conference      = 0x40,  // Conference call active
        Mute            = 0x80,  // Mute
        MultiCall       = 0x100, // Multi call
        FullConference  = 0x200, // Max member count in conference
        Outgoing        = 0x400, // Outgoing call
        IhfAsPushButton  = 0x800,  // Speaker as push button in bubble
        BluetoothAvailable   = 0x1000,  // Bluetooth available
        DisableSoftReject    = 0x2000,  // Disable soft reject
    };
    Q_DECLARE_FLAGS(PhoneButtonFlags, PhoneButtonFlag)

    /**
     * Constructor
     */
    PhoneUIQtButtonsController ();

    /**
     * Destructor
     */
    virtual ~PhoneUIQtButtonsController ();

    /*!
        \fn void PhoneUIQtButtonsController::setButtonFlags (PhoneButtonFlags flag,bool set)
        
        This method is called when audio path or hold status is changed
    */
    void setButtonFlags (PhoneButtonFlags flag,bool set);

    /*!
        \fn bool PhoneUIQtButtonsController::getButtonFlags (PhoneButtonFlags flag)
        
        This method is called when audio path or hold status is enquired
    */
    inline bool getButtonFlags (PhoneButtonFlags flag) {return m_buttonFlags & flag;};

private:
    PhoneButtonFlags m_buttonFlags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PhoneUIQtButtonsController::PhoneButtonFlags)

#endif // PHONEUIQTBUTTONSCONTROLLER_H
