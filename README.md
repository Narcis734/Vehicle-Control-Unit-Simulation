# Vehicle-Control-Unit-Simulation

Embedded C simulation of a Vehicle Control Unit (VCU) with dual-channel pedal
validation, torque control, fault handling, and Unity unit tests.

## Behavior

- Two accelerator channels use normalized integer values from 0 to 1000 permille.
- Middleware rejects out-of-range channels and channel disagreement greater than
  50 permille. A valid position is the integer average of the channels.
- Application states are `INIT`, `READY`, `DRIVE`, and latched `FAULT`.
  Enable moves `INIT` to `READY`; a pedal request above the 10-permille deadband
  moves `READY` to `DRIVE`. Releasing the pedal returns to `READY`.
- Torque is scaled linearly from 0 to 3000 Nm. State transitions and all fault
  conditions command zero torque.
- Initialization times out after 5000 ms. A control-cycle interval above 100 ms
  is treated as a timing fault.
- A fault can only be cleared with an explicit clear request, valid neutral pedal
  signals, and enable switched off. Clearing returns to `INIT`; it does not jump
  directly to `READY`.

The pedal units and torque limit are simulation parameters, not vehicle-specific
calibration values. This project is an educational logic simulation and is not
validated or suitable for controlling real vehicle hardware.

## Build and run

Requirements: CMake 3.16+, a C11 compiler, and Git (to fetch the Unity Test
Framework dependency on the first configure). The VCU implementation itself is
written in C99.

```powershell
cmake -S . -B build
cmake --build build
.\build\Debug\vcu_demo.exe
ctest --test-dir build --output-on-failure
```

With a single-configuration generator, the demo executable is usually at
`build\vcu_demo.exe`.

## Layout

```text
include/                 Public VCU, input-simulation, and pedal interfaces
src/input_sim/           Software input source
src/middleware/          Pedal range and plausibility parsing
src/application/         State machine and torque-request logic
app/                     Demonstration executable
tests/                   Unity Test Framework unit tests
```
