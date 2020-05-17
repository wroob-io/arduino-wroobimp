# WroobImp

This library implements Wroob Inter-Module-Protocol. It transforms any Arduino project into [Wroob](https://wroob.io) system compatible custom peripheral module. WroobImp library provides simple API for registering into Wroob system and messaging between Wroob broker and created module. Thanks to that you can control your Arduino board via high-level serial protocol from Android device with Wroob mobile app.

## Wroob system

[Wroob](https://wroob.io) is an environment allowing to create complex systems based on functional modules. Its main core is a mobile application which switches any Android device into system CPU. User can use any of in-built [Wroob modules](https://wroob.io/#section-info) providing some less or more complex functionalities (like audio processing, video processing, speech synthesis, speech recognition) and/or connect Android device via USB to Arduino board and develop Arduino based own customized Wroob module using **WroobImp** library.

## Installation

* Install the library by [Using the Library Manager](https://www.arduino.cc/en/Guide/Libraries#toc3)
* **OR** by [Importing the .zip library](https://www.arduino.cc/en/Guide/Libraries#toc4) using the [master](https://github.com/wroob-io/arduino-wroobimp/archive/master.zip)

## Examples

The library comes with [examples](examples). After installing the library you need to restart the Arduino IDE before they can be found under **File > Examples > WroobImp**.

---

# Reference

### Include Library

```c
#include <WroobImp.h>
```

### Define your Wroob module type

```c
WroobImp wroob("abcd");
```

### Define message callback

In this function user can handle incoming messages from Wroob system to developped module. Argument is a JSON object containing payload of incoming message. WroobImp is using [ArduinoJson](https://arduinojson.org/) library for managing JSON objects.
```c
void my_callback(JsonObject &payload) {
  ...
}
```

### Initialize Wroob protocol

As an argument provide callback function where incoming message payload will be handled.

```c
void setup()
{
  ...
  wroob.begin(my_callback);
  ...
}
```

### Feed Wroob module

Feed your Wroob module with data from serial port.

```c
void loop()
{
  ... 
  wroob.feed();
  ...
}
```

### Sending data to Wroob system

To send any data from your module to Wroob system pack them into JSON object using [ArduinoJson](https://arduinojson.org/) API

```c
StaticJsonDocument<80> event;
event["name"] = "some data";
event["value"] = 48;
```

and then:

```c
wroob.sendMessage(event);
```

Do you like this library? Please [star this project on GitHub](https://github.com/wroob-io/arduino-wroobimp/stargazers)!
