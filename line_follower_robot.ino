// ═══════════════════════════════════════════════════════════
//  LINE FOLLOWER ROBOT — Smart Brain + Recovery System
//  Motors: TB6612FNG driver
//  Sensors: 3x IR digital sensors (LEFT, MIDDLE, RIGHT)
// ═══════════════════════════════════════════════════════════

// ── Sensor Pins ──
#define LEFT_SENSOR   2
#define RIGHT_SENSOR  3
#define MIDDLE_SENSOR 4

// ── Motor Driver Pins (TB6612FNG) ──
#define AIN1 8   // Motor A direction pin 1
#define AIN2 9   // Motor A direction pin 2
#define BIN1 10  // Motor B direction pin 1
#define BIN2 11  // Motor B direction pin 2
#define PWMA 5   // Motor A speed (PWM)
#define PWMB 6   // Motor B speed (PWM)
#define STBY 7   // Standby pin — must be HIGH to enable driver

// ── Motor Speed (0–255) — pre-calibrated, do not change ──
int speedA = 180;  // Left motor speed
int speedB = 180;  // Right motor speed

// ── Memory: remembers which side the line was last seen ──
// -1 = left side | 0 = unknown | 1 = right side
int lastDirection = 0;

// ── Recovery State Machine ──
// Tracks which recovery step the robot is currently executing
int recoveryStep = 0;

// Timestamp of when the current recovery step started
unsigned long recoveryTimer = 0;

// ═══════════════════════════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════════════════════════
void setup() {
  // Sensor pins as inputs
  pinMode(LEFT_SENSOR,   INPUT);
  pinMode(RIGHT_SENSOR,  INPUT);
  pinMode(MIDDLE_SENSOR, INPUT);

  // Motor control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Enable motor driver (pull STBY high)
  digitalWrite(STBY, HIGH);

  Serial.begin(9600);
}

// ═══════════════════════════════════════════════════════════
//  MAIN LOOP
// ═══════════════════════════════════════════════════════════
void loop() {
  // Read all three sensors each cycle
  int left   = digitalRead(LEFT_SENSOR);
  int middle = digitalRead(MIDDLE_SENSOR);
  int right  = digitalRead(RIGHT_SENSOR);

  // Print sensor states to Serial Monitor for debugging
  // Format: LEFT  MIDDLE  RIGHT  (1 = on line, 0 = off line)
  Serial.print(left);
  Serial.print("  ");
  Serial.print(middle);
  Serial.print("  ");
  Serial.println(right);

  // Pass all sensor data to the brain for a combined decision
  brain(left, middle, right);

  delay(50); // Small delay to stabilize sensor readings
}

// ═══════════════════════════════════════════════════════════
//  BRAIN — combined sensor logic
//  Reads all three sensors together to make smarter decisions
//  instead of reacting to each sensor individually
// ═══════════════════════════════════════════════════════════
void brain(int left, int middle, int right) {

  // True if ANY sensor sees the line
  bool lineDetected = left || middle || right;

  // ── Line is visible — normal tracking mode ──
  if (lineDetected) {

    // Line found again — reset recovery so next loss starts fresh
    recoveryStep = 0;

    if (left && middle && right) {
      // All sensors on line → intersection or very wide line
      // Best action: keep going straight through it
      forward();
    }
    else if (middle && left && !right) {
      // Robot drifting right — line is pulling to the left
      // Gentle left correction before it becomes a hard deviation
      lastDirection = -1;
      softLeft();
    }
    else if (middle && !left && right) {
      // Robot drifting left — line is pulling to the right
      // Gentle right correction before it becomes a hard deviation
      lastDirection = 1;
      softRight();
    }
    else if (middle && !left && !right) {
      // Only middle sensor on line → perfectly centered
      // No correction needed, full speed ahead
      forward();
    }
    else if (!middle && left && !right) {
      // Only left sensor sees line → sharp left deviation
      // Apply hard left turn to get back on track
      lastDirection = -1;
      turnLeft();
    }
    else if (!middle && !left && right) {
      // Only right sensor sees line → sharp right deviation
      // Apply hard right turn to get back on track
      lastDirection = 1;
      turnRight();
    }
    else if (left && !middle && right) {
      // Both sides see line but not middle → ambiguous split (Y-junction?)
      // Safest choice: hold straight and let middle sensor resolve it
      forward();
    }
  }

  // ── Line is completely lost — enter recovery mode ──
  else {
    recover();
  }
}

