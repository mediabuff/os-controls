#include "stdafx.h"
#include "ExposeView.h"
#include "RenderFrame.h"

ExposeView::ExposeView() :
  m_handIsGrabbing(false),
  m_opacity(0.0f, 0.3, EasingFunctions::Linear<float>)
{
  
}

ExposeView::~ExposeView() {
}

void ExposeView::AutoInit() {
  std::cout << "Add ExposeView to the render tree." << std::endl;
  auto self = shared_from_this();
  m_rootNode->AddChild(self);
}

void ExposeView::StartView() {
  m_opacity.Set(1.0f, 0.3);
}

void ExposeView::CloseView() {
  m_opacity.Set(0.0f, 0.2f);
}

void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  m_opacity.Update(frame.deltaT.count());
  UpdateLayout(frame.deltaT);
}

void ExposeView::Render(const RenderFrame& frame) const {
  m_backgroundRect.Draw(frame.renderState);
  
  for(auto window : m_windows) {
    //QUESTION: Do I need to lock the window here? 
    window->Draw(frame.renderState);
  }
}

void ExposeView::UpdateLayout(std::chrono::duration<double> timeout) {
  // TODO:  Given the current state of this object, update its layout:
}

void ExposeView::StartGrab() {
  m_handIsGrabbing = true;
}

bool ExposeView::EndGrab() {
  bool retVal = false;
  m_handIsGrabbing = false;
  return retVal;
}

void ExposeView::moveWindowToTop(std::shared_ptr<RectanglePrim> window) {
  auto windowIttr = std::find(m_windows.begin(), m_windows.end(), window);
  if(windowIttr != m_windows.end()) {
    m_windows.erase(windowIttr);
    m_windows.push_back(window);
  }
}