/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef GLXDRMUTILITYWRAPPER_H
#define GLXDRMUTILITYWRAPPER_H

#include<QObject>
// forward decleration
class GlxDRMUtilityWrapperPrivate;

#ifdef BUILD_DRMUTILITYWRAPPER
#define GLX_DRMUTILITYWRAPPER_EXPORT Q_DECL_EXPORT
#else
#define GLX_DRMUTILITYWRAPPER_EXPORT Q_DECL_IMPORT
#endif

class GLX_DRMUTILITYWRAPPER_EXPORT GlxDRMUtilityWrapper
{
public:
    /*
     * Constructor
     */
    GlxDRMUtilityWrapper();
    
    /*
     * Destructor
     */
    ~GlxDRMUtilityWrapper();
    
    bool ItemRightsValidityCheck(QString aUri, bool aCheckViewRights);
    bool ConsumeRights(QString aUri);
    void ShowRightsInfo(QString aUri);

private:
    GlxDRMUtilityWrapperPrivate* mDRMUtilityWrapperPrivate;
};
    
#endif //GLXDRMUTILITYWRAPPER_H 
// End of file