// ═══════════════════════════════════════════════════════════
//  RECOVERY — intelligent exploration when line is lost
//
//  Uses a timed state machine so each step runs for a fixed
//  duration without blocking the sensor-reading loop.
//
//  Strategy (in order):
//    Step 0 → Back up        (maybe we overshot the line)
//    Step 1 → Spin toward last known direction  (most likely side)
//    Step 2 → Spin opposite way                 (wider sweep)
//    Step 3 → Creep forward slowly              (line might be ahead)
//    Step 4 → Full 180° spin                    (last resort)
//    Loop  → Repeat from step 1 indefinitely until line is found
// ═══════════════════════════════════════════════════════════
void recover() {
  unsigned long now = millis(); // Current timestamp for timing each step

  switch (recoveryStep) {

    case 0:
      // ── Step 0: Back up briefly ──
      // The robot may have overshot a curve or a gap in the line.
      // Reversing gives the rear sensors a chance to re-detect it.
      Serial.println("Recovery: backing up...");
      backward();
      if (now - recoveryTimer > 300) {   // Run for 300ms
        recoveryTimer = now;
        recoveryStep  = 1;
      }
      break;

    case 1:
      // ── Step 1: Spin toward last known direction ──
      // The line most likely continues in the direction we were tracking.
      // Use stored lastDirection memory to spin the right way first.
      Serial.println("Recovery: spinning to last known side...");
      if      (lastDirection == -1) spinLeft();
      else if (lastDirection ==  1) spinRight();
      else                          spinLeft(); // Default if direction unknown
      if (now - recoveryTimer > 400) {   // Run for 400ms
        recoveryTimer = now;
        recoveryStep  = 2;
      }
      break;

    case 2:
      // ── Step 2: Sweep the opposite direction ──
      // Step 1 didn't find the line. Try a wider arc in the other direction.
      // Duration is longer to cover more ground.
      Serial.println("Recovery: sweeping opposite side...");
      if (lastDirection == -1) spinRight();
      else                     spinLeft();
      if (now - recoveryTimer > 700) {   // Run for 700ms
        recoveryTimer = now;
        recoveryStep  = 3;
      }
      break;

    case 3:
      // ── Step 3: Creep forward slowly ──
      // Spinning didn't work. The line might be further ahead.
      // Move forward at low speed to reach it without overshooting again.
      Serial.println("Recovery: creeping forward...");
      creep();
      if (now - recoveryTimer > 500) {   // Run for 500ms
        recoveryTimer = now;
        recoveryStep  = 4;
      }
      break;

    case 4:
      // ── Step 4: Full 180° spin — last resort ──
      // Nothing worked. Do a long spin to cover all directions.
      // After this, forget last direction and loop back to keep exploring.
      Serial.println("Recovery: full sweep...");
      spinLeft();
      if (now - recoveryTimer > 1000) {  // Run for 1000ms
        recoveryTimer = now;
        recoveryStep  = 1;   // Loop back — keep trying indefinitely
        lastDirection = 0;   // Forget last direction, explore freely
      }
      break;
  }
}

// ═══════════════════════════════════════════════════════════
//  MOTION FUNCTIONS
// ═══════════════════════════════════════════════════════════

// Both motors forward at full speed
void forward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

// Both motors backward at full speed (used in recovery step 0)
void backward() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

// Both motors forward at low speed — careful exploration
void creep() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, 80);  // ~44% of full speed
  analogWrite(PWMB, 80);
}

// ── Soft Turns — one motor off, one at full speed ──
// Used for gentle mid-course corrections while line is visible

// Right motor drives, left motor idle → curves left gently
void softLeft() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, speedB);
}

// Left motor drives, right motor idle → curves right gently
void softRight() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, 0);
}

// ── Hard Turns — one motor off, one at full speed ──
// Used for sharp single-sensor deviations

// Right motor drives, left motor idle → hard left
void turnLeft() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, speedB);
}

// Left motor drives, right motor idle → hard right
void turnRight() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, 0);
}

// ── Full Spins — motors run in opposite directions ──
// Used in recovery for fast pivoting in place

// Left motor backward, right motor forward → spins left on the spot
void spinLeft() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

// Left motor forward, right motor backward → spins right on the spot
void spinRight() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

// Cut power to both motors immediately
void stopMotors() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}
