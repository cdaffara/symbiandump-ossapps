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

#include "cntcontactcarddatacontainer.h"
#include "cntcontactcarddataitem.h"
#include "cntdetailorderinghelper.h"
#include <maptileservice.h> //For fetching maptile
#include <cntuiactionextension.h>
#include <cntuiextensionfactory.h>
#include <cntuicontactcardextension.h>

#include <QPainter>
#include <QList>
#include <qtcontacts.h>
#include <hbicon.h>
#include <hbnumbergrouping.h>
#include <QLocale>
#include <QDir>

#include <cntviewparams.h>
#include "cntstringmapper.h"
#include "cntextensionmanager.h"

/*!
Compare function for CntContactCardDataItem
*/
namespace
{
    bool compareObjects(const CntContactCardDataItem *d1, const CntContactCardDataItem *d2)
    {   
        return d1->mPosition < d2->mPosition;
    }
}

/*!
Constructor
*/
CntContactCardDataContainer::CntContactCardDataContainer(
    MapTileService* maptile,
    CntExtensionManager& aExtensionManager,
    Qt::Orientations orientation,
    QObject *parent) :
            mContact(NULL), 
            mCallAction(NULL),
            mMessageAction(NULL),
            mSeparatorIndex(-1),
            mLocationFeatureEnabled(false), 
            mMaptileInterface(maptile),
            mOrientation( orientation ),
            mExtensionManager( &aExtensionManager )
{
    Q_UNUSED(parent); 
    QList<QContactActionDescriptor> callActionDescriptors = QContactAction::actionDescriptors("call", "symbian");
    if (!callActionDescriptors.isEmpty())
    {
        mCallAction = QContactAction::action(callActionDescriptors.first());
    }
    QList<QContactActionDescriptor> messageActionDescriptors = QContactAction::actionDescriptors("message", "symbian");
    if (!messageActionDescriptors.isEmpty())
    {
        mMessageAction = QContactAction::action(messageActionDescriptors.first());
    }
}

void CntContactCardDataContainer::setContactData(QContact* contact, bool aMyCard)
{
    clearContactData();
    mContact = contact;
    mSeparatorIndex = -1;
    if (contact->type() == QContactType::TypeGroup)
    {
        initializeGroupData();
    }
    else
    {
        initializeActionsData(aMyCard);
        initialiseExtensionActions(aMyCard);
        initializeDetailsData();
        sortDataItems();
    }  
}

/*!
Destructor
*/
CntContactCardDataContainer::~CntContactCardDataContainer()
{
    clearContactData();
    delete mCallAction;
    mCallAction = NULL;
    delete mMessageAction;
    mMessageAction = NULL;
}

void CntContactCardDataContainer::clearContactData()
{
    qDeleteAll(mDataItemList);
    mDataItemList.clear();
}

