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

#ifndef CCONTACTSPLUGIN_H
#define CCONTACTSPLUGIN_H

#include <e32base.h>
#include <cntfldst.h>
#include <cntitem.h>
#include <cntdbobs.h>
#include <cindexingplugin.h>
#include <common.h>
#include "delayedcallback.h"
#include "cdocumentfield.h"

class CSearchDocument;
class CContactItemFieldSet;
class CCPixIndexer;

/** Field names */
_LIT(KContactsGivenNameField, "GivenName");
_LIT(KContactsFamilyNameField, "FamilyName");
_LIT(KContactsCompanyNameField, "CompanyName");
_LIT(KContactsPhoneNumberField, "PhoneNumber");
_LIT(KContactsAddressField, "Address");
_LIT(KContactsNoteField, "Note");
_LIT(KContactsJobTitleField, "JobTitle");
_LIT(KContactsSecondNameField, "SecondName");

_LIT(KContactsSuffixField, "Suffix");
_LIT(KContactsEMailField, "EMail");
_LIT(KContactsUrlField, "URL");

_LIT(KContactsPostOfficeField, "PostOffice");
_LIT(KContactsExtendedAddressField, "ExtendedAddress");
_LIT(KContactsLocalityField, "Locality");
_LIT(KContactsRegionField, "Region");
_LIT(KContactsPostcodeField, "PostCode");
_LIT(KContactsCountryField, "Country");
_LIT(KContactsSIPIDField, "SIPID");
_LIT(KContactsSpouseField, "Spouse");
_LIT(KContactsChildrenField, "Children");
//_LIT(KContactsClassField, "Class");
_LIT(KContactsPrefixField, "Prefix");
_LIT(KContactsAdditionalNameField, "AdditionalName");
_LIT(KContactsFaxField, "Fax");
_LIT(KContactsDepartmentName, "Departmant");
_LIT(KContactIMAddress, "IMAddress");
_LIT(KContactServiceProvider, "ServiceProvider");
_LIT(KContactAssistant, "Assistant");
_LIT(KContactAnniversary, "Anniversary");
_LIT(KContactBirthday, "Birthday");
_LIT(KContactIsGroup, "IsGroup");
_LIT(KDateSeparator, "-");
const TInt KDateFieldLength = 10;
const TInt KDayPosition = 8;
const TInt KMonthPosition = 5;
const TInt KYearPosition = 0;
const TInt KDayLength = 2;
const TInt KYearLength = 4;

class CContactsPlugin : public CIndexingPlugin, public MContactDbObserver, public MDelayedCallbackObserver
{
private:
    enum THarvesterState
        {
        EHarvesterIdleState,
        EHarvesterStartHarvest        
        };
        
    struct TRecord 
        {
        TInt iContactId;
        TCPixActionType iActionType;
        };
public:
	static CContactsPlugin* NewL();
	static CContactsPlugin* NewLC();
	virtual ~CContactsPlugin();

	/**
	 * From CIndexingPlugin
	 */
	void StartPluginL();
	void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
	void PausePluginL();
	void ResumePluginL();
	/**
	 * From MContactDbObserver, HandleDatabaseEventL.
	 */
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    /**
     * From MDelayedCallbackObserver, DelayedCallbackL
     */
    void DelayedCallbackL(TInt aCode);
    void DelayedError(TInt aError);

protected:
	CContactsPlugin();
	void ConstructL();

	/**
	 *  Adds information field (if available)
	 */
	void AddFieldL(CSearchDocument& aDocument, CContactItemFieldSet& aFieldSet, TUid aFieldId, const TDesC& aFieldName, const TInt aConfig = CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);
	
	/**
     *  Gets the date from anniversary text field
     */	
	void GetDateL(const TDesC& aTime, TDes& aDateString);
	/**
	 *  Adds to excerpt
	 */
	void AddToExcerptL(CSearchDocument& aDocument, CContactItemFieldSet& aFieldSet, TUid aFieldId, const TDesC& aFieldName );

	/**
	 *  Helper function: adds information field to the document and to the excerpt field(if available)
	 */
	void AddFieldToDocumentAndExcerptL(CSearchDocument& aDocument, CContactItemFieldSet& aFieldSet, TUid aFieldId, const TDesC& aFieldName, const TInt aConfig = CDocumentField::EStoreYes | CDocumentField::EIndexTokenized );
//#ifdef USE_HIGHLIGHTER
	void AddFieldToHLExcerptL( CContactItemFieldSet& aFieldSet, TUid aFieldId);
//#endif
	/**
	 * Creates the actual contact book index item
	 */
	void CreateContactIndexItemL(TInt aContentId, TCPixActionType aActionType);
	
	void OverWriteOrAddToQueueL(TRecord& aEntry);
	
	void IndexQueuedItems();

private:
	
	/** Contact change notifier */
	CContactChangeNotifier* iChangeNotifier;
	/** Sorted contact id array */
	const CContactIdArray* iContacts;
	/** Contacts database. */
	CContactDatabase* iDatabase;
	/** The asynchronizer */
	CDelayedCallback* iAsynchronizer;
	/** Current harvested contact index */
	TInt iCurrentIndex;	
	/** placeholder for Excerpt text dynamic creation */
	HBufC* iExcerpt;
//#ifdef USE_HIGHLIGHTER
	   HBufC* iHLDisplayExcerpt;
//#endif
	// CPix database 
    CCPixIndexer* iIndexer;
    //State of harvester either to pause/resume
    TBool iIndexState;    
    // Queue of documents to be indexed
    RArray<TRecord> iJobQueue;
    //harvesting state
    THarvesterState iHarvestState;
//for helping with testing.
#ifdef HARVESTERPLUGINTESTER_FRIEND
    friend class CHarvesterPluginTester;
#endif
    
#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
    void UpdatePerformaceDataL(TCPixActionType);
#endif	
};

#endif // CCONTACTSPLUGIN_H
