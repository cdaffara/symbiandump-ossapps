#include "firstpluginforcpcategorymodel.h"
#include "firstpluginviewforcpcategorymodel.h"
#include <cpsettingformentryitemdataimpl.h>

FirstPluginForCpCategoryModel::FirstPluginForCpCategoryModel()
{
}

FirstPluginForCpCategoryModel::~FirstPluginForCpCategoryModel()
{
}
QList<CpSettingFormItemData*> FirstPluginForCpCategoryModel::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    return QList<CpSettingFormItemData*>() 
            << new CpSettingFormEntryItemDataImpl<FirstPluginViewForCategoryModel>(
               itemDataHelper,
               tr("The plugin for test"), 
               tr("TestPlugin."));
}

CpBaseSettingView *FirstPluginForCpCategoryModel::createSettingView(const QVariant &hint) const
{
    return 0;
}

Q_EXPORT_PLUGIN2(FirstPluginForCpCategoryModel, FirstPluginForCpCategoryModel);
