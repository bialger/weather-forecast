#ifndef TUIWORKER_HPP_
#define TUIWORKER_HPP_

#include <ftxui/component/screen_interactive.hpp>
#include "lib/forecast/Forecaster.hpp"
#include "lib/ftxui/ftxui_interface.hpp"

enum class Action {
  kNext,
  kPrev,
  kRefresh,
  kAddLine,
  kRemoveLine,
  kNextLine,
  kPrevLine
};

class TuiWorker {
 public:
  static const std::map<int32_t, std::vector<std::string>> kWeatherIcons;

  explicit TuiWorker(Forecaster& forecaster, int32_t interval);

  int32_t Run();

 private:
  static constexpr int32_t kMaxWidth = 45;
  static constexpr int32_t kMaxHeight = 12;
  static constexpr int32_t kFocusLen = 3;
  Forecaster& forecaster_;
  ConditionalOutput error_output_;
  ConditionalOutput log_output_;
  int32_t interval_;
  std::vector<ftxui::Element> elements_;
  ftxui::ScreenInteractive screen_;
  size_t start_focus_;
  std::mutex mutex_;
  int32_t result_;

  bool HandleEvent(const ftxui::Event& event);

  void RefreshElements();
  void RedrawScreen();
  int32_t ReloadScreen(Action action);

  void FocusNext();
  void FocusPrev();

  static ftxui::Element GetWeatherIcon(int32_t weather_code);
  static ftxui::Element GetUnit(const WeatherTimeUnit& unit, std::string header = "");
  static ftxui::Element GetDay(const WeatherDay& day);
  ftxui::Element GetCurrentUnit() const;
};

#endif //TUIWORKER_HPP_
