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

#ifndef RADIOSTATIONIF_H_
#define RADIOSTATIONIF_H_

// System includes
#include <QtGlobal>

class RadioStationIf
{
public:

    virtual int presetIndex() const = 0;
    virtual void setPresetIndex( int presetIndex ) = 0;

    virtual uint frequency() const = 0;
    virtual void setFrequency( uint frequency ) = 0;

    virtual QString name() const = 0;
    virtual void setName( QString name ) = 0;

    virtual bool isRenamedByUser() const = 0;
    virtual void setRenamedByUser( bool renamed ) = 0;

    virtual int genre() const = 0;
    virtual void setGenre( int genre ) = 0;

    virtual QString url() const = 0;
    virtual void setUrl( QString url ) = 0;

    virtual int piCode() const = 0;
    virtual void setPiCode( int piCode ) = 0;

    virtual bool isFavorite() const = 0;
    virtual void setFavorite( bool favorite ) = 0;

    virtual bool isLocalStation() const = 0;
    virtual void setLocalStation( bool localStation ) = 0;

    virtual bool hasStationSentRds() const = 0;
    virtual void setStationHasSentRds( bool hasSentRds ) = 0;

};


#endif // RADIOSTATIONIF_H_
