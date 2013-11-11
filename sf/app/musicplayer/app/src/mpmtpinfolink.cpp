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
* Description: Music Player MTP info link. This class is used to provide a link
*              to educate the user about MTP after a successful manual refresh.
*              Basically this class inherits from HbLabel and catches mouse events,
*              when clicked it opens a url already specified in settings.
*
*/

//Qt
#include <QUrl>
#include <QDesktopServices>
#include <QGraphicsSceneMouseEvent>

//Orbit
#include <hbparameterlengthlimiter.h>

//Local
#include "mpsettingsmanager.h"
#include "mpmtpinfolink.h"
#include "mptrace.h"

/*!
    \class MpMtpInfoLink
    \brief MpMtpInfoLink provides a label that can handle mouse events.

    When label is clicked it opens a URL containing MTP information.
*/

/*!
 Constructs MpMtpInfoLink.
 */
MpMtpInfoLink::MpMtpInfoLink()
{
    TX_ENTRY
    setTextWrapping( Hb::TextWrapAnywhere );
    setMtpInfoText();
    TX_EXIT
}

/*!
 Destructs the MpMtpInfoLink.
 */
MpMtpInfoLink::~MpMtpInfoLink()
{
    TX_LOG
}

/*!
 Catches mouse press events.
 */
void MpMtpInfoLink::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    Q_UNUSED( event );
}

/*!
 Catches mouse release event.
 */
void MpMtpInfoLink::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    TX_ENTRY
    QRectF itemRect = boundingRect();
    QPointF releasePoint = event->lastPos();
    if ( itemRect.contains( releasePoint ) ) {
        QUrl url( MpSettingsManager::instance()->mtpInfoUrl() );
        QDesktopServices::openUrl( url );
    }
    TX_EXIT
}

/*!
 \internal
 Set text to be shown in html format
 */
void MpMtpInfoLink::setMtpInfoText()
{
    TX_ENTRY
    QString url = MpSettingsManager::instance()->mtpInfoUrl();
    QString text = HbParameterLengthLimiter( hbTrId("txt_mus_info_note_that_using_nokia_ovi_suite_mode") ).arg( url );
    QString htmlLink = "<a href=\"" + url + "\">" + url + "</a>" ;
    text.replace( url, htmlLink );
    text = "<p>" + text + "</p>" ;
    setHtml( text );
    TX_EXIT
}

