#pragma once

class Time {
 public:
  explicit Time(double tickRate);
  void BeginFrame(double frameDelta);
  bool ShouldRunTick();
  void ConsumeTick();
  double Alpha() const;

 private:
  double tickInterval_;
  double accumulator_ = 0.0;
};
