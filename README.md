# mc_brushed_driver

Low level communication drivers for the Brushed Motor Controller dev board
running the [mc_brushed](https://github.com/scalpelspace/mc_brushed) firmware.

---

<details markdown="1">
  <summary>Table of Contents</summary>

<!-- TOC -->
* [mc_brushed_driver](#mc_brushed_driver)
  * [1 Overview](#1-overview)
  * [2 CAN Bus Drivers](#2-can-bus-drivers)
<!-- TOC -->

</details>

---

## 1 Overview

The Brushed Motor Controller dev board provides 2 direct communication
interfaces:

1. UART
2. CAN (classic)

**CAN** us supported natively through the software included in this driver
package.

CAN drivers are included in the [mc_brushed_driver.h](mc_brushed_driver.h) file
for simple implementation.

---

## 2 CAN Bus Drivers

CAN drivers are implemented via the [
`can_driver`](https://github.com/scalpelspace/can_driver) submodule.
