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

#ifndef RADIOSETTINGSIF_H_
#define RADIOSETTINGSIF_H_

// Class declaration
class RadioSettingsIf
    {
public:

    // First time start
    virtual bool isFirstTimeStart() = 0;
    virtual void setFirstTimeStartPerformed( bool firstTimeStartPerformed ) = 0;

    // Favorites
    virtual bool showFavorites() const = 0;
    virtual void setShowFavorites( bool showFavorites ) = 0;
    virtual bool toggleShowFavorites() = 0;

    };

#endif // RADIOSETTINGSIF_H_
