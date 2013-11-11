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
* Description: VoIP-specific view customization implementation.
*
*/

#ifndef C_CPHONEVIEWCUSTOMIZATIONVOIP_H
#define C_CPHONEVIEWCUSTOMIZATIONVOIP_H

#include <e32base.h>
#include "mphoneviewcustomization.h"
#include "phoneviewcommanddefinitions.h"

class CConvergedServiceSelector;
class MPhoneViewBlockingDialogObserver;
class TPhoneCommandParam;
class MPhoneCustomization;

/**
 *  Voip-specific view customization implementation.
 *  
 *  @lib PhoneUIVoIPExtension.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPhoneViewCustomizationVoip ) 
        : public CBase, public MPhoneViewCustomization
    {

public:
    
    /**
     * Two-phased constructor.
     * @param aCustomization: Handle to phone customization 
     */
    static CPhoneViewCustomizationVoip* NewL( 
        MPhoneCustomization& aCustomization );
    
    /**
     * Two-phased constructor.
     * @param aCustomization: Handle to phone customization
     */
    static CPhoneViewCustomizationVoip* NewLC( 
        MPhoneCustomization& aCustomization );

    /**
     * Destructor.
     */
    virtual ~CPhoneViewCustomizationVoip();

// from base class MPhoneViewCustomization
    
    /**
     * From MPhoneViewCustomization.
     * Handles custom view commands.
     * 
     * @param   aBlockingObserver   Blocking dialog observer.
     * @param   aCmdId              Command identifier.
     * @param   aCommandParam       Command parameter.
     * @return  ETrue if command was handled.
     */
    TBool HandleCommandL( 
        MPhoneViewBlockingDialogObserver& aBlockingObserver,
        TPhoneViewCommandId aCmdId,
        TPhoneCommandParam* aCommandParam );
    

    /**
     * From MPhoneViewCustomization.
     * Removes blocking dialogs.
     */
    void RemoveDialog();

private:

    CPhoneViewCustomizationVoip( MPhoneCustomization& aCustomization );

    void ConstructL();
    
    /**
     * Makes service selection.
     * @param   aBlockingObserver   Blocking dialog observer.
     * @param   aCommandParam       A command parameter.
     * @return  ETrue if service was selected.
     */
    TBool SelectVoipService( 
        MPhoneViewBlockingDialogObserver& aBlockingObserver,
        TPhoneCommandParam* aCommandParam );

private: // data

    /*
     * Handle to customization
     */
    MPhoneCustomization& iPhoneCustomization;
    };

#endif // C_CPHONEVIEWCUSTOMIZATIONVOIP_H
