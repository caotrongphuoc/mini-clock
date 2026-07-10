<div align="center">
  
![Repo Traffic](https://komarev.com/ghpvc/?username=mini-clock&label=Repo+Traffic&color=blue&style=flat-square)

</div>

# Mini Clock - Digital Desk Clock built on AK Embedded Base Kit

## Demo

## Documentation

| File | Description |
|---|---|
| [README.md](README.md) | Main project overview, hardware information, operation, and object descriptions. |

## Introduction

Mini Clock is a digital desk clock built on the AK Embedded Base Kit.

This project focuses on building a simple clock application using an event-driven approach. It handles basic features such as displaying time, updating the screen, reading button inputs, and triggering an hourly chime.

Through this project, I practiced several embedded concepts:

* **System design:** Organizing the main features and logic of the clock.
* **Task management:** Splitting the program into small tasks such as time update, display update, and button handling.
* **Communication:** Using signals, timers, and messages to handle events.
* **State machine:** Managing clock modes such as normal display, setting time, and chime control.

### I. Hardware

<table align="center">
  <tr>
    <td align="center"><img src="hardware/images/ak-embedded-base-kit-version-3.jpg" alt="AK Embedded Base Kit - STM32L151 - v3.0" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> AK Embedded Base Kit - STM32L151</p>

[AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) is an evaluation kit aimed at intermediate and advanced embedded software learners.

The kit integrates a **1.54" OLED LCD**, **3 push buttons**, and **a buzzer** capable of playing short melodies, giving you everything you need to study **event-driven systems** through hands-on game-machine design.
It also exposes **RS485**, the **Qwiic Connect System**, and **Grove** connectors, so it doubles as a convenient prototyping board for real-world embedded projects.

**MCU Overview:**

```text
SoC Name : STM32L151CBT6
RAM      : 16 KB

Flash Partitions Layout
----------------------
[ 0x08000000 - 0x08001FFF ] : Bootloader Partition (8 KB)
=> AK Bootloader

[ 0x08002000 - 0x08002FFF ] : BSF Shared Partition (4 KB)
=> Used for data sharing between Bootloader and Application

[ 0x08003000 - 0x0801FFFF ] : Application Partition (116 KB)
=> Zomwar firmware
```

**MCU Naming Convention:**

| Part | Meaning |
|---|---|
| `STM32` | STMicroelectronics 32-bit MCU family. |
| `L` | Low-power series. |
| `151` | STM32L151 product line. |
| `C` | 48-pin package. |
| `B` | 128 KB Flash memory. |
| `T` | LQFP package. |
| `6` | Industrial temperature grade. |


<table align="center">
  <tr>
    <td align="center"><img src="hardware/images/board-view-top-bottom.png" width="900"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 2:</em></strong> Board view Top + Bottom </p>

## Contact & Support

<p style="font-size: 20px;"><strong>Cao Trong Phuoc</strong> - Software Engineer - Embedded Systems</p>

``` Note
Thank you for visiting this repository.
If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.
```

<a href="https://github.com/caotrongphuoc">
  <img src="https://img.shields.io/badge/GitHub-caotrongphuoc-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/cao-trong-phuoc/">
  <img src="https://img.shields.io/badge/LinkedIn-Cao%20Trong%20Phuoc-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>

<p style="font-size: 20px;"><strong>Josef Tan</strong> - Software Engineer - Embedded Systems</p>

> **Note**  
> Thank you for visiting this repository.  
> If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.

<a href="https://github.com/joseftanlang">
  <img src="https://img.shields.io/badge/GitHub-joseftanlang-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/josef-tan/">
  <img src="https://img.shields.io/badge/LinkedIn-Josef%20Tan-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>
