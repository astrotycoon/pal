#include "libpal/libpal.h"

class TimeLineEventCallback {
public:
  int count;
  TimeLineEventCallback() {
    count = 0;
  }

  virtual ~TimeLineEventCallback() {

  }

  virtual void Callback(palTimeLine* tline) {
    // print out position
    palPrintf("%f %f\n", tline->GetNormalizedPosition(), tline->GetFilteredPosition());
  }

  void CallBackEndRepeat(palTimeLine* tline) {
    count--;
    if (count < 0) {
      tline->SetRepeatStyle(kTimeLineRepeatDont);
    }
  }
};

bool PalTimeLineTest() {
  palTimeLine tline;
  TimeLineEventCallback tec;
  
  tline.SetDuration(1.0f);

  tline.GetTimeLineEndEvent().Register(&tec, &TimeLineEventCallback::Callback);

  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);

  tline.Reset();

  
  // the callback will call the repetition after two times
  tec.count = 2;
  tline.GetTimeLineEndEvent().Register(&tec, &TimeLineEventCallback::CallBackEndRepeat);
  tline.SetRepeatStyle(kTimeLineRepeatFlipDirection);
  tline.RepeatCountTimes(4);

  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);

  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);

  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);

  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);
  tline.Step(0.25f);

  palBreakHere();
  return true;
}