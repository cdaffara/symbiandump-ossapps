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
* Description:
*
*/

#ifndef RADIOSETTINGS_H_
#define RADIOSETTINGS_H_

// System includes
#include <QtGlobal>
#include <QScopedPointer>

// User includes
#include "radiosettingsif.h"

// Forward declarations
class RadioSettingsPrivate;

// Class declaration
class RadioSettings : public RadioSettingsIf
    {
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioSettings )
    Q_DISABLE_COPY( RadioSettings )

    friend class RadioEngineWrapperPrivate;

public:

    virtual ~RadioSettings();

private:

    RadioSettings();

// from base class RadioSettingsIf

    bool isFirstTimeStart();
    void setFirstTimeStartPerformed( bool firstTimeStartPerformed );
    bool showFavorites() const;
    void setShowFavorites( bool showFavorites );
    bool toggleShowFavorites();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    const QScopedPointer<RadioSettingsPrivate> d_ptr;

    };

#endif // RADIOSETTINGS_H_