/*!
Initialize contact details which include actions.
*/
void CntContactCardDataContainer::initializeActionsData(bool myCard)
{
    QList<QContactActionDescriptor> actionDescriptors = mContact->availableActions();
    QStringList availableActions;
    QStringList extendedActions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        if(actionDescriptors.at(i).vendorName() == "symbian" && actionDescriptors.at(i).implementationVersion() == 1)
            // String list for hardcoded actions, all actions falling in to this category must be hardcoded
            // to show them on UI.
            availableActions << action;
        else if(!extendedActions.contains(action))
            // String list for dynamically extendable actions. Duplicate actions
            // are handled later
            extendedActions << action;
    }

    QList<QContactPhoneNumber> details = CntDetailOrderingHelper::getOrderedSupportedPhoneNumbers(*mContact);
    for (int i = 0; i < details.count(); i++)
    { 
        //call
        if (availableActions.contains("call", Qt::CaseInsensitive) && supportsDetail("call", details[i]))
        {            
            QString context = details[i].contexts().isEmpty() ? QString() : details[i].contexts().first();
            QString subtype = details[i].subTypes().isEmpty() ? details[i].definitionName() : details[i].subTypes().first();
            
            int position = getPosition(subtype, context);
            
            if (position != CntContactCardDataItem::ENotSupported)
            {       
                CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(subtype, context), position, true);
                dataItem->setAction("call");
                /*
                 * Internationalization support, activate the following code 
                 * when support available from Orbit
                 */
                //dataItem->setValueText(HbNumberGrouping::formatPhoneNumber(details[i].number()));
                dataItem->setValueText(details[i].number());
                dataItem->setIcon(HbIcon(mStringMapper.getContactCardIconString(subtype, context)));
                dataItem->setContactDetail(details[i]);
                mDataItemList.append(dataItem);
            }
        }
        //message
        if (availableActions.contains("message", Qt::CaseInsensitive) && supportsDetail("message", details[i]))
        {  
           QString context = details[i].contexts().isEmpty() ? QString() : details[i].contexts().first();
           QString subtype = details[i].subTypes().isEmpty() ? details[i].definitionName() : details[i].subTypes().first();
           
           int position = getPosition(subtype, context);
           
           CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_send_message"), position, true);
           dataItem->setAction("message");
           /*
            * Internationalization support, activate the following code 
            * when support available from Orbit
            */
           //dataItem->setValueText(HbNumberGrouping::formatPhoneNumber(details[i].number()));
           dataItem->setValueText(details[i].number());
           QString icon;
           if (details[i].contexts().isEmpty())
           {
               icon = "qtg_large_message";
           }
           else if (details[i].contexts().first() == QContactDetail::ContextHome)
           {
               icon = "qtg_large_message_home";
           }
           else if (details[i].contexts().first() == QContactDetail::ContextWork)
           {
               icon = "qtg_large_message_work";
           }
           else
           {
               icon = "qtg_large_message";
           }
           dataItem->setIcon(HbIcon(icon));
           dataItem->setContactDetail(details[i]);
           mDataItemList.append(dataItem);
        }
    }
    //email
    if (availableActions.contains("email", Qt::CaseInsensitive))
    {
        QList<QContactDetail> details = actionDetails("email", *mContact);
        for (int i = 0; i < details.count(); i++)
        {
            QContactEmailAddress email(details.at(i));
            QString context = email.contexts().isEmpty() ? QString() : email.contexts().first();
            
            int position = getPosition(email.definitionName(), context);
            
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(email.definitionName(), context), position, true);
            dataItem->setAction("email");
            dataItem->setValueText(email.emailAddress(), Qt::ElideNone);
            dataItem->setIcon(HbIcon(mStringMapper.getContactCardIconString(email.definitionName(), context)));
            dataItem->setContactDetail(email);
            mDataItemList.append(dataItem);
        }
    }
    //url
    if (availableActions.contains("url", Qt::CaseInsensitive))
    {
        QList<QContactDetail> details = actionDetails("url", *mContact);
        for (int i = 0; i < details.count(); i++)
        {
            QContactUrl url(details.at(i));
            QString context = url.contexts().isEmpty() ? QString() : url.contexts().first();
            
            int position = getPosition(url.definitionName(), context);
            
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(url.definitionName(), context), position, true);
            dataItem->setAction("url");
            dataItem->setValueText(url.url());
            dataItem->setIcon(HbIcon(mStringMapper.getContactCardIconString(url.definitionName(), context)));
            dataItem->setContactDetail(url);
            mDataItemList.append(dataItem);
        }
    }
    QList<QContactDetail> allDetails = mContact->details();
    
    if(!myCard && extendedActions.count())
    {
        // Do not create actions for details in my card
        for (int i = 0; i < allDetails.count(); i++)
        {
            for(int j = 0; j < extendedActions.count(); j++)
            {
                QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(extendedActions[j]);
                for(int l = 0; l < actionDescriptors.count(); l++)
                {
                    // Different implementations(vendor, version) for same actions handled in loop
                    QContactAction* contactAction = QContactAction::action(actionDescriptors.at(l));
                    if(contactAction->isDetailSupported(allDetails[i], *mContact))
                    {
                        const QContactDetail detail = allDetails.at(i);
                        QVariantMap map = contactAction->metaData();
                        if(map.contains(KCntUiActionMetaTitleText) || map.contains(KCntUiActionMetaTitleTextDetail))
                        {
                            // Actions without title text and title text detail are considered to be non UI items
                            //action description
                            QString title = map.value(KCntUiActionMetaTitleText, "").toString();
                            if(title.isEmpty())
                            {
                                title = detail.value(map.value(KCntUiActionMetaTitleTextDetail).toString());
                            }
                            else
                            {
                                //TODO: We shoud have localizations for "Home" and "Work" strings...
//                                if (!detail.contexts().isEmpty())
//                                {
//                                    title += " ";
//                                    title += mStringMapper.getMappedDetail(detail.contexts().first());
//                                }
                            }
                            if(title.count())
                            {
                                QString context = detail.contexts().isEmpty() ? QString() : detail.contexts().first();
                                int position = getPosition(detail.definitionName(), context, true);
                                CntContactCardDataItem* dataItem = new CntContactCardDataItem(title, position, true);
                                //type
                                dataItem->setAction(extendedActions[j]);
                                //data
                                QString valueText = detail.value(map.value(KCntUiActionMetaValueTextDetail,"").toString());
                                if(valueText.isEmpty())
                                {
                                    valueText = map.value(KCntUiActionMetaValueText," ").toString();
                                }
                                dataItem->setValueText(valueText);
                                //icon
                                dataItem->setIcon(HbIcon(map.value(KCntUiActionMetaIcon, "").value<QIcon>()));
                                //detail
                                dataItem->setContactDetail(detail);
                                //save text for long press menu
                                dataItem->setLongPressText(map.value(KCntUiActionMetaValueTextLongPress,"...").toString());
                                // We must save descriptor to be able to distinguish separate services for same action
                                dataItem->setActionDescriptor(actionDescriptors.at(l));
                                mDataItemList.append(dataItem);     
                            }
                        }
                    }
                    delete contactAction;
                }
            }
        }
    }

    // This is special action case. Here we query implementations that are generic
    // to contact, so it's not linked to any detail(usually generic my card actions).
    for(int j = 0; j < extendedActions.count(); j++)
    {
        QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(extendedActions[j]);
        for(int l = 0; l < actionDescriptors.count(); l++)
        {
            // Different implementations(vendor, version) for same actions handled in loop
            QContactAction* contactAction = QContactAction::action(actionDescriptors.at(l));
            if(contactAction->isDetailSupported(QContactDetail(), *mContact))
            {
                QVariantMap map = contactAction->metaData();
                if(map.contains(KCntUiActionMetaTitleText))
                {
                    // Actions without title text are considered to be non UI items
                    //action description
                    QString title = map.value(KCntUiActionMetaTitleText, "").toString();
                    // Put as last action item on UI
                    CntContactCardDataItem* dataItem = new CntContactCardDataItem(title, CntContactCardDataItem::EGenericDynamic, true);
                    //type
                    dataItem->setAction(extendedActions[j]);
                    //data
                    dataItem->setValueText(map.value(KCntUiActionMetaValueText, "").toString());
                    //icon
                    dataItem->setIcon(HbIcon(map.value(KCntUiActionMetaIcon, "").value<QIcon>()));
                    //detail
                    dataItem->setContactDetail(QContactDetail());
                    //save text for long press menu
                    dataItem->setLongPressText(map.value(KCntUiActionMetaValueTextLongPress,"...").toString());
                    // We must save descriptor to be able to distinguish separate services for same action
                    dataItem->setActionDescriptor(actionDescriptors.at(l));
                    mDataItemList.append(dataItem);     
                }
            }
            delete contactAction;
        }
    }
}

