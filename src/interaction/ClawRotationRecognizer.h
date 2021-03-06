#pragma once
#include "Leap.h"
#include "HandPoseRecognizer.h"
#include "TimeRecognizer.h"
#include "EigenTypes.h"
#include <Animation.h>

struct ClawRotation {
  float absoluteRotation;
  float deltaRotation;
  int64_t deltaTime;
};

class ClawRotationRecognizer {
public:
  ClawRotationRecognizer();
  ~ClawRotationRecognizer();
  
  void AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, ClawRotation& clawRotation);
private:
  // True if no packets have been received yet--used to guard against initialization spikes
  bool m_hasLast;
  
  Smoothed<float>m_deltaRot;
  // Last measured hand roll amount, used to guard against wander
  Smoothed<float>m_lastRot;
};