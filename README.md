# 🐍 Snake Game - Arduino

A simple **Snake Game** built for **Arduino UNO**, using a **joystick** and **MAX7219 8x8 LED matrix**.

---

## Table of Contents
- [What's in the Repo?](#whats-in-the-repo)
- [Game Demo](#game-demo)
- [About the Game](#about-the-game)
- [Components](#components)
- [How to Play](#how-to-play)
- [Setup & Wiring](#setup--wiring)
- [Code & Installation](#code--installation)

---

## What's in the Repo?
- **Game Code** (Arduino Sketch)
- **Example Video** (Gameplay)
- **Circuit Photo** (Wiring Diagram)

---

## Game Demo
▶ **[Watch the Demo](https://drive.google.com/file/d/1jzhvqaAOAJd3xr2Pa_UUV-WztmFv7yg7/view?usp=drive_link)**

---

## About the Game
This is a **classic arcade Snake game** where you control a snake using a **joystick** on an **8x8 LED matrix**. The goal is to **eat apples** and grow without hitting the walls or yourself.

---

## Components Required
- **Arduino UNO**
- **MAX7219 8x8 LED Matrix**
- **Joystick Module**
- **Jumper Wires**
- **Breadboard (Optional)**

---

## How to Play?
1. **Move the joystick** to control the snake.
2. **Eat apples** to grow longer.
3. **Avoid crashing** into walls or yourself.
4. **Game over?** Try again!

---

## Setup & Wiring
| Arduino UNO | MAX7219 8x8 Matrix |
|------------|------------------|
| 5V         | VCC              |
| GND        | GND              |
| 10         | DIN              |
| 11         | CLK              |
| 12         | CS               |

| Arduino UNO | Joystick Module |
|------------|----------------|
| 5V         | VCC            |
| GND        | GND            |
| A0         | VRx            |
| A1         | VRy            |
| 2          | SW (Optional)  |

---

## 📝 Code & Installation
1. **Clone this repository**  
   ```sh
   git clone https://github.com/your-username/snake-game-arduino.git