void CntContactCardDataContainer::initialiseExtensionActions(bool /*aMyCard*/)
{
    // There can be several plugins that supply buttons.
    for( int n = 0; n < mExtensionManager->pluginCount(); ++n )
    {
        CntUiExtensionFactory& factory = *mExtensionManager->pluginAt( n );
        CntUiContactCardExtension* ext = factory.contactCardExtension();
        if ( ext )
        {
            ext->prepare( *mContact, false ); // aMyCard = false
            const int actionCount = ext->actionCount();
            for( int x = 0; x < actionCount; ++x )
            {
                const CntUiContactCardExtAction& action = ext->actionAt( x );

                const QContactDetail& detail = action.detailUsedForChoosingPosition();
                QString context = detail.contexts().isEmpty() ? QString() : detail.contexts().first();
                int position = getPosition(
                    detail.definitionName(),
                    context,
                    true); // dynamicAction = true

                CntContactCardDataItem* dataItem = new CntContactCardDataItem(
                    action.firstTextLine(),
                    position,
                    true); // aIsFocusable = true
                dataItem->setContactCardExtensionAndActionIndex( *ext, x );
                dataItem->setValueText(action.secondTextLine());
                dataItem->setIcon( action.icon());
                dataItem->setContactDetail( action.detailUsedForChoosingPosition() );
                //TODO: dataItem->setLongPressText();
                mDataItemList.append(dataItem);
            }
        }
    }
}

