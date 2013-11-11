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

#include"glxdrmutilitywrapper_p.h"
#include"glxdrmutilitywrapper.h"

GlxDRMUtilityWrapper::GlxDRMUtilityWrapper()
    {
    mDRMUtilityWrapperPrivate = new GlxDRMUtilityWrapperPrivate(this);
    }
GlxDRMUtilityWrapper::~GlxDRMUtilityWrapper()
    {
    delete mDRMUtilityWrapperPrivate;
    }
bool GlxDRMUtilityWrapper::ItemRightsValidityCheck(QString aUri, bool aCheckViewRights)
    {
    bool ret = mDRMUtilityWrapperPrivate->ItemRightsValidityCheck(aUri,aCheckViewRights);
    return ret;
    }

bool GlxDRMUtilityWrapper::ConsumeRights(QString aUri)
    {
    bool ret = mDRMUtilityWrapperPrivate->ConsumeRights(aUri);
    return ret;
    }

void GlxDRMUtilityWrapper::ShowRightsInfo(QString aUri)
    {
    mDRMUtilityWrapperPrivate->ShowRightsInfo(aUri);
    }
