# mc_brushed_driver

Low level communication drivers for the brushed motor controller board running
the [`mc_brushed`](https://github.com/scalpelspace/mc_brushed) firmware.

---

<details markdown="1">
  <summary>Table of Contents</summary>

<!-- TOC -->
* [mc_brushed_driver](#mc_brushed_driver)
  * [1 Overview](#1-overview)
  * [2 CAN Bus Drivers](#2-can-bus-drivers)
  * [3 CAN Bus Messages](#3-can-bus-messages)
    * [3.1 Message Table](#31-message-table)
    * [3.2 Muxed Setpoint (`command_brushed`)](#32-muxed-setpoint-command_brushed)
    * [3.3 Muxed PID Gains (`controls_config_*`)](#33-muxed-pid-gains-controls_config_)
    * [3.4 Encoder Configuration (`encoder_config_*`)](#34-encoder-configuration-encoder_config_)
<!-- TOC -->

</details>

---

## 1 Overview

CAN drivers are included in the [`mc_brushed_driver.h`](mc_brushed_driver.h)
file for simple implementation.

This repository defines the CAN **protocol contract** for MC Brushed: message
IDs, DLCs, signal layouts and scaling. The source of truth is the DBC file: [
`can_mc_brushed.dbc`](can_mc_brushed.dbc).

> **Note:** Behavioural semantics (controls state machine, safety gating of
> config commands, telemetry rates, fault handling) are owned by the
> [`mc_brushed`](https://github.com/scalpelspace/mc_brushed) firmware and are
> documented there.

---

## 2 CAN Bus Drivers

CAN drivers are implemented via the [
`can_driver`](https://github.com/scalpelspace/can_driver) submodule.

All CAN IDs follow the ScalpelSpace node CAN ID standard defined by
`can_driver`: the 11-bit classic CAN ID is split into a 6-bit `message_id` and
a 5-bit `node_id`, so a node's on-bus ID is its base message ID plus its
assigned node ID. Node IDs `0` (unassigned) and `31` (broadcast) are reserved,
and node IDs are assigned at runtime via the dynamic allocation protocol.

> See the [`can_driver` README](can_driver/README.md) for the full ID scheme,
> reserved ID ranges and the node ID allocation protocol.

---

## 3 CAN Bus Messages

### 3.1 Message Table

Direction is from the MC Brushed node's perspective: **TX** = transmitted by
MC Brushed, **RX** = received by MC Brushed.

| Base ID (hex) | Message                    | DLC | Direction | Description                                                                                                                                         |
|--------------:|----------------------------|----:|-----------|-----------------------------------------------------------------------------------------------------------------------------------------------------|
|          0x20 | `state`                    |   4 | TX        | Periodic system state, fault byte and DRV8873S fault/diag registers.                                                                                |
|          0x40 | `command_brushed`          |   6 | RX        | Control mode, enable, clear faults and muxed setpoint (see [3.2 Muxed Setpoint (`command_brushed`)](#32-muxed-setpoint-command_brushed)).           |
|          0x60 | `command_brushed_zero`     |   1 | RX        | Software zero of the position reference.                                                                                                            |
|          0x80 | `sensor`                   |   8 | TX        | IPROPI current sense count, encoder raw count and unwrapped position.                                                                               |
|          0xA0 | `controls_diagnostic`      |   8 | TX        | Control loop internals: torque/velocity/position errors and duty command.                                                                           |
|          0xC0 | `controls_config_set`      |   8 | RX        | Set closed loop PID gains for a muxed cascade stage (see [3.3 Muxed PID Gains (`controls_config_*`)](#33-muxed-pid-gains-controls_config_)).        |
|          0xE0 | `controls_config_get`      |   2 | RX        | Request PID gains for a cascade stage.                                                                                                              |
|         0x100 | `controls_config_response` |   8 | TX        | Response to `controls_config_get`.                                                                                                                  |
|         0x120 | `encoder_config_set`       |   4 | RX        | Set the quadrature encoder counts-per-revolution (see [3.4 Encoder Configuration (`encoder_config_*`)](#34-encoder-configuration-encoder_config_)). |
|         0x140 | `encoder_config_get`       |   1 | RX        | Request the encoder mode and counts-per-revolution.                                                                                                 |
|         0x160 | `encoder_config_response`  |   4 | TX        | Response to `encoder_config_get`.                                                                                                                   |
|         0x180 | `datetime_set`             |   7 | RX        | Set the RTC date and time.                                                                                                                          |
|         0x1A0 | `datetime_get`             |   0 | RX        | Request the RTC date and time.                                                                                                                      |
|         0x1C0 | `datetime_get_response`    |   7 | TX        | Response to `datetime_get`.                                                                                                                         |
|         0x1E0 | `rgb_led_set`              |   3 | RX        | Set the on-board RGB LED colour.                                                                                                                    |
|         0x3C0 | `version_get`              |   0 | RX        | Request the firmware version.                                                                                                                       |
|         0x3E0 | `version_get_response`     |   4 | TX        | Response to `version_get`: major, minor, patch and identifier.                                                                                      |

### 3.2 Muxed Setpoint (`command_brushed`)

`command_brushed` carries a single 32-bit setpoint field (bytes 2..5)
multiplexed by the `control_mode` signal:

| `control_mode` | Active setpoint signal | Unit            | Scale  |
|---------------:|------------------------|-----------------|--------|
|              1 | `target_torque`        | N*m             | 0.0001 |
|              2 | `target_velocity`      | rad/s           | 0.001  |
|              3 | `target_position_rel`  | rad             | 0.001  |
|              4 | `target_hbridge_ol`    | duty \[-1, +1\] | 0.0001 |

All setpoint signals are signed 32-bit. A `control_mode` of 0 (or any unlisted
value) carries no setpoint, allowing frames that only toggle `enable` or
`clear_faults`.

### 3.3 Muxed PID Gains (`controls_config_*`)

`controls_config_set`, `controls_config_get` and `controls_config_response`
share a 3-bit `controller` mux selector choosing which cascade stage the PID
gains target:

| `controller` | Cascade stage | Active gain signals                            |
|-------------:|---------------|------------------------------------------------|
|            0 | Torque        | `torque_k_p`, `torque_k_i`, `torque_k_d`       |
|            1 | Velocity      | `velocity_k_p`, `velocity_k_i`, `velocity_k_d` |
|            2 | Position      | `position_k_p`, `position_k_i`, `position_k_d` |

The gain slots occupy fixed frame positions regardless of the selected
controller: `k_p` in bytes 2..3, `k_i` in bytes 4..5 and `k_d` in bytes 6..7.
All gain signals are unsigned 16-bit with 0.001 scaling (range 0..65.535).
`controls_config_response` echoes the requested `controller` value, unlisted
selector values are ignored (no gain change, no response).

### 3.4 Encoder Configuration (`encoder_config_*`)

`encoder_config_set` adjusts the quadrature encoder counts-per-revolution
(`quadrature_count`, unsigned 16-bit, counts) used for the counts to radians
conversion. `encoder_config_get` is answered with an `encoder_config_response`
reporting the active encoder `mode` and counts-per-revolution:

| `mode` | Encoder backend                   |
|-------:|-----------------------------------|
|      0 | AB quadrature (relative) encoder  |
|      1 | AS5047P magnetic absolute encoder |

The `mode` is read-only: the backend is selected at compile time (it requires
direct pin mode configuration) and cannot be changed over CAN. A
`quadrature_count` set request is only honoured in quadrature mode and only at
standstill. Applying a new counts-per-revolution re-zeroes the position
reference (recalibration semantics). Requests in absolute mode, at speed, or
with a `quadrature_count` of 0 are dropped. In absolute mode the response
reports a `quadrature_count` of 0 (no quadrature configuration exists).
