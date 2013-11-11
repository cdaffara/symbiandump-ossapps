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

#ifndef _RADIOUIUTILITIES_H_
#define _RADIOUIUTILITIES_H_

// System includes
#include <QWeakPointer>

// User includes
#include "radio_global.h"
#include "radiofrequencystripdef.h"

// Forward declarations
class RadioFrequencyStrip;
class RadioStationCarousel;
class RadioFrequencyScanner;
class HbNotificationDialog;
class QSortFilterProxyModel;

struct EffectInfo
{
    EffectInfo( QGraphicsItem* item, QString path, QString event ) :
        mItem( item ),
        mPath( path ),
        mEvent( event )
    {}

    QGraphicsItem* mItem;
    QString mPath;
    QString mEvent;

private:
    EffectInfo();
};
typedef QList<EffectInfo> QEffectList;

// Class declaration
class RadioUtil
{
public:

    static bool addEffects( QEffectList list );

    static RadioFrequencyStrip* frequencyStrip();
    static RadioStationCarousel* carousel();
    static bool isScannerAlive();
    static Scan::Status scanStatus();

    static void setFrequencyStrip( RadioFrequencyStrip* frequencyStrip );
    static void setCarousel( RadioStationCarousel* carousel );
    static void setFrequencyScanner( RadioFrequencyScanner* scanner );
    static void setScanStatus( Scan::Status status );

    static Scroll::Direction scrollDirectionFromSkipMode( int skipMode );
    static Scroll::Direction scrollDirection( int direction );
    static TuneReason::Reason tuneReason( int tuneReason );

    static void showDiscreetNote( const QString& text );
    
    static void mapToSource( const QModelIndexList& proxyIndices, 
                             QModelIndexList& sourceIndices, 
                             const QSortFilterProxyModel* model);

private:

    RadioUtil();
    ~RadioUtil();

    static RadioUtil& instance();

private: // data

    QWeakPointer<RadioFrequencyStrip>   mFrequencyStrip;

    QWeakPointer<RadioStationCarousel>  mCarousel;

    QWeakPointer<RadioFrequencyScanner> mScanner;

    QWeakPointer<HbNotificationDialog>  mNotificationDialog;

    Scan::Status                        mScanStatus;

};


#endif // _RADIOUIUTILITIES_H_
