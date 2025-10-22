# Changelog

## [0.9.0] - 2025-10-22
- bug fixes, help improvements
    - fixed quotes not parsing correctly
    - added help for config fields with help aliases
    - added config help
    - added help color

## [0.8.0] - 2025-10-15
- internal restruct, config, extend help
    - xmacro for creating config
    - removing blaot by autogenerating entries and parsing based on type
    - better scalability/maintainablity

---

## [0.7.0] - 2025-10-12
- internal restructure, help cmd
    - source cmd
    - x macros for creating commands and automatic help

---

## [0.6.1] - 2025-10-10
- Windows Imp for 0.6

## [0.6.0] - 2025-10-10
- Raw Mode/Interactive Input
    - change to raw mode
    - enable ansi escape sequence chars like arrows
    - history with rc options
    - finally make timeout parseable

---

## [0.4.1] - 2025-10-08
- windows imp for 0.3/0.4
    - implement set/unsetenv for windows
    - modify file persistance because windows rename fails if file exists
    - implement windows strndup
    - fixed windows getline implementation to work for empty lines

---

## [0.5.0] - 2025-10-06
- Set/Unset/Get Builtins

---

## [0.4.0] - 2025-10-03
- Envs + correct args
    - added envs (set(env))
    - fixed argument parsing, now strings are 1
    - added allow\_env\_override

---

## [0.3.0] - 2025-09-30
- More RC option
    - added show exit code
    - show cmd path
    - show expanded alias
    - example rc file with explanations
    - updated syntax file

---

## [0.2.0] - 2025-09-30
- Windows release
    - timeout option (windows only)
    - updated syntax file

---

## [0.1.0] - 2025-09-28
- Initial Release
    - unix only
    - simple rc file
    - exit
    - external commands
    - vim syntax file
