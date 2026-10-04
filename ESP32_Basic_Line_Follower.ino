// Practical Work 4 - Hybrid 3-Sensor + Turn Memory + Obstacle Stop
// ESP32 + 3 IR Sensors + L298N + HC-SR04

// ==========================================
// IR Sensors
// ==========================================
#define IR_LEFT 5
#define IR_CENTER 15   // <-- Mesti tambah sensor tengah
#define IR_RIGHT 34

// ==========================================
// Ultrasonic Sensor (HC-SR04)
// ==========================================
#define TRIG_PIN 4
#define ECHO_PIN 2

// ==========================================
// Motor Pins
// ==========================================
#define ENA 32
#define IN1 33
#define IN2 25
#define IN3 26
#define IN4 27
#define ENB 14

// ==========================================
// Motor Speed (Adjust ikut kekuatan bateri)
// ==========================================
const int MOTOR_SPEED = 85;   // Kelajuan lurus
const int TURN_SPEED = 115;    // Kelajuan pusing biasa
const int PIVOT_SPEED = 110;   // Kelajuan memori 90 darjah (kena lebih kuat untuk pivot)

// ==========================================
// IR Sensor Logic
// ==========================================
const int LINE_DETECTED = LOW; 
const int NO_LINE = HIGH;

// ==========================================
// Obstacle Settings
// ==========================================
const int OBSTACLE_CM = 15;
const int OBSTACLE_CONFIRM = 2;
int obstacleCount = 0;

// ==========================================
// Turn Memory Variable
// ==========================================
int turn_memory = 0; // 0 = Lurus, 1 = Kiri, 2 = Kanan

// ==========================================
// Sensor & Motor Functions
// ==========================================
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 3500);
  if (duration == 0) return 999;
  return duration * 0.0343 / 2.0;
}

void moveForward() {
  analogWrite(ENA, MOTOR_SPEED); analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

// Slight Turn (Satu tayar jalan, satu stop) - Untuk selekoh biasa
void turnLeft() {
  analogWrite(ENA, TURN_SPEED); analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void turnRight() {
  analogWrite(ENA, TURN_SPEED); analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
}

// Sharp Pivot Turn (Satu tayar forward, satu reverse) - Untuk 90 darjah
void sharpPivotLeft() {
  analogWrite(ENA, PIVOT_SPEED); analogWrite(ENB, PIVOT_SPEED);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // Kiri reverse
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Kanan forward
}
void sharpPivotRight() {
  analogWrite(ENA, PIVOT_SPEED); analogWrite(ENB, PIVOT_SPEED);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Kiri forward
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // Kanan reverse
}

void stopRobot() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ==========================================
// Setup
// ==========================================
void setup() {
  Serial.begin(115200);
  pinMode(IR_LEFT, INPUT); pinMode(IR_CENTER, INPUT); pinMode(IR_RIGHT, INPUT);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopRobot();
  delay(2000); // Masa untuk letak robot di trek
}

// ==========================================
// Main Loop
// ==========================================
void loop() {
  // 1. Semak Obstacle
  float distance = readDistanceCm();
  if (distance > 1.0 && distance <= OBSTACLE_CM) {
    obstacleCount++;
  } else {
    obstacleCount = 0;
  }

  if (obstacleCount >= OBSTACLE_CONFIRM) {
    stopRobot();
    return; // Langkau execution pergerakan, sambung pusingan loop baru
  }

  // 2. Baca IR Sensor
  int L = digitalRead(IR_LEFT);
  int C = digitalRead(IR_CENTER);
  int R = digitalRead(IR_RIGHT);

  // 3. Update Turn Memory (Simpan memori arah mana yang last nampak line)
  if (L == LINE_DETECTED && R == NO_LINE) turn_memory = 1; // Last nampak di kiri
  if (R == LINE_DETECTED && L == NO_LINE) turn_memory = 2; // Last nampak di kanan

  // Jika center nampak line, terus reset memori supaya tak salah pusing di jalan lurus
  if (C == LINE_DETECTED) turn_memory = 0; 

  // 4. Execution Pergerakan (Hybrid Logic)
  
  // KES 1: Terbabas sepenuhnya (Semua NO_LINE) - Trigger Memory 90 Darjah!
  if (L == NO_LINE && C == NO_LINE && R == NO_LINE) {
    if (turn_memory == 1) {
      sharpPivotLeft();
    } 
    else if (turn_memory == 2) {
      sharpPivotRight();
    } 
    else {
      stopRobot(); // Tiada memori & tiada line = berhenti dengan selamat
    }
  }
  
  // KES 2: Berada di atas line dengan sempurna (Center sahaja detect)
  else if (L == NO_LINE && C == LINE_DETECTED && R == NO_LINE) {
    moveForward();
  }
  
  // KES 3: Senget sikit ke kanan, perbetulkan ke kiri
  else if (L == LINE_DETECTED && C == LINE_DETECTED && R == NO_LINE) {
    turnLeft();
  }
  // KES 4: Terlalu senget ke kanan
  else if (L == LINE_DETECTED && C == NO_LINE && R == NO_LINE) {
    sharpPivotLeft();
  }
  
  // KES 5: Senget sikit ke kiri, perbetulkan ke kanan
  else if (L == NO_LINE && C == LINE_DETECTED && R == LINE_DETECTED) {
    turnRight();
  }
  // KES 6: Terlalu senget ke kiri
  else if (L == NO_LINE && C == NO_LINE && R == LINE_DETECTED) {
    sharpPivotRight();
  }
  
  // KES 7: Simpang T / Cross (Semua Detect Line)
  else if (L == LINE_DETECTED && C == LINE_DETECTED && R == LINE_DETECTED) {
    moveForward(); // Langgar je simpang tu terus ke depan
  }

  delay(5); // Stabilizer cip ESP32
}
