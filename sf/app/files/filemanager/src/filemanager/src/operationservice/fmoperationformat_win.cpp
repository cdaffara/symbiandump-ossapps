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

#include "fmoperationformat.h"
#include "fmcommon.h"
#include "fmoperationbase.h"
#include "fmutils.h"

#include <QString>

/* \fn  void driveSpaceChanged()
 * This signal is emitted when copy or move is completed, and used to update the drive size.
 */

/*
 * Constructs a format operation with \a parent
 * \a mDriverName the drive to be formatted.
 */
FmOperationFormat::FmOperationFormat( QObject *parent, const QString &mDriverName ) : FmOperationBase( parent, FmOperationService::EOperationTypeFormat ),
    mDriverName( mDriverName )
{
}

/*
 * Destructs the operation.
 */
FmOperationFormat::~FmOperationFormat()
{
}

/*
 * Returns the to be formatted drive name
 */
QString FmOperationFormat::driverName()
{
    return mDriverName;
}

/*
 * Starts to format.
 * \a isStopped not used
 */
void FmOperationFormat::start( volatile bool */*isStopped*/ )
{ 
    QString logString = "FmOperationFormat::start";
    FM_LOG( logString );
    
    if( mDriverName.isEmpty() ) {
        emit notifyError( FmErrWrongParam, QString() );
        return;
    }
    int totalCount( 100 );
    emit notifyStart( totalCount, false );
    for( int i = 0; i < totalCount; i++ ) {
        emit notifyProgress( i );
    }

    emit notifyFinish();
    emit driveSpaceChanged();

}
