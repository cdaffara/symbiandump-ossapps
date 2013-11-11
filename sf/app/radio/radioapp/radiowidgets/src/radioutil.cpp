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

// System includes
#include <HbEffect>
#include <HbNotificationDialog>
#include <QSortFilterProxyModel>

// User includes
#include "radioutil.h"
#include "radiofrequencystrip.h"
#include "radiostationcarousel.h"
#include "radiofrequencyscanner.h"
#include "radiologger.h"

// Constants

static RadioUtil* theInstance = 0;

/*!
 *
 */
RadioUtil::RadioUtil() :
    mScanStatus( Scan::NotScanning )
{
}

/*!
 *
 */
RadioUtil::~RadioUtil()
{
}

/*!
 *
 */
bool RadioUtil::addEffects( QEffectList list )
{
    bool allAvailable = true;
    QEffectList added;
    foreach ( EffectInfo info, list ) {
        if ( HbEffect::add( info.mItem, info.mPath, info.mEvent ) ) {
            added.append( info );
        } else {
            allAvailable = false;
            break;
        }
    }

    if ( !allAvailable ) {
        foreach ( const EffectInfo& info, added ) {
            HbEffect::remove( info.mItem, info.mPath, info.mEvent );
        }
    }

    return allAvailable;
}


/*!
 *
 */
RadioFrequencyStrip* RadioUtil::frequencyStrip()
{
    return instance().mFrequencyStrip.data();
}

/*!
 *
 */
RadioStationCarousel* RadioUtil::carousel()
{
    return instance().mCarousel.data();
}

/*!
 *
 */
bool RadioUtil::isScannerAlive()
{
    RadioFrequencyScanner* scanner = instance().mScanner.data();
    if ( scanner ) {
        return scanner->isAlive();
    }
    return false;
}

/*!
 *
 */
Scan::Status RadioUtil::scanStatus()
{
    return instance().mScanStatus;
}

/*!
 *
 */
void RadioUtil::setFrequencyStrip( RadioFrequencyStrip* frequencyStrip )
{
    instance().mFrequencyStrip = frequencyStrip;
}

/*!
 *
 */
void RadioUtil::setCarousel( RadioStationCarousel* carousel )
{
    instance().mCarousel = carousel;
}

/*!
 *
 */
void RadioUtil::setFrequencyScanner( RadioFrequencyScanner* scanner )
{
    instance().mScanner = scanner;
}

/*!
 *
 */
void RadioUtil::setScanStatus( Scan::Status status )
{
    instance().mScanStatus = status;
}

/*!
 *
 */
Scroll::Direction RadioUtil::scrollDirectionFromSkipMode( int skipMode )
{
    return ( skipMode == StationSkip::Next || skipMode == StationSkip::NextFavorite )
           ? Scroll::Left : Scroll::Right;
}

/*!
 *
 */
Scroll::Direction RadioUtil::scrollDirection( int direction )
{
    return static_cast<Scroll::Direction>( direction );
}

/*!
 *
 */
TuneReason::Reason RadioUtil::tuneReason( int tuneReason )
{
    return static_cast<TuneReason::Reason>( tuneReason );
}

/*!
 *
 */
void RadioUtil::showDiscreetNote( const QString& text )
{
    RadioUtil& self = instance();
    if ( !self.mNotificationDialog ) {
        self.mNotificationDialog = new HbNotificationDialog();
    }
    self.mNotificationDialog.data()->setAttribute( Qt::WA_DeleteOnClose );
    self.mNotificationDialog.data()->setTitle( text );
    self.mNotificationDialog.data()->show();
}

/*!
 *
 */
void RadioUtil::mapToSource( const QModelIndexList& proxyIndices, 
                             QModelIndexList& sourceIndices, 
                             const QSortFilterProxyModel* model)
{
   if(!model) {
       return;
   }
   QModelIndexList::const_iterator iter = proxyIndices.constBegin();
   QModelIndexList::const_iterator end  = proxyIndices.constEnd();
   while( iter != end ) {
       sourceIndices.append( model->mapToSource( *iter ) );
       ++iter;
   }
}

/*!
 *
 */
RadioUtil& RadioUtil::instance()
{
    if ( !::theInstance ) {
        ::theInstance = new RadioUtil;
    }
    return *::theInstance;
}

