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

#ifndef RADIOPRESETSTORAGE_P_H_
#define RADIOPRESETSTORAGE_P_H_

// System includes
#include <QScopedPointer>

// User includes

// Forward declarations
class CPresetUtility;

class RadioPresetStoragePrivate
{
public:

    RadioPresetStoragePrivate();
    ~RadioPresetStoragePrivate();

    bool init();

public: // data

    QScopedPointer<CPresetUtility>  mPresetUtility;

};

#endif // RADIOPRESETSTORAGE_P_H_
