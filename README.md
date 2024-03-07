# CNC Growbox

This repo is the first home CNC growbox programm for Arduino boards.

## Преимущества

- унифицирована работа с датчиками и исполнительными устройствами, параметризуемый алгоритм управления внутренней средой
- [G-коды](GCODE.md), для параметризации автоматики гроубокса "на лету".
- 3 вида автоматики, работающих псевдопараллельно:
  - Циклическая с резким переключением периода
  - Циклическая с плавной сменой периода
  - Климат-контроль

## G-Code parsers

Very simple
- https://github.com/tinkersprojects/G-Code-Arduino-Library

Interesting (commands, comments):
- https://github.com/tgolla/gcodeparser

Marlin:
- https://github.com/MarlinFirmware/Marlin/tree/bugfix-2.1.x/Marlin/src/gcode
