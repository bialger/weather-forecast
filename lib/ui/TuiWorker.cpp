#include "TuiWorker.hpp"
#include "lib/forecast/WeatherTimeUnit.hpp"

const std::map<int32_t, std::vector<std::string>> TuiWorker::kWeatherIcons = {
    {0, {"    \\   /    ", "     .-.     ", "  ― (   ) ―  ", "     `-’     ", "    /   \\    "}},
    {1, {"   \\  /      ", " _ /\"\".-.    ", "   \\_(   ).  ", "   /(___(__) ", "             "}},
    {2, {"             ", "     .--.    ", "  .-(    ).  ", " (___.__)__) ", "             "}},
    {3, {"             ", "     .--.    ", "  .-(    ).  ", " (___.__)__) ", "             "}},
    {45, {"             ", " _ - _ - _ - ", "  _ - _ - _  ", " _ - _ - _ - ", "             "}},
    {48, {"             ", " _ - _ - _ - ", "  _ - _ - _  ", " _ - _ - _ - ", "             "}},
    {51, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ ‘ ‘ ‘  ", "   ‘ ‘ ‘ ‘   "}},
    {53, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ ‘ ‘ ‘  ", "   ‘ ‘ ‘ ‘   "}},
    {55, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ ‘ ‘ ‘  ", "   ‘ ‘ ‘ ‘   "}},
    {56, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ * ‘ *  ", "   * ‘ * ‘   "}},
    {57, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "     ‘ * ‘ * ", "    * ‘ * ‘  "}},
    {61, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ ‘ ‘ ‘  ", "   ‘ ‘ ‘ ‘   "}},
    {63, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ ‘ ‘ ‘  ", "   ‘ ‘ ‘ ‘   "}},
    {65, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "  ‚‘‚‘‚‘‚‘   ", "  ‚’‚’‚’‚’   "}},
    {66, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ * ‘ *  ", "   * ‘ * ‘   "}},
    {67, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    ‘ * ‘ *  ", "   * ‘ * ‘   "}},
    {71, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "    *  *  *  ", "   *  *  *   "}},
    {73, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "   * * * *   ", "  * * * *    "}},
    {75, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "   * * * *   ", "  * * * *    "}},
    {77, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "   * * * *   ", "  * * * *    "}},
    {80, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "     ‘ ‘ ‘ ‘ ", "    ‘ ‘ ‘ ‘  "}},
    {81, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "     ‘ ‘ ‘ ‘ ", "    ‘ ‘ ‘ ‘  "}},
    {82, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "   ‚‘‚‘‚‘‚‘  ", "   ‚’‚’‚’‚’  "}},
    {85, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "     *  *  * ", "    *  *  *  "}},
    {86, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "    * * * *  ", "   * * * *   "}},
    {95, {"     .-.     ", "    (   ).   ", "   (___(__)  ", "  ‚‘⚡‘‚⚡‚‘ ", "  ‚’‚’⚡’‚’  "}},
    {96, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "    ⚡‘ ‘⚡‘ ‘ ", "    ‘ ‘ ‘ ‘  "}},
    {99, {" _`/\"\".-.    ", "  ,\\_(   ).  ", "   /(___(__) ", "     *⚡*⚡* ", "    *  *  *  "}},
    {100, {"    .-.      ", "     __)     ", "    (        ", "     `-’     ", "      •      "}}
};

TuiWorker::TuiWorker(Forecaster& forecaster, int32_t interval)
    : forecaster_(forecaster),
      error_output_(forecaster.GetErrorOutput()),
      log_output_(forecaster.GetLogOutput()),
      interval_(interval),
      screen_(ftxui::ScreenInteractive::Fullscreen()),
      start_focus_(0),
      result_(0) {
  RefreshElements();
}

