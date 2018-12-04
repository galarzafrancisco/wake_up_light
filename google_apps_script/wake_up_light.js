// Hello world!
function myFunction() {
  return ContentService.createTextOutput('Hello world!');
}


// This API will return the time in milliseconds until the next event in the calendar "Alarms".
// It will look for events in the next 2 hours. If no events are found, it will return 2 hours in milliseconds.
function doGet(e) {
  var cal = CalendarApp.getCalendarsByName('Alarms');
  Logger.log(cal);
  cal = cal[0];
  Logger.log(cal);
  if (cal == undefined) {
    return ContentService.createTextOutput('no access to calendar');
  }

  // Set the time window to look for events. I will look for events in the next 2 hours.
  const one_hour = 3600000; // [msec]
  var start = new Date(); //start.setHours(0, 0, 0); // start at midnight
  const stop = new Date(start.getTime() + 2 * one_hour);
  
  // Get the events from the calendar
  var events = cal.getEvents(start, stop);

  // Init the default value to be returned by this API if no events are found.
  var out = 2 * one_hour;
  
  // I'm gonna check all the events until I find one in the future.
  // If an event is happening right now, it will be the first in the list,
  // but the start date will be in the past. I want to skip this one.
  var q_events = events.length;
  var event_index = 0;
  while (event_index < q_events) {
    // Pick one event from the list
    event = events[event_index];
    var milliseconds_to_event = event.getStartTime() - start;
    if (milliseconds_to_event >= 0) {
      // This event hasn't started yet. This is what I want. I'll exit the while loop.
      out = Math.min(milliseconds_to_event, out);
      break;
    } else {
      // This event is in the past. I'll look for the next one.
      event_index += 1;
    }
  }
  
  // Convert the milliseconds to string so I can return it in the HTTP request.
  out = out.toString();
  Logger.log(out);
  return ContentService.createTextOutput(out)
}