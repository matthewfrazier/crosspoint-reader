#pragma once

#include <HalGPIO.h>

class MappedInputManager {
 public:
  enum class Button { Back, Confirm, Left, Right, Up, Down, Power, PageBack, PageForward };

  struct Labels {
    const char* btn1;
    const char* btn2;
    const char* btn3;
    const char* btn4;
  };

  explicit MappedInputManager(HalGPIO& gpio) : gpio(gpio) {}

  void update() const { gpio.update(); }
  bool wasPressed(Button button) const;
  bool wasReleased(Button button) const;
  bool isPressed(Button button) const;
  bool wasAnyPressed() const;
  bool wasAnyReleased() const;
  unsigned long getHeldTime() const;
  Labels mapLabels(const char* back, const char* confirm, const char* previous, const char* next) const;
  // Returns the raw front button index that was pressed this frame (or -1 if none).
  int getPressedFrontButton() const;

#ifdef XTENIA_DEV_HARNESS
  // Inject a synthetic press/release event. The next call to wasPressed()
  // / wasReleased() for the named button will return true exactly once,
  // mimicking a hardware-driven press.
  void injectSyntheticPress(Button button);
  void injectSyntheticRelease(Button button);
#endif

 private:
  HalGPIO& gpio;

  bool mapButton(Button button, bool (HalGPIO::*fn)(uint8_t) const) const;

#ifdef XTENIA_DEV_HARNESS
  // Bitmask of synthetic state pending for the next wasPressed/wasReleased
  // call. Bit position = static_cast<int>(Button).
  mutable uint16_t synthPressed_ = 0;
  mutable uint16_t synthReleased_ = 0;
  // Per-button level-triggered state for isPressed():
  //   0 = idle (defer to hardware), 1 = press pending, 2 = release pending.
  mutable uint8_t synthState_[9] = {};
#endif
};
