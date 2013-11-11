/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Parameter class for query dialogs.
*
*/

#ifndef __TPHONECMDPARAMQUERY_H
#define __TPHONECMDPARAMQUERY_H

//  INCLUDES
#include "tphonecommandparam.h"
#include "tcbacommandmapping.h"
#include "mphonequerycommandobserver.h"
#include <bamdesca.h>

// DATA TYPES
enum TPhoneQueryType
    {
    EPhoneQueryDialog,
    EPhoneTextQueryDialog,
    EPhoneGlobalQueryDialog,  
    EPhoneDtmfListQueryDialog,
    EPhoneDtmfTextQuery,
    EPhoneContactsListQuery,
    EPhoneGlobalMsgQuery,
    EPhoneGenericTextQuery,
    EPhoneGlobalWaitNote, // This is special case cause its note with softkeys
    EPhoneCustomListQueryDialog // Custom list query.
    };

/** Maximum number of configurable CBA command mappings.*/
const TInt KMaxNumberOfCbaCommandMappings = 4;

// CLASS DECLARATION

/**
*  A concrete parameter class for query dialogs.
*  
*/
class TPhoneCmdParamQuery : public TPhoneUICommandParam
    {
    public:
        
        /**
         * C++ default constructor.
         */
        IMPORT_C TPhoneCmdParamQuery();
        
        /**
         * C++ copy constructor.
         */
        IMPORT_C TPhoneCmdParamQuery( const TPhoneCmdParamQuery& aParam );
        
        /**
         * C++ assignment operator.
         */
        IMPORT_C TPhoneCmdParamQuery& operator=( 
            const TPhoneCmdParamQuery& aParam );
        
    public: 
        
        /**
        * Sets the resource id for dialog
        * @param aDialogResourceId is integer value
        */
        IMPORT_C void SetQueryResourceId( TInt aDialogResourceId );

        /**
        * Returns the dialog resource id.
        * @return Returns the carried resource id
        */
        IMPORT_C TInt QueryResourceId() const;
        
        /**
        * Sets the dialog prompt
        * @param aDialogPrompt is dialog prompt text
        */
        IMPORT_C void SetQueryPrompt( const TDesC& aDialogPrompt );

        /**
        * Returns the dialog prompt.
        * @return Returns the dialog prompt text
        */
        IMPORT_C const TDesC& QueryPrompt() const;
        
        /**
        * Sets the default cba resource
        * @param aCbaResourceId is integer value
        */
        IMPORT_C void SetDefaultCba( TInt aCbaResourceId );
        
        /**
        * Returns the default CBA's resource id
        * @return Returns the carried default CBA resource id
        */
        IMPORT_C TInt DefaultCba() const;
        
        /**
        * Sets the content cba resource.
        * These CBAs get displayed when there is some text/numbers in the the dialog.
        * @param aCbaResourceId is integer value
        */
        IMPORT_C void SetContentCba( TInt aCbaResourceId );
        
        /**
        * Returns the content CBA's resource id
        * @return Returns the carried content CBA resource id
        */
        IMPORT_C TInt ContentCba() const;
        
        /**
        * Sets the data text for dialog
        * @param aDataText is TDes pointer
        */
        IMPORT_C void SetDataText( TDes* aDataText );
        
        /**
        * Returns the data text
        * @return Returns the pointer to data text
        */
        IMPORT_C TDes* DataText() const;
        
        /**
        * Sets the query type
        * @param query type
        */
        IMPORT_C void SetQueryType( TPhoneQueryType aQueryType );
        
        /**
        * Returns the query type
        * @return query type
        */
        IMPORT_C TPhoneQueryType QueryType() const;

        /**
        * Sets the send key enable status.
        * @param aSendKeyEnabled is ETrue if the send key can be
        *  processed by the query.
        */
        IMPORT_C void SetSendKeyEnabled( TBool aSendKeyEnabled );

        /**
        * Returns the send key enabled status.
        * @return Returns ETrue if the send key can be processed by the query 
        */
        IMPORT_C TBool SendKeyEnabled() const;
        
        /**
        * Sets the command param id.
        * @param aParamId command id
        */
        IMPORT_C void SetCommandParamId( TPhoneCommandParam::TPhoneParamId aParamId );

        /**
        * Sets the timeout in milliseconds.
        * Query will destroyed after this time 
        * @param aTimeOut is integer value
        */
        IMPORT_C void SetTimeOut( TInt aTimeOut );

        /**
        * Gets the timeout of global query.
        */
        IMPORT_C TInt TimeOut() const;

        /**
        * Sets query dialog tone identifier.
        * @param    aTone   Identifier for a tone to play with dialog.
        */
        IMPORT_C void SetTone( TInt aTone );
        
        /**
        * Gets query dialog tone identifier.
        * @return   Identifier for a tone to play with dialog. 
        */
        IMPORT_C TInt Tone() const;
        
        /**
         * Sets mapping from CBA command to customized one.
         * @param    aCbaCommandId      CBA command identifier.
         * @param    aCustomCommandId   Custom command identifier.
         */
        IMPORT_C void SetCbaCommandMapping( 
            TInt aCbaCommandId, TInt aCustomCommandId );
        
        /**
         * Returns custom command mapped to given CBA command. 
         * If mapping is not defined, CBA command is returned.
         * @return  Custom command identifier for CBA command.
         */
        IMPORT_C TInt CbaCommandMapping( TInt aCbaCommandId ) const;

        /**
         * Sets custom command to be used if query time outs.
         * @param    aCustomCommandId   Custom command identifier.
         */
        IMPORT_C void SetCustomCommandForTimeOut( TInt aCustomCommandId );
        
        /**
         * Gets custom command to use if query time outs.
         * @param   aCustomCommandId    Custom command, if defined. 
         * @return  KErrNotFound if not defined, KErrNone otherwise.
         */
        IMPORT_C TInt GetCustomCommandForTimeOut( TInt& aCustomCommandId ) const;
        

        /**
         * Sets observer for query commands.
         * @param    aObserver   Command Observer. Ownership NOT
         *                       transferred.
         */
        IMPORT_C void SetQueryCommandObserver( 
                MPhoneQueryCommandObserver& aObserver );

        /**
         * Returns observer for query commands.
         * @return    Query command observer (NULL if not setted). 
         *            Ownership NOT transferred.
         */
        IMPORT_C MPhoneQueryCommandObserver* QueryCommandObserver();
        
        /**
         * Sets item text array for custom list query.
         * @param    aItemTextArray   Item text array.
         */
        IMPORT_C void SetItemTextArrayForListQuery( 
                MDesCArray& aItemTextArray );

        /**
         * Returns item text array for custom list query.
         * @return    Item text array (NULL if not setted). 
         */
        IMPORT_C MDesCArray* ItemTextArrayForListQuery() const;
        
    private:   

        /**
        * The dialog resource id
        */
        TInt iDialogResourceId;
        
        /**
        * The dialog prompt
        */
        TPtrC iDialogPrompt;

        /**
        * The default CBA resource id
        */
        TInt iDefaultCbaResourceId;
        
        /**
        * The content CBA resource id
        */
        TInt iContentCbaResourceId;
        
        /**
        * The data text
        */
        TDes* iDataText;
        
        /**
        * Query type
        */
        TPhoneQueryType iQueryType;

        /**
        * The send key enabled status
        */
        TBool iSendKeyEnabled;

        /**
        * The timeout
        */
        TInt iTimeOut;
        
        /**
         * Query dialog tone.
         */
        TInt iTone;
        
        /**
         * CBA command <-> custom command mappings.
         */
        TFixedArray<TCbaCommandMapping, KMaxNumberOfCbaCommandMappings> 
            iCbaCommandMappings;
        
        /**
         * Custom command to be used if query time outs.
         */
        TInt iCustomCmdForTimeOut;

        /**
         * Query command observer.
         * Not own.
         */
        MPhoneQueryCommandObserver* iCommandObserver;

        /**
         * Item text array for custom list query.
         * Not own.
         */
        MDesCArray* iItemTextArray;
    };

#endif      // __TPHONECMDPARAMQUERY_H   
            
// End of File
