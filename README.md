# 🤖 Line Follower Robot with Smart Recovery System

An intelligent **Arduino-based Line Follower Robot** featuring a smart recovery algorithm, adaptive movement logic, and autonomous line-search behavior.

This robot uses:

- **3 IR Sensors**
- **TB6612FNG Motor Driver**
- **Dual DC Motors**
- Advanced recovery state machine for lost-line situations

---

# 📖 Overview

This project implements a smart autonomous line follower capable of:

- Following black lines accurately
- Detecting intersections
- Making smooth and sharp turns
- Recovering automatically when the line is lost
- Remembering the last known line direction

Unlike basic line followers, this robot includes a **multi-step intelligent recovery system** that improves navigation reliability.

---

# ✨ Features

## 🧠 Smart Decision-Making System
The robot reads all 3 sensors together and makes combined movement decisions instead of reacting to sensors independently.

---

## 🔄 Intelligent Recovery Mode
When the robot loses the line, it automatically:

1. Moves backward
2. Spins toward the last known direction
3. Sweeps the opposite side
4. Creeps forward slowly
5. Performs a full 180° sweep

This creates a robust autonomous search system.

---

## 📍 Line Tracking Behaviors

### ✅ Perfect Alignment
- Moves straight when centered on the line

### ↩️ Soft Corrections
- Gentle left/right corrections for small deviations

### 🔁 Hard Turns
- Sharp turns for strong deviations

### ➕ Intersection Handling
- Detects wide lines or intersections

---

# 🛠️ Hardware Components

| Component | Quantity |
|---|---|
| Arduino UNO/Nano | 1 |
| TB6612FNG Motor Driver | 1 |
| IR Sensors | 3 |
| DC Motors | 2 |
| Robot Chassis | 1 |
| Wheels | 2 |
| Battery Pack | 1 |

---

# ⚙️ Pin Configuration

## 📡 IR Sensors

| Sensor | Pin |
|---|---|
| Left Sensor | D2 |
| Right Sensor | D3 |
| Middle Sensor | D4 |

---

## ⚡ Motor Driver (TB6612FNG)

| Function | Pin |
|---|---|
| AIN1 | D8 |
| AIN2 | D9 |
| BIN1 | D10 |
| BIN2 | D11 |
| PWMA | D5 |
| PWMB | D6 |
| STBY | D7 |

---

# 🧠 Recovery State Machine

The recovery system operates using timed states:

| Step | Action | Purpose |
|---|---|---|
| 0 | Backward | Recover from overshooting |
| 1 | Spin toward last direction | Search likely path |
| 2 | Sweep opposite side | Wider exploration |
| 3 | Slow forward creep | Search ahead carefully |
| 4 | Full spin | Last resort exploration |

---

# 🚀 How It Works

## Normal Tracking

The robot continuously reads:

- Left sensor
- Middle sensor
- Right sensor

Then decides whether to:

- Move forward
- Soft left/right
- Hard left/right
- Enter recovery mode

---

# 📂 Project Structure

```bash
LineFollowerRobot/
│
├── line_follower.ino
└── README.md
```

---

# ▶️ Getting Started

## 1️⃣ Install Arduino IDE

Download:

- Arduino IDE

---

## 2️⃣ Connect Hardware

Wire:

- IR sensors
- TB6612FNG driver
- Motors
- Power supply

according to the pin configuration.

---

## 3️⃣ Upload the Code

1. Open Arduino IDE
2. Paste the code
3. Select your board
4. Upload to Arduino

---

# 🔍 Serial Monitor Debugging

The robot prints sensor readings to the Serial Monitor:

```bash
1  0  1
```

Format:

```bash
LEFT  MIDDLE  RIGHT
```

Where:

- `1` = line detected
- `0` = no line

---

# 🧪 Motion Functions

The project includes multiple movement modes:

| Function | Description |
|---|---|
| `forward()` | Full forward |
| `backward()` | Reverse movement |
| `softLeft()` | Gentle left correction |
| `softRight()` | Gentle right correction |
| `turnLeft()` | Sharp left turn |
| `turnRight()` | Sharp right turn |
| `spinLeft()` | Rotate left in place |
| `spinRight()` | Rotate right in place |
| `creep()` | Slow forward search |
| `stopMotors()` | Stop all motors |

---

# 🧠 Concepts Used

This project demonstrates:

- Embedded Systems
- Arduino Programming
- Robotics
- Sensor Fusion
- Finite State Machines
- Autonomous Navigation
- Real-Time Decision Making

---

# 🔮 Future Improvements

Possible upgrades:

- PID control
- Speed auto-calibration
- Bluetooth control
- OLED display
- Obstacle avoidance
- AI-based path optimization
- Maze solving
- WiFi telemetry

---

# 📚 Educational Purpose

This project is ideal for learning:

- Arduino robotics
- Motor control
- Sensor integration
- Autonomous systems
- Embedded programming

---

# 🤝 Contributing

Contributions are welcome.

1. Fork the repository
2. Create a feature branch
3. Commit changes
4. Push the branch
5. Open a Pull Request

---

# 📄 License

This project is for educational and learning purposes.

---

# ⭐ Support

If you like this project, give it a ⭐ on GitHub.
