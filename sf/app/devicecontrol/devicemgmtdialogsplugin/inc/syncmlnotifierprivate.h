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
* Description: 
*
*/

#ifndef SYNCMLNOTIFIERPRIVATE_H_
#define SYNCMLNOTIFIERPRIVATE_H_

#include <QtCore>

class syncmlnotifierprivate
{
public:
syncmlnotifierprivate();

public:
   void setstatus(TInt status);
   void getdmprofileparameters(QString& displayname, TInt profileid);
   TInt serverinittype(TInt uimode);
   void setnotifierorigin(TInt notifierorig);

private:
   TInt iNotifierorigin;

};

#endif
