This project provides a shared embedded game framework for STM32, where multiple games are integrated into a common menu system and runtime loop.

Each game operates independently while sharing input, rendering, and hardware resources.

## Controls

- **Joystick UP/DOWN**: Navigate menu
- **BT2 Button**: Available for custom game use
- **B3 Button**: Available for custom game use.
- **B1 Button**: Select menu option or custom game use

## Hardware Features

- **STM32L476 Microcontroller**
- **ST7789V2 LCD Display** (240×240)
- **Joystick Input** with 8-directional output
- **PWM LED** for visual effects
- **Buzzer** for sound effects
- **Timers**: TIM6 (100Hz) and TIM7 (1Hz) available for game timing

See driver folders (Joystick/, PWM/, Buzzer/) for API documentation.
See [TIMER_USAGE_GUIDE.md](TIMER_USAGE_GUIDE.md) for timer examples.
