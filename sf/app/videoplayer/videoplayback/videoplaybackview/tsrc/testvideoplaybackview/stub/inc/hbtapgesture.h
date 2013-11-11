/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HbTapGesture
*
*/

// Version : %version:   1 %



#ifndef HbTAPGESTURE_H_
#define HbTAPGESTURE_H_

#include <QTapGesture>

class HbTapGesture : public QTapGesture
{
    Q_OBJECT

    public:
        HbTapGesture();
        virtual ~HbTapGesture();
        Qt::GestureState state(){ return mState; }

    public:
        Qt::GestureState mState;
};

#endif /*HbTAPGESTURE_H_*/

