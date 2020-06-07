# Smart-Blinds
Web controlled blinds for automatic raising and lowering in Home Assistant.

## Build
[![Build Status](https://travis-ci.com/oschwartz10612/Smart-Blinds.svg?branch=master)](https://travis-ci.com/oschwartz10612/Smart-Blinds)

## MQTT Topics
`home-assistant/desk_cover/set`
Publish to set poition of the cover

`home-assistant/desk_cover/availability`
Cover published `online` ever 60 seconds

## MQTT Commands
`reset`
See "Resetting Position" Below

`open`
Sets the cover to the open position

`close`
Sets the cover to the closed position

`mid`
Sets the cover to the midway position

`inch`
Sometimes the stepper looses steps on the way up and down and the blinds become lower than they should be. By publishing `inch`, the blinds will move up 5000 steps and reset that position to 0.

## Home Assistant Configuration
```yaml
- platform: mqtt
    name: "Cover"
    command_topic: "home-assistant/desk_cover/set"
    position_topic: "home-assistant/desk_cover/position"
    availability_topic: "home-assistant/desk_cover/availability"
    set_position_topic: "home-assistant/desk_cover/set_position"
    qos: 0
    retain: true
    payload_open: "open"
    payload_close: "close"
    payload_stop: "mid"
    position_open: 1
    position_closed: 0
    payload_available: "online"
    payload_not_available: "offline"
    optimistic: false
    value_template: '{{ value.x }}'
```

## Reseting Position 
1. Publish `reset` to the `set` topic

2. Publish `open`, `mid`, or `closed` to the `set` topic to set the current postition.
