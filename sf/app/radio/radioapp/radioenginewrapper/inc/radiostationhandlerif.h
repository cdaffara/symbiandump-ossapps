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

#ifndef RADIOSTATIONHANDLERIF_H_
#define RADIOSTATIONHANDLERIF_H_

// System includes
#include <QtGlobal>

// Forward declarations
class RadioStationIf;

class RadioStationHandlerIf
{
public:

    virtual uint currentFrequency() const = 0;
    virtual int currentPresetIndex() const = 0;

    virtual void setCurrentStation( uint frequency ) = 0;

    virtual bool containsFrequency( uint frequency ) = 0;

    virtual bool containsPresetIndex( int presetIndex ) = 0;

    virtual void startDynamicPsCheck() = 0;

    virtual void addScannedFrequency( uint frequency ) = 0;

    virtual void setCurrentPsName( uint frequency, const QString& name ) = 0;
    virtual void setCurrentRadioText( uint frequency, const QString& radioText ) = 0;
    virtual void setCurrentRadioTextPlus( uint frequency, int rtClass, const QString& rtItem ) = 0;
    virtual void setCurrentPiCode( uint frequency, int piCode ) = 0;
    virtual void setCurrentGenre( uint frequency, int genre ) = 0;

};

#endif // RADIOSTATIONHANDLERIF_H_
