#include <glxmediamodel.h>
#include <glxmodelcommandhandler.h>
#include <glxmodelparm.h>

GlxModelCommandHandler::GlxModelCommandHandler()
    {
    }

GlxModelCommandHandler::~GlxModelCommandHandler()
    {
    }

void GlxModelCommandHandler::executeCommand(int commandId, int collectionId,QList<QModelIndex> indexList)
    {
    Q_UNUSED(commandId);
    GlxModelParm modelParm (collectionId, 0);
    GlxMediaModel* mediaModel = new GlxMediaModel (modelParm);
    doHandleUserAction(mediaModel,indexList);
    delete mediaModel;
    }
