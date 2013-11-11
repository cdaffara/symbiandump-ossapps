/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock widget
*
*/

#include <QTimer>
#include "hsclocksettingsnotifier_symbian.h"

#include <bacntf.h> // CEnvironmentChangeNotifier


namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";
    const char TIME12[] = "TIME12";
    const char TIME24[] = "TIME24";
}

/*!
    \class HsClockSettingsNotifierType
    \ingroup group_HsClockSettingsNotifierplugin
    \brief Implementation for the observation of the system clock settings (analog/digital). 

*/


/*!
    Constructor
*/
HsClockSettingsNotifier::HsClockSettingsNotifier(QObject *parent)
    : QObject(parent),
      mDateTimeNotifier(0)
{
    mClockFormat = clockFormatString();
    mTimeFormat = timeFormatString();
    createObserver();
}



/*!
    Destructor.
*/
HsClockSettingsNotifier::~HsClockSettingsNotifier()
{
    delete mDateTimeNotifier;
}

/*!
    Returns the clock format ('analog'/'digital')
*/
QString HsClockSettingsNotifier::clockFormat() const
{
    return mClockFormat;
}

/*!
    Returns the time format ('12'/'24')
*/
QString HsClockSettingsNotifier::timeFormat() const
{
    return mTimeFormat;
}

/*!
    Callback function to receive system settings changes.
*/
TInt HsClockSettingsNotifier::EnvironmentChanged( TAny* aSelf )
{
    HsClockSettingsNotifier* self = static_cast<HsClockSettingsNotifier*>(aSelf);
    if ( self ){
        TInt change = self->mDateTimeNotifier->Change();
        if ( change & EChangesLocale ) {
            self->onSettingsChanged();
        }
    }
    return KErrNone;        
}

/*!
 Creates CEnvironmentChangeNotifier object to listen system settings changes.
*/
void HsClockSettingsNotifier::createObserver() 
{
    if ( !mDateTimeNotifier ){
        mDateTimeNotifier = CEnvironmentChangeNotifier::NewL( 
            CActive::EPriorityLow,
            TCallBack( EnvironmentChanged, this ) );
        
        mDateTimeNotifier->Start();            
    }
}   

/*!
    Reads clock format from system locale settings
*/
QString HsClockSettingsNotifier::clockFormatString() const
{
    TLocale locale;
    TClockFormat clockFormat = locale.ClockFormat();
    if ( clockFormat==EClockAnalog ) {
        return QString(ANALOG);
    } else {
        return QString(DIGITAL);
    }
}

/*!
    Reads time format from system locale settings
*/
QString HsClockSettingsNotifier::timeFormatString() const
{
    TLocale locale;
    TTimeFormat timeFormat = locale.TimeFormat();
    if ( timeFormat==ETime12 ) {
        return QString(TIME12);
    } else {
        return QString(TIME24);
    }
}

/*!
*/
void HsClockSettingsNotifier::onSettingsChanged()
{
    QString clockFormat = clockFormatString();
    QString timeFormat = timeFormatString();
    if ( clockFormat != mClockFormat || timeFormat != mTimeFormat ) {
        mClockFormat = clockFormat;
        mTimeFormat = timeFormat;
        emit settingsChanged(mClockFormat, mTimeFormat);
    }
}

