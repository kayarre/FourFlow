#ifndef _pqFourFlowPlaneWidget_h
#define _pqFourFlowPlaneWidget_h

#include "pq3DWidget.h"
//#include "pqComponentsExport.h"
#include <QColor>

class pqServer;

/// Provides a complete Qt UI for working with a 3D FourFlowPlane widget
class pqFourFlowPlaneWidget : public pq3DWidget {
  typedef pq3DWidget Superclass;
  Q_OBJECT
public:
  pqFourFlowPlaneWidget(vtkSMProxy* o, vtkSMProxy* pxy, QWidget* p = 0, const char* xmlname="FourFlowPlaneWidgetRepresentation");
  ~pqFourFlowPlaneWidget();

  /// Resets the bounds of the 3D widget to the reference proxy bounds.
  /// This typically calls PlaceWidget on the underlying 3D Widget 
  /// with reference proxy bounds.
  /// This should be explicitly called after the panel is created
  /// and the widget is initialized i.e. the reference proxy, controlled proxy
  /// and hints have been set.
  virtual void resetBounds() { this->Superclass::resetBounds(); }
  virtual void resetBounds(double bounds[6]);

protected:
  /// Called on pick.
  virtual void pick(double, double, double);

private slots:
  void onWidgetVisibilityChanged(bool visible);

private:
  void createWidget(pqServer* server, const QString& xmlname);

  class pqImplementation;
  pqImplementation* const Implementation;
};

#endif
