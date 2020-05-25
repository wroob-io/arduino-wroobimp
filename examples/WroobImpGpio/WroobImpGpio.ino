//
// WroobImpLed.ino
//
// Example controls pin using WroobImp protocol.
//

#include <WroobImp.h>

#define PIN_OUT 13
#define PIN_IN  14

// WroobImp constructor takes a "type"
// of your new Wroob module as an argument
WroobImp wroob("abcd");

// init JSON object for creating outgoing messages
StaticJsonDocument<80> event;

// Declaration of callback function where you
// can handle payload from incoming Wroob messages
void my_callback(JsonObject &payload) {
  // check if field "cmd" exists - if no then return
  if (payload["cmd"].isNull()) {
    return;
  }

  // handle payload: {"cmd": "set_pin", "value": X}
  if (payload["cmd"] == "set_pin" && !payload["value"].isNull()) {
    if (payload["value"] == 1) {
      digitalWrite(PIN_OUT, HIGH);
    } 
    else if (payload["value"] == 0) {
      digitalWrite(PIN_OUT, LOW);
    }

    return;
  }

  // handle payload: {"cmd": "get_pin"} - send GPIO state back
  if (payload["cmd"] == "get_pin") {
    event.clear();
    event["ev"] = "get_pin";
    event["value"] = digitalRead(PIN_IN);
    wroob.sendMessage(event);

    return;
  }
}

void setup()
{
  // initialize used GPIOs
  pinMode(PIN_OUT, OUTPUT);
  pinMode(PIN_IN, INPUT); 

  // initialize Wroob protocol
  wroob.begin(my_callback);
}

void loop()
{
  // feed your Wroob module with data from serial port
  wroob.feed();
}
