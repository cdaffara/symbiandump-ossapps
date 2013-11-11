#include "secondpluginforcpcategorymodel.h"
#include "secondpluginviewforcpcategorymodel.h"
#include <cpsettingformentryitemdataimpl.h>

SecondPluginForCpCategoryModel::SecondPluginForCpCategoryModel()
{
}

SecondPluginForCpCategoryModel::~SecondPluginForCpCategoryModel()
{
}
QList<CpSettingFormItemData*> SecondPluginForCpCategoryModel::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    return QList<CpSettingFormItemData*>() 
            << new CpSettingFormEntryItemDataImpl<SecondPluginViewForCategoryModel>(
               itemDataHelper,
               tr("The plugin for test"), 
               tr("TestPlugin."));
}

CpBaseSettingView *SecondPluginForCpCategoryModel::createSettingView(const QVariant &hint) const
{
    return 0;
}

Q_EXPORT_PLUGIN2(SecondPluginForCpCategoryModel, SecondPluginForCpCategoryModel);
