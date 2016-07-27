#include <pebble.h>

#include <pebble-dash-api/pebble-dash-api.h>
#include <pebble-events/pebble-events.h>

#include "windows/main_window.h"

static bool s_first_request_made;

/********************************** Dash API **********************************/

static void error_callback(ErrorCode code) {
  if(code != ErrorCodeSuccess) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error! Code=%d", code);
  }
}

static void get_event_callback(DataType type, DataValue value) {
  main_window_set_next_event(value.string_value);
}

static void get_unread_count_callback(DataType type, DataValue value) {
  main_window_set_unread_count(value.integer_value);

  dash_api_get_data(DataTypeNextCalendarEventTwoLine, get_event_callback);
}

/************************************ App *************************************/

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  main_window_set_time(tick_time);

  const int mins = tick_time->tm_min;
  if((mins % 15 == 0) || !s_first_request_made) {
    s_first_request_made = true;

    dash_api_get_data(DataTypeUnreadSMSCount, get_unread_count_callback);
  }
}

static void init() {
  main_window_push();

  char *app_name = "Eventful";
  dash_api_init(app_name, error_callback);
  events_app_message_open();

  events_tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);
  tick_handler(time_now, MINUTE_UNIT);
}

static void deinit() { }

int main() {
  init();
  app_event_loop();
  deinit();
}
