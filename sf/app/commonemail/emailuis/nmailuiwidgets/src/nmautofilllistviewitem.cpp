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
* Description: Customized list view item to display autofill items on list. 
*
*/

#include "nmailuiwidgetsheaders.h"

static const QString NmAutoFillListViewDisplayName("AutoFillListViewDisplayName");
static const QString NmAutoFillListViewAddress("AutoFillListViewAddress");

static const QString NmAutoFillListViewItemUnderLineStart("<U>");
static const QString NmAutoFillListViewItemUnderLineEnd("</U>");


/*!
    \class NmAutoFillListViewItem
    \brief List view items for autofill items. 
    
    Customized list view item to display autofill items on list. 
*/

/*!
    Constructor of NmAutoFillListViewItem.
    
    Calls constructUi function and sets stretching to landscape mode. 
*/
NmAutoFillListViewItem::NmAutoFillListViewItem(QGraphicsItem *parent)
: HbListViewItem(parent), mDisplayName(NULL), mAddress(NULL)
{
    NM_FUNCTION;   
}

/*!
    Destructor of NmAutoFillListViewItem.
*/
NmAutoFillListViewItem::~NmAutoFillListViewItem()
{
}

/*!
    Item creation. Overwrites HbListViewItem funtion.
    
    \return A newly created NmAutoFillListViewItem. 
*/
HbListViewItem* NmAutoFillListViewItem::createItem()
{
    NM_FUNCTION;
    
    return new NmAutoFillListViewItem(*this);
}

/*!
    Update child items. Overwrites HbListViewItem funtion.
    
    The data is fetched from the model by using current model index and displayed.     
*/
void NmAutoFillListViewItem::updateChildItems()
{
    NM_FUNCTION;
 
    QString displayText;
    QString addressText;


    //Get data from model
    //check if QVariant is ok for this item etc.
    NmContactHistoryModelItem item =
            modelIndex().data(Qt::DisplayRole).value<NmContactHistoryModelItem>();
    
    if (item.subItemCount()) {

        QList<NmContactHistoryModelSubItem> itemlist = item.subEntries();
        
        //Check if both displayname and address is found. 
        if(itemlist.count() == 2) {
            displayText.append(setHtmlUnderLine(itemlist[0]));
            addressText.append(setHtmlUnderLine(itemlist[1]));
        //If only 1 item is found. It will be set to position of displayname. 
        } else if(itemlist.count() == 1) {
            displayText.append(setHtmlUnderLine(itemlist[0]));
        }
    } else {
        //Items not found. There is no point to continue. 
        return;
    }

    if (!mDisplayName) {
        mDisplayName = new HbRichTextItem(this);
        mDisplayName->setObjectName(NmAutoFillListViewDisplayName);
        HbStyle::setItemName(mDisplayName, NmAutoFillListViewDisplayName);
    }

    mDisplayName->setText(displayText);

    if (!mAddress){
        mAddress = new HbRichTextItem(this);
        mAddress->setObjectName(NmAutoFillListViewAddress);
        HbStyle::setItemName(mAddress, NmAutoFillListViewAddress);
    }

    mAddress->setText(addressText);

    HbListViewItem::updateChildItems();
}

/*!    
    Model index can be set. Overwrites HbListViewItem funtion.

    Enables set of all model indexes.  
    \param index The model index. 
    \return always true. 
*/
bool NmAutoFillListViewItem::canSetModelIndex(const QModelIndex &index)
{
    NM_FUNCTION;
    
    Q_UNUSED(index);
    return true;
}

/*!
    Private helper class to set html underline tags. 
    
    \param subItem Reference to subitem to do underlining. 
    \return QString underlined by html tags. 
 */
QString NmAutoFillListViewItem::setHtmlUnderLine(const NmContactHistoryModelSubItem &subItem)
{
    NM_FUNCTION;

    QString ret;
    bool odd(false);
    int index_corrector(0);

    QList<int> matching = subItem.mMatchingRanges;
    ret.append(subItem.mItemText);

    //Check that there is even count of matching range indexes.
    if ((matching.count() % 2) == 0) {

        foreach(int index, matching) {
            
            //Make sanity check for indexes. 
            if (index < subItem.mItemText.length() && index >= 0) {
                if (!odd) {
                    ret = ret.insert(index+index_corrector,NmAutoFillListViewItemUnderLineStart);
                    index_corrector += NmAutoFillListViewItemUnderLineStart.length();
                } else {
                    ret = ret.insert(index+1+index_corrector,NmAutoFillListViewItemUnderLineEnd);
                    index_corrector += NmAutoFillListViewItemUnderLineEnd.length();
                }
            } else {
                //Sanity check failed. No point to continue. 
                break; 
            }
            odd = odd ? false : true;
       }
    }
    return ret;
}

