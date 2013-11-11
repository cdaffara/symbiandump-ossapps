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

#ifndef RADIORDSLISTENER_H_
#define RADIORDSLISTENER_H_

// user includes
#include "mradiordsdataobserver.h"

class RadioStationHandlerIf;
class RadioEngineWrapperPrivate;

class RadioRdsListener : public MRadioRdsDataObserver
{
public:

    RadioRdsListener( RadioStationHandlerIf& stationHandler, RadioEngineWrapperPrivate& wrapper );

private:

// from base class MFMRadioRdsObserver

    void RdsAvailable( TUint32 aFrequency, TBool aAvailable );
    void RdsAfSearchBegin();
    void RdsAfSearchEnd( TUint32 aFrequency, TInt aError );
    void RdsAfSearchStateChange( TBool aEnabled );
    void RdsDataProgrammeService( TUint32 aFrequency, const TDesC& aProgramService );
    void RdsDataRadioText( TUint32 aFrequency, const TDesC& aRadioText );
    void RdsDataRadioTextPlus( TUint32 aFrequency, const TInt aRadioTextPlusClass, const TDesC& aRadioText );
    void RdsDataGenre( TUint32 aFrequency, const TInt aGenre );
    void RdsDataPiCode( TUint32 aFrequency, const TInt aPiCode );

private: //data

    RadioStationHandlerIf& mStationHandler;

    RadioEngineWrapperPrivate& mWrapper;
};

#endif // RADIORDSLISTENER_H_
