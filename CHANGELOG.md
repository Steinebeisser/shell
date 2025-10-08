# Changelog

## [0.4.1] - 2025-10-08
- windows imp for 0.3/0.4
    - implement set/unsetenv for windows
    - modify file persistance because windows rename fails if file exists
    - implement windows strndup
    - fixed windows getline implementation to work for empty lines

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
