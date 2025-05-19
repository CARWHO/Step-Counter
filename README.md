# Step-Counter  
A firmware project for a step counter implemented on an STM32 Nucleo Board, utilizing an Inertial Measurement Unit (IMU) for step detection.

## 1. Description of Overall Project

This project implements a step counter on an STM32 microcontroller using an LSM6DS IMU. It processes sensor data and displays information on an SSD1306 OLED screen.

Key features:
- **Step Detection:** Uses filtered 3-axis accelerometer data and a peak detection algorithm.
- **Data Display:** OLED screen shows step count, distance (km/yards), step goal, goal progress, and joystick status.
- **User Interaction:** Joystick for menu navigation, display mode selection, and goal editing. Hardware buttons for manual step increment, UART diagnostics toggle, and test mode activation.
- **Step Goal Management:** User-settable daily step goal with potentiometer editing and achievement feedback.
- **Audible Feedback:** Buzzer alerts for events like goal achievement.
- **Test Mode:** Simulates step input via joystick for debugging.
- **Serial Data Output:** UART transmission of telemetry data, toggled by a button.
- **Visual Feedback:** RGB and onboard LEDs for status indication.

The system employs a modular C-based design for clarity and maintainability.

## 2. Justification and Description of Modularisation

The firmware is structured into distinct modules, each handling specific functionalities. This approach enhances code organization, readability, and independent development. For a detailed visual representation of the module interactions and data flow, please refer to the following diagram:

![Modularisation Diagram](361modularization.png)

Key module categories include:
- **Core Application Logic (`app.c`, `main.c`):** Initializes system and orchestrates tasks.
- **Sensor & Data Processing (`imu_sensor.c`, `peak_detect.c`, `noise_filter.c`):** Handles IMU data acquisition, filtering, and step identification.
- **User Interface & Display (`display.c`, `state_selector.c`, `ssd1306` library):** Manages OLED display content and UI state transitions.
- **Input Handling (`joystick_task.c`, `button_task.c`, `buttons.c`, `potentiometer.c`):** Processes inputs from joystick, buttons, and potentiometer.
- **Feature Modules (`counter_incrementer.c`, `test_mode.c`, `buzzer.c`, `serial.c`):** Implements step counting, goal management, test functionalities, alerts, and serial communication.
- **Low-Level Control (`rgb.c`, `pwm.c`, `led_blinking.c`, `blinky.c`):** Manages LEDs and PWM outputs.
- **Hardware Abstraction (STM32CubeMX generated HAL files):** Provides low-level drivers for MCU peripherals.

## 3. Analysis of Firmware Operation

The firmware operates on a cooperative multitasking model within a main super-loop in `app_main`, with each module polled at a fixed rate.

### 3.1 Measured execution times  
Using TIM2 at 48 MHz and the in-code profiling technique from Lecture 24, we measured each task’s duration in ticks and converted to microseconds:

| Task                      | Freq (Hz) | Ticks Taken | Time per Call (µs)     |
|---------------------------|----------:|------------:|-----------------------:|
| button_task_execute       |       300 |       1 283 | 1 283 / 48 ≈ 26.7       |
| buzzer_execute            |         2 |         134 |   134 / 48 ≈ 2.8        |
| display_execute           |         4 |      30 574 | 30 574 / 48 ≈ 636.5     |
| joystick_task_execute     |        20 |         790 |   790 / 48 ≈ 16.5       |
| test_mode_execute         |        20 |         853 |   853 / 48 ≈ 17.8       |
| serial_task_execute       |       500 |      36 844 | 36 844 / 48 ≈ 768.4     |

### 3.2 Deadline analysis  
For each task _k_, the available time per invocation is  
`P_k = 1 / F_k [s]`,  
and the execution time is `T_k`. We check `T_k < P_k` for all tasks:

| Task                    | Period \(P_k\) | Exec. time \(T_k\) | Meets deadline? |
|-------------------------|--------------:|-------------------:|:---------------:|
| button_task_execute     | 3.33 ms       | 0.027 ms           | ✓               |
| buzzer_execute          | 500 ms        | 0.003 ms           | ✓               |
| display_execute         | 250 ms        | 0.637 ms           | ✓               |
| joystick_task_execute   | 50 ms         | 0.016 ms           | ✓               |
| test_mode_execute       | 50 ms         | 0.018 ms           | ✓               |
| serial_task_execute     | 2 ms          | 0.768 ms           | ✓               |

### 3.3 CPU load & schedulability  
Using the CPU-load formula `L_k = T_k × F_k` from Lecture 25, total load `L_tot = sum(L_k)`:

| Task                    | \(T_k\) (s)        | \(F_k\) (Hz) | \(L_k=T_kF_k\)       |
|-------------------------|---------------:|-------------:|---------------------:|
| button_task_execute     | 0.0000267         |        300   | 0.0080 (0.8 %)       |
| buzzer_execute          | 0.0000028         |          2   | 0.000006 (0.0006 %)  |
| display_execute         | 0.0006365         |          4   | 0.00255 (0.25 %)     |
| joystick_task_execute   | 0.0000165         |         20   | 0.00033 (0.033 %)    |
| test_mode_execute       | 0.0000178         |         20   | 0.00036 (0.036 %)    |
| serial_task_execute     | 0.0007684         |        500   | 0.3842 (38.4 %)      |

L_tot ≈ 0.0080 + 0.000006 + 0.00255 + 0.00033 + 0.00036 + 0.3842 ≈ 0.3955 < 1

Since `L_tot ≈ 0.40 < 1`, the task set is schedulable under Rate-Monotonic priorities.

### 3.4 Interpretation  
- **All tasks meet their deadlines** with comfortable margins.  
- **Total CPU load ≈ 40 %**, leaving ~60 % headroom for future extensions or higher-level functions.  
- Assigning priorities by frequency (highest `F_k` → highest priority) ensures predictable preemption and latency behavior.

## 4. Conclusion

This profiling and schedulability analysis demonstrate that our firmware:
- Executes all core tasks well within their timing budgets.
- Leaves ample CPU slack (~60 %) for additional features or unplanned interrupts.
- Adheres to a clean, modular design that simplifies both reasoning about timing and future maintenance.

**Potential future enhancements** could include:
- Optimizing the most time-critical code paths (e.g., UART transmit) via DMA.  
- Adding dynamic power management during idle periods.  
- Extending the scheduler to support occasional higher-priority tasks (e.g., Bluetooth or higher-rate sampling).