/*!
Initialize group details which includes actions.
*/
void CntContactCardDataContainer::initializeGroupData()
{
    // get the conference number
    QContactPhoneNumber confCallNumber = mContact->detail<QContactPhoneNumber>();
    
    //call
    if (confCallNumber.number() != NULL)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_conference_call"), itemCount(), true);
        dataItem->setAction("call");
        /*
         * Internationalization support, activate the following code 
         * when support available from Orbit
         */
        //dataItem->setValueText(HbNumberGrouping::formatPhoneNumber(confCallNumber.number()));
        dataItem->setValueText(confCallNumber.number());
        dataItem->setIcon(HbIcon("qtg_large_call_group"));
        dataItem->setContactDetail(confCallNumber);  
        mDataItemList.append(dataItem);
    }
    
    //message
    CntContactCardDataItem* dataMessageItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_send_val_members"), itemCount(), true);
    dataMessageItem->setAction("message");
    /*
     * Internationalization support, activate the following code 
     * when support available from Orbit
     */
    //dataMessageItem->setValueText(HbNumberGrouping::formatPhoneNumber(confCallNumber.number()));
    dataMessageItem->setValueText(confCallNumber.number());
    dataMessageItem->setIcon(HbIcon("qtg_large_message"));
    dataMessageItem->setContactDetail(confCallNumber);  
    mDataItemList.append(dataMessageItem);
    
    //email
    CntContactCardDataItem* dataEmailItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_email"), itemCount(), true);
    dataEmailItem->setAction("email");
    /*
     * Internationalization support, activate the following code 
     * when support available from Orbit
     */
    //dataEmailItem->setValueText(HbNumberGrouping::formatPhoneNumber(confCallNumber.number()));
    dataEmailItem->setValueText(confCallNumber.number());
    dataEmailItem->setIcon(HbIcon("qtg_large_email"));
    dataEmailItem->setContactDetail(confCallNumber);  
    mDataItemList.append(dataEmailItem);
}

