@startuml
title Washing Machine Controller - Finite State Machine

[*] --> OFF

OFF : LED OFF
OFF --> WASH : Button long press\n(> 2 seconds)

WASH : LED ON\nDuration = pot value
WASH --> RINSE : Wash time elapsed
WASH --> OFF : Button short press

RINSE : LED slow blink\n(1s ON / 1s OFF)
RINSE --> SPIN : (Wash time / 2) elapsed
RINSE --> OFF : Button short press

SPIN : LED fast blink\n(0.2s ON / 0.2s OFF)
SPIN --> OFF : (Wash time * 1.5) elapsed
SPIN --> OFF : Button short press

@enduml
