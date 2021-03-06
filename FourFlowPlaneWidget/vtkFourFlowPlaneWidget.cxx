#include "vtkFourFlowPlaneWidget.h"
#include "vtkFourFlowPlaneRepresentation.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkHandleWidget.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h" 
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkRenderWindow.h"


vtkStandardNewMacro(vtkFourFlowPlaneWidget);

vtkFourFlowPlaneWidget::vtkFourFlowPlaneWidget() {
	//cout << "vtkFourFlowPlaneWidget::vtkFourFlowPlaneWidget()" << endl;
  this->WidgetState = vtkFourFlowPlaneWidget::Start;
  this->ManagesCursor = 1;
  this->CurrentHandle = 0;

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,vtkWidgetEvent::Select,this, vtkFourFlowPlaneWidget::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,vtkWidgetEvent::EndSelect,this, vtkFourFlowPlaneWidget::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,vtkWidgetEvent::Move,this, vtkFourFlowPlaneWidget::MoveAction);
}

vtkFourFlowPlaneWidget::~vtkFourFlowPlaneWidget() {  
	////std::cout << "vtkFourFlowPlaneWidget::vtkFourFlowPlaneWidget" << std::endl;
}

void vtkFourFlowPlaneWidget::SelectAction(vtkAbstractWidget *w) {
	//std::cout << "vtkFourFlowPlaneWidget::SelectAction" << std::endl;
	vtkFourFlowPlaneWidget *self = reinterpret_cast<vtkFourFlowPlaneWidget*>(w);
	if ( self->WidgetRep->GetInteractionState() == vtkFourFlowPlaneRepresentation::Outside )
		return;
	

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];
  
  // We are definitely selected
  self->WidgetState = vtkFourFlowPlaneWidget::Active;
  self->GrabFocus(self->EventCallbackCommand);
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  reinterpret_cast<vtkFourFlowPlaneRepresentation*>(self->WidgetRep)->StartWidgetInteraction(e);
  self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL); //for the handles
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->EventCallbackCommand->SetAbortFlag(1);
}

void vtkFourFlowPlaneWidget::MoveAction(vtkAbstractWidget *w) {
	//std::cout << "vtkFourFlowPlaneWidget::MoveAction" << std::endl;
  vtkFourFlowPlaneWidget *self = reinterpret_cast<vtkFourFlowPlaneWidget*>(w);
  // compute some info we need for all cases
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // See whether we're active
  if ( self->WidgetState == vtkFourFlowPlaneWidget::Start ) {
	  reinterpret_cast<vtkFourFlowPlaneRepresentation*>(self->WidgetRep)->allowMove = true;
    self->Interactor->Disable(); //avoid extra renders

    int state = self->WidgetRep->ComputeInteractionState(X,Y);
    if ( state == vtkFourFlowPlaneRepresentation::Outside )
      self->RequestCursorShape(VTK_CURSOR_DEFAULT);
    else
      self->RequestCursorShape(VTK_CURSOR_HAND);
    self->Interactor->Enable(); //avoid extra renders
    }
  else {
    double e[2];
    e[0] = static_cast<double>(X);
    e[1] = static_cast<double>(Y);
    reinterpret_cast<vtkFourFlowPlaneRepresentation*>(self->WidgetRep)->WidgetInteraction(e);
    self->Render();
  }
}

void vtkFourFlowPlaneWidget::EndSelectAction(vtkAbstractWidget *w) {
	//std::cout << "vtkFourFlowPlaneWidget::EndSelectAction" << std::endl;
	vtkFourFlowPlaneWidget *self = reinterpret_cast<vtkFourFlowPlaneWidget*>(w);
	if ( self->WidgetState == vtkFourFlowPlaneWidget::Start )
		return;

	self->InvokeEvent(vtkCommand::MouseMoveEvent,NULL); //handles observe this
	self->InvokeEvent(vtkCommand::InteractionEvent,NULL);
	self->InvokeEvent(vtkCommand::MouseMoveEvent,NULL); //handles observe this
	self->InvokeEvent(vtkCommand::InteractionEvent,NULL);

	// Return state to not active
	self->WidgetState = vtkFourFlowPlaneWidget::Start;
	self->ReleaseFocus();
	reinterpret_cast<vtkFourFlowPlaneRepresentation*>(self->WidgetRep)->allowMove = false;
}

void vtkFourFlowPlaneWidget::CreateDefaultRepresentation() {
	//std::cout << "vtkFourFlowPlaneWidget::CreateDefaultRepresentation" << std::endl;
	if (!this->WidgetRep)
		this->WidgetRep = vtkFourFlowPlaneRepresentation::New();
}

void vtkFourFlowPlaneWidget::PrintSelf(ostream& os, vtkIndent indent) {
	//std::cout << "vtkFourFlowPlaneWidget::PrintSelf" << std::endl;
	this->Superclass::PrintSelf(os,indent);
}