/*!
Initialize contact details which not include actions.
*/
void CntContactCardDataContainer::initializeDetailsData()
{
    //sip & internet call
    QList<QContactOnlineAccount> onlinedDetails = CntDetailOrderingHelper::getOrderedSupportedOnlineAccounts(*mContact);
    for (int i = 0; i < onlinedDetails.count(); i++)
    {
        QContactOnlineAccount online(onlinedDetails.at(i));
        QString context = online.contexts().isEmpty() ? QString() : online.contexts().first();
        QString subtype = online.subTypes().isEmpty() ? online.definitionName() : online.subTypes().first();
        
        int position = getPosition(subtype, context);
        
        if (position != CntContactCardDataItem::ENotSupported)
        { 
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(subtype, context), position, false);
            dataItem->setValueText(online.accountUri());
            dataItem->setContactDetail(online);  
            addSeparator(itemCount());
            mDataItemList.append(dataItem);
        }
    }
    
    //address
    QString contextHome(QContactAddress::ContextHome.operator QString());
    QString contextWork(QContactAddress::ContextWork.operator QString());
    MapTileService::AddressType sourceAddressType;
    if( mMaptileInterface )
    {
        mLocationFeatureEnabled = mMaptileInterface->isLocationFeatureEnabled(MapTileService::AppTypeContacts) ;
    }

    QList<QContactAddress> addressDetails = mContact->details<QContactAddress>();
    for (int i = 0; i < addressDetails.count(); i++)
    {
        sourceAddressType = MapTileService::AddressPreference;
        QVariantList addressList;
        //no action
        int position = CntContactCardDataItem::EOther;
        QString title;
        if (addressDetails[i].contexts().isEmpty())
        {
            title = hbTrId("txt_phob_formlabel_address");
            position = CntContactCardDataItem::EAddress;
        }
        else
        {
            if ( addressDetails[i].contexts().at(0) == contextHome )
            {
                sourceAddressType = MapTileService::AddressHome;
                title = hbTrId("txt_phob_formlabel_address_home");
                position = CntContactCardDataItem::EAddressHome;
            }
            else if (addressDetails[i].contexts().at(0) == contextWork)
            {
                sourceAddressType = MapTileService::AddressWork;
                title = hbTrId("txt_phob_formlabel_address_work");
                position = CntContactCardDataItem::EAddressWork;
            }
            else
            {
                title = hbTrId("txt_phob_formlabel_address");
                position = CntContactCardDataItem::EAddress;
            }
        }
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(title, position, false);
        
        QStringList address;
        if (!addressDetails[i].street().isEmpty())
            address.append(addressDetails[i].street());
        if (!addressDetails[i].postcode().isEmpty())
            address.append(addressDetails[i].postcode());
        if (!addressDetails[i].locality().isEmpty())
            address.append(addressDetails[i].locality());
        if (!addressDetails[i].region().isEmpty())
            address.append(addressDetails[i].region());
        if (!addressDetails[i].country().isEmpty())
            address.append(addressDetails[i].country());
	    bool maptileAvailable = false;
		QString imageFile;
		
		if( mLocationFeatureEnabled && mMaptileInterface )
        {
        
            QContactLocalId contactId = mContact->id().localId();
            
			      //Get the maptile image path
            int status = mMaptileInterface->getMapTileImage(
                        contactId, sourceAddressType, imageFile, mOrientation );
                 
            if( status == MapTileService::MapTileFetchingInProgress || status == 
                    MapTileService::MapTileFetchingNetworkError )
            {
                //Load the progress indicator icon
                QString iconName("qtg_anim_small_loading_1");
                HbIcon inProgressIcon(iconName);
                dataItem->setSecondaryIcon( inProgressIcon );
            }
            else if( status == MapTileService::MapTileFetchingUnknownError || 
                        status == MapTileService::MapTileFetchingInvalidAddress  )
            {
                //Load the search stop icon
                QString iconName("qtg_mono_search_stop");
                HbIcon stopIcon(iconName);
                dataItem->setSecondaryIcon( stopIcon );
            }
            else if( status == MapTileService::MapTileFetchingCompleted )
            {
                maptileAvailable = true;
            }
        }
        dataItem->setValueText(address.join(", "));
        dataItem->setContactDetail(addressDetails[i]);
        addSeparator(itemCount());
        mDataItemList.append(dataItem);
        
        //Check whether location feature enabled
        if ( mLocationFeatureEnabled && maptileAvailable && !imageFile.isNull() )
        {
            //Display the maptile image
            HbIcon icon(imageFile);
            if( !icon.isNull() )
            {
                CntContactCardDataItem* dataItem = new CntContactCardDataItem(QString(), position, false);
                dataItem->setIcon(icon);
                addSeparator(itemCount());
                mDataItemList.append(dataItem);
		       }
        }
    } 
    
    //company
    QList<QContactOrganization> organizationDetails = mContact->details<QContactOrganization>();
    for (int i = 0; i < organizationDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_company_details"), CntContactCardDataItem::ECompanyDetails, false);
        QStringList companyList;
        if (!organizationDetails[i].title().isEmpty())
            companyList.append(organizationDetails[i].title());
        if (!organizationDetails[i].name().isEmpty())
            companyList.append(organizationDetails[i].name());
        if (!organizationDetails[i].department().isEmpty())
            companyList.append(organizationDetails[i].department());
        if (companyList.count()>0)
        {
            dataItem->setValueText(companyList.join(", ").trimmed());
            dataItem->setContactDetail(organizationDetails[i]);
            addSeparator(itemCount());
            mDataItemList.append(dataItem);
        }
    }
    
    //assistant name
    for (int i = 0; i < organizationDetails.count(); i++)
    {
        QString assistant = organizationDetails[i].assistantName();
        if (!assistant.isEmpty())
        {
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_assistant"), CntContactCardDataItem::EAssistantName, false);
            dataItem->setValueText(assistant);
            dataItem->setContactDetail(organizationDetails[i]);
            addSeparator(itemCount());
            mDataItemList.append(dataItem);
        }
    }
            
    //birthday
    QList<QContactBirthday> birthdayDetails = mContact->details<QContactBirthday>();
    for (int i = 0; i < birthdayDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_birthday"), CntContactCardDataItem::EBirthday, false);
        dataItem->setValueText(QLocale::system().toString(birthdayDetails[i].date()));
        dataItem->setContactDetail(birthdayDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.append(dataItem);
    }

    //anniversary
    QList<QContactAnniversary> anniversaryDetails = mContact->details<QContactAnniversary>();
    for (int i = 0; i < anniversaryDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_anniversary"), CntContactCardDataItem::EAnniversary, false);
        dataItem->setValueText(QLocale::system().toString(anniversaryDetails[i].originalDate()));
        dataItem->setContactDetail(anniversaryDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.append(dataItem);
    }
    
    //ringing tone
    QList<QContactRingtone> ringtoneDetails = mContact->details<QContactRingtone>();
    for (int i = 0; i < ringtoneDetails.count(); i++)
    {
        if (!ringtoneDetails.at(i).audioRingtoneUrl().isEmpty())
        {
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_ringing_tone"), CntContactCardDataItem::ERingtone, false);
            QFileInfo ringtoneFileInfo(ringtoneDetails[i].audioRingtoneUrl().toString());
            QString ringtoneFileName = ringtoneFileInfo.fileName();    
            dataItem->setValueText(ringtoneFileName);
            dataItem->setContactDetail(ringtoneDetails[i]);  
            addSeparator(itemCount());
            mDataItemList.append(dataItem);
            break;
        }
    }

    //note
    QList<QContactNote> noteDetails = mContact->details<QContactNote>();
    for (int i = 0; i < noteDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_note"), CntContactCardDataItem::ENote, false);
        dataItem->setValueText(noteDetails[i].note());
        dataItem->setContactDetail(noteDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.append(dataItem);
    }

    //family details
    QList<QContactFamily> familyDetails = mContact->details<QContactFamily>();
    for (int i = 0; i < familyDetails.count(); i++)
    {
        if (!familyDetails[i].spouse().isEmpty())
        {
            CntContactCardDataItem* dataSpouseItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_spouse"), 
                    CntContactCardDataItem::ESpouse, false);
            dataSpouseItem->setValueText(familyDetails[i].spouse());
            dataSpouseItem->setContactDetail(familyDetails[i]);  
            addSeparator(itemCount());
            mDataItemList.append(dataSpouseItem);
        }
        
        if (!familyDetails[i].children().isEmpty())
        {
            CntContactCardDataItem* dataChildrenItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_children"), 
                    CntContactCardDataItem::EChildren, false);
            dataChildrenItem->setValueText(familyDetails[i].children().join(", "));
            dataChildrenItem->setContactDetail(familyDetails[i]);  
            addSeparator(itemCount());
            mDataItemList.append(dataChildrenItem);
        }    
    }
}

