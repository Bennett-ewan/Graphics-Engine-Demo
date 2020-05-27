// ---------------------------------------------------------------------------
// Project Name		:	GAM200 Project
// File Name		:	ScopeTimer.h
// Author			:	Allan Deutsch
// Creation Date	:	2019/09/09
// Purpose			:	Times its own life and outputs it.
// History			:
// - 2019/09/09		:	Scope Timer implementation taken from engine example
//					:	 on GameCentral
// Copyright © 2019 DigiPen(USA) Corporation.
// ---------------------------------------------------------------------------

#ifndef SCOPE_TIMER_H
#define SCOPE_TIMER_H

namespace Time
{
  typedef std::chrono::high_resolution_clock clock;
  typedef std::chrono::time_point<clock>     timePoint;
  typedef std::chrono::milliseconds          ms;
  typedef std::chrono::microseconds          us;
  typedef std::chrono::duration<float>       floatSeconds;

  class ScopeTimer
  {
  public:
    // When timer is initialized, get current time.
    ScopeTimer(float* output) : start_(clock::now()), durationOut_(output) {}

    // When timer leaves scope, calculate duration proportional to 1s/1.0f
    // Result is stored in output
    ~ScopeTimer()
    {
      end_ = clock::now();
      floatSeconds duration = end_ - start_;
      *durationOut_ = duration.count();
    }

  private:
    timePoint start_;
    timePoint end_;

    float* durationOut_;
  };
}

#endif
