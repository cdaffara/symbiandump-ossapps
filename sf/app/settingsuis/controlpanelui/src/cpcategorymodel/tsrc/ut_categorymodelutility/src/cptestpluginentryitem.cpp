#include "CpTestPluginEntryItem.h"

#include <cpitemdatahelper.h>
#include <hbaction.h>
#include <QFileInfo>
CpTestPluginEntryItemData::CpTestPluginEntryItemData(const HbDataFormModelItem *parent /* = 0*/)
{
    
}
CpTestPluginEntryItemData::CpTestPluginEntryItemData(CpItemDataHelper &itemDataHelper,
                                                       const QString &text /*= QString()*/,
                                                       const QString &description /*= QString()*/,
                                                       const HbIcon &icon /*= HbIcon()*/,
                                                       const HbDataFormModelItem *parent /*= 0*/)
                                                       : CpSettingFormEntryItemData(itemDataHelper,
                                                       text,
                                                       description,
                                                       icon,
                                                       parent)
{
}

CpTestPluginEntryItemData::~CpTestPluginEntryItemData()
{
}

void CpTestPluginEntryItemData::testOnLaunchView()
{
    onLaunchView();
}
CpBaseSettingView *CpTestPluginEntryItemData::createSettingView() const
{
    return 0;
}
