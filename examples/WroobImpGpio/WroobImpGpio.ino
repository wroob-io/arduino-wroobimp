//
// WroobImpLed.ino
//
// Example controls pin using WroobImp protocol.
//

#include <WroobImp.h>

// A Wroob module has a numerical type ID and a type string.

// A type ID is a 8-bit number from range 128-255.
// A type string is a lowercase, letters-only human readable value,
// which should be tightly coupled with the type ID (1 to 1 mapping).
// When one value changes, the second should too.
// Usually the type string is an abbreviation of full module name, e.g.:
// "Example Arduino Module" -> "eam"

#define WROOB_MODULE_TYPE_ID 128
#define WROOB_MODULE_TYPE_STR "eam"

#define PIN_OUT 13
#define PIN_IN 14

// WroobImp constructor takes "typeId" and "typeStr" arguments
WroobImp wroob(WROOB_MODULE_TYPE_ID, WROOB_MODULE_TYPE_STR);

// init JSON object for creating outgoing messages
StaticJsonDocument<80> event;

// Declaration of callback function where you
// can handle payload from incoming Wroob messages
void my_callback(JsonObject &payload) {
    // check if field "cmd" exists - if no then return
    if (payload["cmd"].isNull()) {
        return;
    }

    // handle payload: {"cmd": "SetPin", "value": X}
    if (payload["cmd"] == "SetPin" && !payload["value"].isNull()) {
        if (payload["value"] == 1) {
            digitalWrite(PIN_OUT, HIGH);
        } else if (payload["value"] == 0) {
            digitalWrite(PIN_OUT, LOW);
        }

        return;
    }

    // handle payload: {"cmd": "GetPin"} - send GPIO state back
    if (payload["cmd"] == "GetPin") {
        event.clear();
        event["ev"] = "GetPin";
        event["value"] = digitalRead(PIN_IN);
        wroob.sendMessage(event);

        return;
    }
}

void setup() {
    // initialize used GPIOs
    pinMode(PIN_OUT, OUTPUT);
    pinMode(PIN_IN, INPUT);

    // initialize Wroob protocol
    wroob.begin(my_callback);
}

void loop() {
    // feed your Wroob module with data from serial port
    wroob.feed();
}