int32_t TuiWorker::Run() {
  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Started with interval " << interval_ << "\n";

  std::thread([&] {
    while (true) {
      std::this_thread::sleep_for(std::chrono::hours(interval_));
      WriteCurrentTime(log_output_);
      log_output_ << "TUI: Refreshing in interval\n";
      ReloadScreen(Action::kRefresh);
    }
  }).detach();

  auto component_renderer = ftxui::Renderer([&]() -> ftxui::Element {
    return yframe(vbox(elements_));
  });

  component_renderer |= ftxui::CatchEvent([this](const ftxui::Event& event) -> bool {
    return HandleEvent(event);
  });

  screen_.Loop(component_renderer);

  if (result_ != 0) {
    error_output_ << "TUI: Error " << result_ << "\n";
  }

  return result_;
}

bool TuiWorker::HandleEvent(const ftxui::Event& event) {
  if (event == ftxui::Event::Character('+')) {
    result_ = ReloadScreen(Action::kAddLine);
  } else if (event == ftxui::Event::Character('-')) {
    result_ = ReloadScreen(Action::kRemoveLine);
  }
  if (event == ftxui::Event::Character('s') || event == ftxui::Event::ArrowDown) {
    result_ = ReloadScreen(Action::kNextLine);
  } else if (event == ftxui::Event::Character('w') || event == ftxui::Event::ArrowUp) {
    result_ = ReloadScreen(Action::kPrevLine);
  } else if (event == ftxui::Event::Character('n') || event == ftxui::Event::Character('d')
      || event == ftxui::Event::ArrowRight) {
    std::thread([&]() -> void {
      result_ = ReloadScreen(Action::kNext);
    }).detach();
  } else if (event == ftxui::Event::Character('p') || event == ftxui::Event::Character('a')
      || event == ftxui::Event::ArrowLeft) {
    std::thread([&]() -> void {
      result_ = ReloadScreen(Action::kPrev);
    }).detach();
  } else if (event == ftxui::Event::Character('r') || event == ftxui::Event::F5) {
    std::thread([&]() -> void {
      result_ = ReloadScreen(Action::kRefresh);
    }).detach();
  } else if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape) {
    WriteCurrentTime(log_output_);
    log_output_ << "TUI: Exiting for thread " << std::this_thread::get_id() << "\n";
    screen_.Exit();
    return true;
  } else if (event == ftxui::Event::Custom) {
    WriteCurrentTime(log_output_);
    log_output_ << "TUI: Screen refreshed for thread " << std::this_thread::get_id() << "\n";
    return true;
  }

  if (result_ != 0) {
    error_output_ << "TUI: Exiting with error for thread " << std::this_thread::get_id() << "\n";
    screen_.Exit();
    return true;
  }

  return false;
}

int32_t TuiWorker::ReloadScreen(Action action) {
  const std::lock_guard<std::mutex> guard(mutex_);
  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Reloading screen with action ";

  int32_t result = 0;

  switch (action) {
    case Action::kAddLine: {
      log_output_ << "AddLine\n";

      forecaster_.AddDay();
      break;
    }
    case Action::kRemoveLine: {
      log_output_ << "RemoveLine\n";

      forecaster_.RemoveDay();

      if (forecaster_.GetForecast().size() < start_focus_ + kFocusLen) {
        FocusPrev();
      }

      break;
    }
    case Action::kNextLine: {
      log_output_ << "NextLine\n";

      FocusNext();
      break;
    }
    case Action::kPrevLine: {
      log_output_ << "PrevLine\n";

      FocusPrev();
      break;
    }
    case Action::kRefresh: {
      log_output_ << "Refresh\n";

      result = forecaster_.ObtainForecast();

      break;
    }
    case Action::kNext: {
      log_output_ << "Next\n";

      result = forecaster_.SwapToNext();

      break;
    }
    case Action::kPrev: {
      log_output_ << "Prev\n";

      result = forecaster_.SwapToPrev();

      break;
    }
  }

  RefreshElements();
  RedrawScreen();

  if (result != 0) {
    error_output_ << "Error in ReloadScreen in thread " << std::this_thread::get_id() << ", code " << result << "\n";
  } else {
    WriteCurrentTime(log_output_);
    log_output_ << "TUI: Reloaded screen for thread " << std::this_thread::get_id() << "\n";
  }

  return result;
}