/*!
Returns true if contactDetails contains spesific action.
*/
bool CntContactCardDataContainer::supportsDetail(const QString &actionName, const QContactDetail &contactDetail)
{
    QContactAction* action = NULL;

    if (actionName == "call") {
        action = mCallAction;
    } else if (actionName == "message") {
        action = mMessageAction;
    }
    
    return (action != NULL && action->isDetailSupported(contactDetail));
}

/*!
Returns the list of details which current action support.
*/
QList<QContactDetail> CntContactCardDataContainer::actionDetails(const QString &actionName, const QContact &contact)
{
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    if (actionDescriptors.isEmpty())
    {
       return QList<QContactDetail>();
    }
    
    QContactAction* contactAction = QContactAction::action(actionDescriptors.first()); 
    QList<QContactDetail> details = contactAction->supportedDetails(contact);

    delete contactAction;

    return details;
}

/*!
Returns the data for the given index
*/
CntContactCardDataItem* CntContactCardDataContainer::dataItem(int index) const
{
    if (index < 0)
    {
        return NULL;
    }
    else
    {
        return mDataItemList.at(index);
    }  
}

/*!
Set separator index and add separator item for the given index
*/
void CntContactCardDataContainer::addSeparator(int index)
{
    if (mSeparatorIndex == -1)
    {
        mSeparatorIndex = index;
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_subtitle_details"), CntContactCardDataItem::ESeparator, false);
        mDataItemList.append(dataItem);
    }
}

