/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateParameters class 
*
*/


#ifndef IA_UPDATE_PARAMETERS_H
#define IA_UPDATE_PARAMETERS_H

#include <e32base.h>
#include <e32cmn.h>

#define private public
#include "videoiadupdatewrapper.h"
#undef private

/**
 * CIAUpdateParameters defines the update targets. 
 * The update action will be directed only to the objects 
 * that qualify at least one of the parameter requirements.
 *
 * @see CIAUpdate The operation functions provide more detailed
 * description about what combinations of the parameter values 
 * are supported.
 *
 * @since S60 v3.2
 */
class CIAUpdateParameters : public CBase
    {
public:

    enum TUpdateImportance
        {
        ETest = 0x1,
        ENormal = 0x2,
        ERecommended = 0x4,
        ECritical = 0x8,
        EMandatory = 0x10,
        EHidden = 0x20
        };
        
    enum TUpdateType
        {
        ESis  = 0x1,
        EFota = 0x2,
        ENsu  = 0x4
        };

    /**
     * @return CIAUpdateParameters* CIAUpdateParameters object
     * @exception Leaves with system wide error code.
     *
     * @since S60 v3.2
     */
    static CIAUpdateParameters* NewL(){ return new CIAUpdateParameters;};

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateParameters(){};

    /** 
     * @param aUid Uid describes either package UID or SID.
     *
     * @since S60 v3.2
     */
    void SetUid( const TUid&  ){};

    /** 
     * @param aSearchCriteria When correct updates are searched,
     * this text is used as a criteria when checking the information 
     * that is attached to the updates.
     *
     * @since S60 v3.2
     */
    void SetSearchCriteriaL( const TDesC& ){};

     /** 
     * @return aCommandLineExecutable When the update operation finishes,
     * the content of this desciptor will be used as a command line executable.
     *
     * @since S60 v3.2
     */
    void SetCommandLineExecutableL( const TDesC& ){};

    /** 
     * @param aCommandLineArguments When the update operation finishes,
     * the content of this desciptor will be used as command line arguments
     * with the command line executable.
     *
     * @since S60 v3.2
     */
    void SetCommandLineArgumentsL( const TDesC8& ){};

    /**
     * If this parameter is set to ETrue,
     * a ready-made wait dialog with localized texts is
     * shown if CIAUpdate::CheckUpdates operation takes longer
     * time to complete. This happens e.g. if the updates are 
     * refreshed from the server.
     *
     * @param aShowProgress If ETrue, a wait dialog is shown when
     * CIAUpdate::CheckUpdates operation is started. If EFalse,
     * the dialog is not shown.
     *
     * @see CIAUpdate::CheckUpdates
     *
     * @since S60 v3.2     
     */
    void SetShowProgress( TBool ){};

		
    // aImportance acts as a importance filter to the found IAD packages. It has no meaning with found FOTA or NSU packages.
    // If aImportance is not set, all importance types are valid.
    // See TUpdateImportance for possible values.
    void SetImportance( TUint ){};


    // aType acts as a type filter to the found update packages. 
    // If aType is not set, all package types are valid.
    // See TUpdateType for possible values. Combination of 
    void SetType( TUint ){};
	
	
    // If refresh is set on, metadata cache is refreshed from the server when calling
    // CIAUpdate::CheckUpdates operation. This option should not be set by normal applications.
    // It is meant for IAUpdate background checker, and caller SID is used to control this. 
    void SetRefresh( TBool ){};
	
};

#endif // IA_UPDATE_PARAMETERS_H