void TuiWorker::RefreshElements() {
  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Refreshing elements for thread " << std::this_thread::get_id() << "\n";

  elements_.clear();
  elements_.push_back(GetCurrentUnit());
  const std::vector<WeatherDay> forecast = forecaster_.GetForecast();

  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Forecast size: " << forecast.size() << "\n";

  for (size_t i = start_focus_; i < std::min(start_focus_ + kFocusLen, forecast.size()); ++i) {
    elements_.push_back(GetDay(forecast[i]));
  }

  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Elements refreshed, added " << elements_.size()
              << " elements, including current weather and days from " << start_focus_ << " to "
              << (std::min(start_focus_ + kFocusLen, forecast.size()) - 1) << "\n";
}

void TuiWorker::RedrawScreen() {
  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Redrawing screen for thread " << std::this_thread::get_id() << "\n";
  screen_.PostEvent(ftxui::Event::Custom);
}

void TuiWorker::FocusNext() {
  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Focus switched to the next element from " << start_focus_;

  const size_t forecast_days = forecaster_.GetForecast().size();

  if (start_focus_ + kFocusLen < forecast_days) {
    ++start_focus_;
  }

  log_output_ << " to " << start_focus_ << "\n";
}

void TuiWorker::FocusPrev() {
  WriteCurrentTime(log_output_);
  log_output_ << "TUI: Focus switched to the previous element from " << start_focus_;

  if (start_focus_ > 0) {
    --start_focus_;
  }

  log_output_ << " to " << start_focus_ << "\n";
}

ftxui::Element TuiWorker::GetWeatherIcon(int32_t weather_code) {
  ftxui::Elements icon;
  icon.reserve(5);

  for (auto& line : kWeatherIcons.at(weather_code)) {
    icon.push_back(ftxui::text(line));
  }

  return ftxui::vbox(icon);
}

ftxui::Element TuiWorker::GetUnit(const WeatherTimeUnit& unit, std::string header) {
  std::map<std::string, std::string> output_unit = unit.GetAllAsMap();
  std::vector<ftxui::Element> lines{};
  header = header.empty() ? unit.GetName() : header;
  const int32_t width = header.empty() ? kMaxWidth : (2 * kMaxWidth);

  for (const auto& [name, value] : output_unit) {
    std::string label_name = name;
    label_name.append(": ");
    label_name.append(value);
    lines.push_back(ftxui::text(label_name));
  }

  return ftxui::vbox({ftxui::hbox({ftxui::filler(), ftxui::text(header), ftxui::filler()}), ftxui::separator(),
                      ftxui::hbox({GetWeatherIcon(unit.weather_type) | ftxui::flex, ftxui::vbox(lines) | ftxui::flex})})
      | ftxui::border | size(ftxui::WIDTH, ftxui::EQUAL, width) | size(ftxui::HEIGHT, ftxui::EQUAL, kMaxHeight);
}

ftxui::Element TuiWorker::GetDay(const WeatherDay& day) {
  std::vector<WeatherTimeUnit> units = day.GetForecastUnits();
  std::vector<ftxui::Element> elements{};

  for (const WeatherTimeUnit& unit : units) {
    elements.push_back(GetUnit(unit));
  }

  return ftxui::vbox({ftxui::hbox({ftxui::filler(), ftxui::text(day.GetDate()), ftxui::filler()}),
                      ftxui::hbox(elements)})
      | size(ftxui::WIDTH, ftxui::EQUAL, kMaxWidth * 4);;
}

ftxui::Element TuiWorker::GetCurrentUnit() const {
  return GetUnit(forecaster_.GetCurrentWeather(), forecaster_.GetLocation() + ", " + forecaster_.GetLastForecastTime());
}
