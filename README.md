# gaggia-esp
Gaggia Classic + Home Assistant

Existing project replaces the Gaggia Classic's mechanical thermostat with a 4mm Type K thermocouple tied in to a Max6675, interfaced to an ESP8266. r-downing's AutoPID library is used for PID algorithm, outputting PWM to a FosTek 25A SSR.

The ESP8266 reports boiler temperature and raw controller output to home assistant via MQTT - this allows me to see when the boiler is fully up to temperature and stable.

I plan on eventually monitoring button presses to have a fixed PWM value during shot pulling, but the current control strategy is moderate gain with an aggressive derivative term to take action as soon as temperature starts to drop.

Future plans include a flow meter + SSR for the pump to volumetrically dose shots. Thanks much to the many gaggia/arduino pioneers ahead of me for the inspiration.
