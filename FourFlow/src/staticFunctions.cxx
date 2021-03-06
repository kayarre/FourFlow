#include "pqIncludes.h"
#include "vtkIncludes.h"
#include "qtIncludes.h"
#include "vtkSMSessionProxyManager.h"

pqPipelineSource *addSAX(pqPipelineSource *source) {
	pqServer *server = pqActiveObjects::instance().activeServer();
	pqApplicationCore *core = pqApplicationCore::instance();
	pqObjectBuilder *builder = core->getObjectBuilder();  
	//vtkSMProxyManager *pxm = vtkSMProxyManager::GetProxyManager();
	vtkSMSessionProxyManager* pxm = server->proxyManager();
	pqServerManagerModel *sm = core->getServerManagerModel();
	pqActiveObjects *activeObjects = &pqActiveObjects::instance();

	QMap<QString, QList<pqOutputPort*> > namedInputs;

    vtkSMProxy *filterProxy = pxm->GetPrototypeProxy("filters", "SAX");
	namedInputs["Input"].append(source->getOutputPort(0));
	pqPipelineSource* filter = builder->createFilter("filters", "SAX", namedInputs, server);
	return filter;
	return 0;
}

void setActiveRenderView() {
	pqApplicationCore *core = pqApplicationCore::instance();
	pqObjectBuilder *builder = core->getObjectBuilder();
	pqServerManagerModel *sm = core->getServerManagerModel();
	foreach(pqView *view, sm->findItems<pqView*>()) {
		if(view->getViewType() == "RenderView") {
			pqActiveObjects::instance().setActiveView(view);
			break;
		}
	}
}

void updateVolumeSlice() {
	pqApplicationCore *core = pqApplicationCore::instance();
	pqObjectBuilder *builder = core->getObjectBuilder();
	pqServerManagerModel *sm = core->getServerManagerModel();
	foreach(pqView *view, sm->findItems<pqView*>()) {
		if(view->getViewType() == "VolumeSliceView") {
			view->render();
			break;
		}
	}
}

pqPipelineSource *addTemporalTimeSnap(pqPipelineSource *source) {
	pqServer *server = pqActiveObjects::instance().activeServer();
	pqApplicationCore *core = pqApplicationCore::instance();
	pqObjectBuilder *builder = core->getObjectBuilder();  
	//vtkSMProxyManager *pxm = vtkSMProxyManager::GetProxyManager();
	vtkSMSessionProxyManager* pxm = server->proxyManager();
	pqServerManagerModel *sm = core->getServerManagerModel();
	pqActiveObjects *activeObjects = &pqActiveObjects::instance();

	QMap<QString, QList<pqOutputPort*> > namedInputs;

    vtkSMProxy *filterProxy = pxm->GetPrototypeProxy("filters", "TemporalSnapToTimeStep");
	namedInputs["Input"].append(source->getOutputPort(0));
	pqPipelineSource* filter = builder->createFilter("filters", "TemporalSnapToTimeStep", namedInputs, server);
	return filter;
	return 0;
}

