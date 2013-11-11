#ifndef FIRSTPLUGINFORCPCATEGORYMODEL_H
#define FIRSTPLUGINFORCPCATEGORYMODEL_H

#include <qobject.h>
#include <cpplugininterface.h>
#include <cplauncherinterface.h>

class FirstPluginForCpCategoryModel : public QObject, public CpPluginInterface, public CpLauncherInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    Q_INTERFACES(CpLauncherInterface)
public:
    FirstPluginForCpCategoryModel();
    virtual ~FirstPluginForCpCategoryModel();
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
    virtual CpBaseSettingView *createSettingView(const QVariant &hint) const;
};

#endif
