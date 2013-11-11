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

#ifndef RADIOFADINGLABEL_H
#define RADIOFADINGLABEL_H

// System includes
#include <HbLabel>
#include <HbEffect>

// User includes
#include "radiowidgetsexport.h"

class WIDGETS_DLL_EXPORT RadioFadingLabel : public HbLabel
{
    Q_OBJECT

public:

    RadioFadingLabel( QGraphicsItem* parent = 0 );

    void setFadingEnabled( bool fading );

    void setTextWithoutFading( const QString& newText );

public slots:

    void setText( const QString& newText );

private slots:

    void effectFinished( HbEffect::EffectStatus status );

private:

    void startEffect( const QString& effectName, const char* slot = 0 );

    QString trimHtml( const QString& text );

private: // data

    QString     mTextHolder;

    bool        mFadingEnabled;

};

#endif // RADIOFADINGLABEL_H
