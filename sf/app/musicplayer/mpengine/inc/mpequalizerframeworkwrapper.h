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
* Description: Wrapper for equalizer framework
*
*/

#ifndef MPEQUALIZERFRAMEWORKWRAPPER_H
#define MPEQUALIZERFRAMEWORKWRAPPER_H

// System includes
#include <QObject>
#include <QStringList>

class MpEqualizerFrameworkWrapperPrivate;

/**
 *  Wrapper for equalizer framework
 */
class MpEqualizerFrameworkWrapper : public QObject
    {
    Q_OBJECT
    Q_PROPERTY(QStringList presetNames READ presetNames)
    
public:

    explicit MpEqualizerFrameworkWrapper( QObject *parent = 0 );
    virtual ~MpEqualizerFrameworkWrapper();

    int getPresetNameKey( int presetIndex );
    int getPresetIndex( int presetNameKey );
    QStringList presetNames();

signals:
    void equalizerReady();

private:

    Q_DISABLE_COPY(MpEqualizerFrameworkWrapper)
    MpEqualizerFrameworkWrapperPrivate *d_ptr;

    friend class MpEqualizerFrameworkWrapperPrivate;
    };

#endif // MPEQUALIZERFRAMEWORKWRAPPER_H

//End of File
