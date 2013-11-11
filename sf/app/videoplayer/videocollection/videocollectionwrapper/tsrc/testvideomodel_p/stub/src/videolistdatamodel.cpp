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
* Description: CVideoCollectionUtils class implementation
* 
*/

// INCLUDE FILES


#include <qobject.h>
#include "videolistdatamodel.h"

int VideoListDataModel::mStatus = -1;
    
QVariant VideoListDataModel::mStatusData = QVariant();

int VideoListDataModel::mFirstInserted = -1;
    
int VideoListDataModel::mLastInserted = -1;

int VideoListDataModel::mFirstRemoved = -1;
   
int VideoListDataModel::mLastRemoved = -1;

TMPXItemId VideoListDataModel::mModifiedItemId = TMPXItemId::InvalidId();

bool VideoListDataModel::mIndexReturnsInvalid = false;

// End of file