/*!
Returns the amount of items in the container
*/
int CntContactCardDataContainer::itemCount() const
{
    return mDataItemList.count();
}

/*!
Sort data item list by position number
*/
void CntContactCardDataContainer::sortDataItems()
{
    qStableSort(mDataItemList.begin(), mDataItemList.end(), compareObjects);
}

/*!
Returns position of specific item
*/
int CntContactCardDataContainer::getPosition(const QString& aId, const QString& aContext, bool dynamicAction)
{
    int position = CntContactCardDataItem::EOther;
    
    if (aId == QContactPhoneNumber::SubTypeAssistant && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallAssistant;
    }
    else if (aId == QContactPhoneNumber::SubTypeCar && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallCar;
    }
    else if (aId == QContactPhoneNumber::SubTypeMobile && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallMobile;
    }
    else if (aId == QContactPhoneNumber::SubTypeMobile && aContext == QContactDetail::ContextHome && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallMobileHome;
    }
    else if (aId == QContactPhoneNumber::SubTypeMobile && aContext == QContactDetail::ContextWork && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallMobileWork;
    }
    else if (aId == QContactPhoneNumber::SubTypeLandline && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallPhone;
    }
    else if (aId == QContactPhoneNumber::SubTypeLandline && aContext == QContactDetail::ContextHome && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallPhoneHome;    
    }
    else if (aId == QContactPhoneNumber::SubTypeLandline && aContext == QContactDetail::ContextWork && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallPhoneWork;
    }
    else if (aId == QContactPhoneNumber::SubTypeFax && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallFax;
    }
    else if (aId == QContactPhoneNumber::SubTypeFax && aContext == QContactDetail::ContextHome && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallFaxHome;
    }
    else if (aId == QContactPhoneNumber::SubTypeFax && aContext == QContactDetail::ContextWork && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallFaxWork;
    }
    else if (aId == QContactPhoneNumber::SubTypePager && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ECallPager;
    }
    else if (aId == QContactPhoneNumber::DefinitionName && aContext == QContactDetail::ContextHome && dynamicAction)
    {
        position = CntContactCardDataItem::ECallDynamicHome;
    }
    else if (aId == QContactPhoneNumber::DefinitionName && aContext == QContactDetail::ContextWork && dynamicAction)
    {
        position = CntContactCardDataItem::ECallDynamicWork;
    }
    else if (aId == QContactPhoneNumber::DefinitionName && aContext.isEmpty() && dynamicAction)
    {
        position = CntContactCardDataItem::ECallDynamic;
    }
    else if (aId == QContactOnlineAccount::SubTypeSipVoip && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::EInternetTelephone;
    }
    else if (aId == QContactOnlineAccount::SubTypeSipVoip && aContext == QContactDetail::ContextHome && !dynamicAction)
    {
        position = CntContactCardDataItem::EInternetTelephoneHome;
    }
    else if (aId == QContactOnlineAccount::SubTypeSipVoip && aContext == QContactDetail::ContextWork && !dynamicAction)
    {
        position = CntContactCardDataItem::EInternetTelephoneWork;
    }
    else if (aId == QContactOnlineAccount::SubTypeSip && aContext.isEmpty() && !dynamicAction)
    {
        position = CntContactCardDataItem::ESip;
    }
    else if (aId == QContactOnlineAccount::DefinitionName && aContext == QContactDetail::ContextHome && dynamicAction)
    {
        position = CntContactCardDataItem::EInternetDynamicHome;
    }
    else if (aId == QContactOnlineAccount::DefinitionName && aContext == QContactDetail::ContextWork && dynamicAction)
    {
        position = CntContactCardDataItem::EInternetDynamicWork;
    }
    else if (aId == QContactOnlineAccount::DefinitionName && aContext.isEmpty() && dynamicAction)
    {
        position = CntContactCardDataItem::EInternetDynamic;
    }
    else if (aId == QContactEmailAddress::DefinitionName && aContext.isEmpty())
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EEmail;
        else
            position = CntContactCardDataItem::EEmailDynamic;
    }
    else if (aId == QContactEmailAddress::DefinitionName && aContext == QContactDetail::ContextHome)
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EEmailHome;   
        else
            position = CntContactCardDataItem::EEmailDynamicHome;
    }
    else if (aId == QContactEmailAddress::DefinitionName && aContext == QContactDetail::ContextWork)
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EEmailWork;
        else
            position = CntContactCardDataItem::EEmailDynamicWork;
    }
    else if (aId == QContactAddress::DefinitionName && aContext.isEmpty())
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EAddress;
        else
            position = CntContactCardDataItem::EAddressDynamic;
    }
    else if (aId == QContactAddress::DefinitionName && aContext == QContactDetail::ContextHome)
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EAddressHome;
        else
            position = CntContactCardDataItem::EAddressDynamicHome;
    }
    else if (aId == QContactAddress::DefinitionName && aContext == QContactDetail::ContextWork)
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EAddressWork;
        else
            position = CntContactCardDataItem::EAddressDynamicWork;
    }
    else if (aId == QContactUrl::DefinitionName && aContext.isEmpty())
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EUrl;
        else
            position = CntContactCardDataItem::EUrlDynamic;
    }
    else if (aId == QContactUrl::DefinitionName && aContext == QContactDetail::ContextHome)
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EUrlHome;
        else
            position = CntContactCardDataItem::EUrlDynamicHome;
    }
    else if (aId == QContactUrl::DefinitionName && aContext == QContactDetail::ContextWork)
    {
        if(!dynamicAction)
            position = CntContactCardDataItem::EUrlWork;
        else
            position = CntContactCardDataItem::EUrlDynamicWork;
    }
    else if (aId == QContactPhoneNumber::DefinitionName && aContext.isEmpty())
    {
        position = CntContactCardDataItem::ELastAction;
    }
    else if (aId == QContactPhoneNumber::DefinitionName && aContext == QContactDetail::ContextHome)
    {
        position = CntContactCardDataItem::ELastActionHome;
    }
    else if (aId == QContactPhoneNumber::DefinitionName && aContext == QContactDetail::ContextWork)
    {
        position = CntContactCardDataItem::ELastActionWork;
    }
    else if (!dynamicAction)
    {
        position = CntContactCardDataItem::ENotSupported;
    }
    else
    {
        position = CntContactCardDataItem::EDynamic;
    }
    
    return position;
}

