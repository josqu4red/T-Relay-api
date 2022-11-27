# T-Relay-API

A REST API to control Lilygo T-Relay boards.

Only works with 4-relays boards at the moment, but easy to extend for 8-relays.

(This is a toy project to learn about Arduino)

## Setup

Follow instructions from https://github.com/Xinyuan-LilyGO/LilyGo-T-Relay and install dependencies. I used Arduino CLI.

Set your WiFi SSID and passphrase in the sketch file:
```
const char *ssid = ""; // SSID
const char *password = ""; // Password
```

# Build

Using Arduino CLI, with dependencies checked out under `libraries/` and code in `t-relay-api/`:
```
arduino-cli compile --build-property "compiler.cpp.extra_flags=-DASYNCWEBSERVER_REGEX" --libraries libraries/ -e -b esp32:esp32:esp32 t-relay-api/t-relay-api.ino
arduino-cli upload -b esp32:esp32:esp32 -p /dev/ttyACM0 t-relay-api/
```

# Usage

Get board IP address from serial.

```
$ curl http://192.168.1.69/relays
{
  "relay1": "down",
  "relay2": "down",
  "relay3": "down",
  "relay4": "down"
}

$ curl -XPUT http://192.168.1.69/relays/1/up
{
  "relay1": "up"
}
$ curl -XPUT http://192.168.1.69/relays/2/down
{
  "relay2": "down"
}
$ curl -XPUT http://192.168.1.69/relays/4/toggle
{
  "relay4": "up"
}

$ curl http://192.168.1.69/relays
{
  "relay1": "up",
  "relay2": "down",
  "relay3": "down",
  "relay4": "up"
}
```
