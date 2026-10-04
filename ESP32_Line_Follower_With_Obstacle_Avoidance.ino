// Practical Work 4 - Hybrid 3-Sensor + Turn Memory + Obstacle Stop/Avoid
// ESP32 + 3 IR Sensors + L298N + HC-SR04

// ==========================================
// 1. PIN SETTINGS
// ==========================================
#define IR_LEFT 5
#define IR_CENTER 15
#define IR_RIGHT 34

#define TRIG_PIN 4
#define ECHO_PIN 2

#define ENA 32
#define IN1 33
#define IN2 25
#define IN3 26
#define IN4 27
#define ENB 14

// ==========================================
// 2. LINE FOLLOWING MOTOR SPEED SETTINGS
// ==========================================
const int MOTOR_SPEED = 85;    // Kelajuan lurus semasa ikut line
const int TURN_SPEED = 115;    // Kelajuan pusing biasa
const int PIVOT_SPEED = 110;   // Kelajuan pusing memori 90 darjah

// ==========================================
// 2.1 AVOIDANCE SPEED SETTINGS (KHAS UNTUK ELAK HALANGAN)
// ==========================================
const int AVOID_SPEED_FORWARD = 125; // Ditingkatkan supaya motor tak sangkut dari keadaan berhenti
const int AVOID_SPEED_TURN = 120;    // Kelajuan pusing semasa elak halangan

// ==========================================
// 3. OBSTACLE SETTINGS
// ==========================================
const int OBSTACLE_CM = 4;        // Jarak brek bila nampak halangan (10cm)
const int OBSTACLE_CONFIRM = 2;    // Filter untuk elak sensor salah baca
int obstacleCount = 0;

// ==========================================
// 4. SYSTEM VARIABLES
// ==========================================
const int LINE_DETECTED = LOW; 
const int NO_LINE = HIGH;
int turn_memory = 0; // 0 = Lurus, 1 = Kiri, 2 = Kanan

// ==========================================
// 5. SENSOR & LINE FOLLOWING FUNCTIONS
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

// Fungsi pergerakan untuk LINE FOLLOWING
void moveForward() {
  analogWrite(ENA, MOTOR_SPEED); analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

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

void sharpPivotLeft() {
  analogWrite(ENA, PIVOT_SPEED); analogWrite(ENB, PIVOT_SPEED);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); 
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  
}

void sharpPivotRight() {
  analogWrite(ENA, PIVOT_SPEED); analogWrite(ENB, PIVOT_SPEED);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); 
}

void stopRobot() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ==========================================
// 5.1 SPECIAL MOVEMENT FUNCTIONS FOR AVOIDANCE
// ==========================================
void avoidMoveForward() {
  analogWrite(ENA, AVOID_SPEED_FORWARD); analogWrite(ENB, AVOID_SPEED_FORWARD);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void avoidPivotLeft() {
  analogWrite(ENA, AVOID_SPEED_TURN); analogWrite(ENB, AVOID_SPEED_TURN);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); 
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  
}

void avoidPivotRight() {
  analogWrite(ENA, AVOID_SPEED_TURN); analogWrite(ENB, AVOID_SPEED_TURN);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); 
}

// ==========================================
// 6. OBSTACLE AVOIDANCE ROUTINE (MANUAL SEQUENCE)
// ==========================================
void avoidObstacle() {
  // 1. Berhenti
  stopRobot();
  delay(1000);

  // 2. Belok kiri menggunakan kelajuan khas elak
  avoidPivotLeft();
  delay(400);

  // 3. Berhenti
  stopRobot();
  delay(1000);

  // 4. Gerak lurus menggunakan kelajuan khas elak
  avoidMoveForward();
  delay(600);

  // 5. Berhenti 
  stopRobot();
  delay(1000);

  // 6. Belok kanan menggunakan kelajuan khas
  avoidPivotRight();
  delay(400);

  // 7. Berhenti
  stopRobot();
  delay(1000);

  // 8. Lurus 
  avoidMoveForward();
  delay(850);

  // 9. Berhenti 
  stopRobot();
  delay(1000);

  // 10. Belok kanan 
  avoidPivotRight();
  delay(400);

  // 11. Berhenti 
  stopRobot();
  delay(1000);

// 12. Lurus 
  avoidMoveForward();
  delay(400);

  // 13. Gerak lurus sehingga jumpa line (Guna kelajuan biasa atau elak terpulang)
  avoidMoveForward();
  while (true) {
    int L = digitalRead(IR_LEFT);
    int C = digitalRead(IR_CENTER);
    int R = digitalRead(IR_RIGHT);
    
    // Auto break bila salah satu sensor cecah line
    if (L == LINE_DETECTED || C == LINE_DETECTED || R == LINE_DETECTED) {
      break; 
    }
    delay(5);
  }

  // 13. Berhenti bila dah pijak line
  stopRobot();
  delay(200);

  // 14. PEMBETULAN ARAH (PENTING)
  avoidPivotLeft();
  delay(200); 

  turn_memory = 0; // Reset memori
  obstacleCount = 0;
}

// ==========================================
// 7. SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  pinMode(IR_LEFT, INPUT); pinMode(IR_CENTER, INPUT); pinMode(IR_RIGHT, INPUT);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  
  stopRobot();
  delay(2000); // Masa selamat sebelum mula bergerak
}

// ==========================================
// 8. MAIN LOOP
// ==========================================
void loop() {
  // --- A. SEMAK HALANGAN ---
  float distance = readDistanceCm();
  if (distance > 1.0 && distance <= OBSTACLE_CM) {
    obstacleCount++;
  } else {
    obstacleCount = 0;
  }

  if (obstacleCount >= OBSTACLE_CONFIRM) {
    avoidObstacle();
    return; // Mula loop baru lepas siap elak
  }

  // --- B. BACA IR SENSOR ---
  int L = digitalRead(IR_LEFT);
  int C = digitalRead(IR_CENTER);
  int R = digitalRead(IR_RIGHT);

  // --- C. UPDATE MEMORI PUSINGAN ---
  if (L == LINE_DETECTED && R == NO_LINE) turn_memory = 1; 
  if (R == LINE_DETECTED && L == NO_LINE) turn_memory = 2; 
  if (C == LINE_DETECTED) turn_memory = 0; 

  // --- D. LOGIK PERGERAKAN ---
  
  // 1. Terbabas terus (Guna memori)
  if (L == NO_LINE && C == NO_LINE && R == NO_LINE) {
    if (turn_memory == 1) sharpPivotLeft();
    else if (turn_memory == 2) sharpPivotRight();
    else stopRobot();
  }
  
  // 2. Tepat di tengah
  else if (L == NO_LINE && C == LINE_DETECTED && R == NO_LINE) {
    moveForward();
  }
  
  // 3. Senget sikit ke kanan
  else if (L == LINE_DETECTED && C == LINE_DETECTED && R == NO_LINE) {
    turnLeft();
  }
  // 4. Terlalu senget ke kanan
  else if (L == LINE_DETECTED && C == NO_LINE && R == NO_LINE) {
    sharpPivotLeft();
  }
  
  // 5. Senget sikit ke kiri
  else if (L == NO_LINE && C == LINE_DETECTED && R == LINE_DETECTED) {
    turnRight();
  }
  // 6. Terlalu senget ke kiri
  else if (L == NO_LINE && C == NO_LINE && R == LINE_DETECTED) {
    sharpPivotRight();
  }
  
  // 7. Simpang 4 / T-Junction
  else if (L == LINE_DETECTED && C == LINE_DETECTED && R == LINE_DETECTED) {
    moveForward(); 
  }

  delay(5);
}
