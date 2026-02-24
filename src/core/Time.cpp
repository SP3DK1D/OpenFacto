#include "core/Time.h"

Time::Time(double tickRate) : tickInterval_(1.0 / tickRate) {}

void Time::BeginFrame(double frameDelta) {
  accumulator_ += frameDelta;
  if (accumulator_ > tickInterval_ * 5.0) {
    accumulator_ = tickInterval_ * 5.0;
  }
}

bool Time::ShouldRunTick() { return accumulator_ >= tickInterval_; }

void Time::ConsumeTick() { accumulator_ -= tickInterval_; }

double Time::Alpha() const { return accumulator_ / tickInterval_; }
