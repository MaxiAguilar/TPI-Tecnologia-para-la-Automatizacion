// Salidas digitales de los sensores IR
const int sensorIzq = 2;
const int sensorCen = 3;
const int sensorDer = 4;
// Driver L298N
const int IN1 = 7;   // Dirección motor derecho (IN1)
const int IN2 = 8;   // Dirección motor derecho (IN2)
const int IN3 = 9;   // Dirección motor izquierdo (IN3)
const int IN4 = 10;  // Dirección motor izquierdo (IN4)
const int ENA = 6;   // PWM velocidad motor derecho (ENA)
const int ENB = 5;   // PWM velocidad motor izquierdo (ENB)

// Velocidad base (0–255)
const int speedBase = 200;
int error = 0;
int lastError = 0;

void setup() {
  //Sensores
  pinMode(sensorIzq, INPUT);
  pinMode(sensorCen, INPUT);
  pinMode(sensorDer, INPUT);
  //Motores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  Serial.begin(9600);
}
// Función para avanzar
void avanzar(int velocidad) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
}

void avanzarProporcional(int velDer, int velIzq) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, velDer);
  analogWrite(ENB, velIzq);
}

// Función para detener
void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void loop() {
  // Leer sensores
  int lecturaIzq = digitalRead(sensorIzq);
  int lecturaCen = digitalRead(sensorCen);
  int lecturaDer = digitalRead(sensorDer);

 // Calcular error
  error = 0;
  if (lecturaIzq == 1 && lecturaCen == 1) {
    error = -1;
  } else if (lecturaIzq == 1 && lecturaCen == 0) {
    error = -2;
  } else if (lecturaDer == 1 && lecturaCen == 1) {
    error = 1;
  } else if (lecturaDer == 1 && lecturaCen == 0) {
    error = 2;
  } // Si solo central detecta, error = 0

  // Calcular derivada
  int derivative = error - lastError;

  // PID solo con P y D
  float Kp = 60;   // Ajustable (30-60)
  float Kd = 10;   // Ajustable (10-30)
  int correction = (Kp * error) + (Kd * derivative);

  // Velocidad base
  int baseSpeed = 190;  // Ajustar según pista y motor

  // Calcular velocidades individuales con corrección
  int leftSpeed = constrain(baseSpeed + correction, 0, 255);
  int rightSpeed = constrain(baseSpeed - correction, 0, 255);

  // Depuración
  Serial.print("Error: ");
  Serial.print(error);
  Serial.print(" | Deriv: ");
  Serial.print(derivative);
  Serial.print(" | Corr: ");
  Serial.print(correction);
  Serial.print(" | LSpeed: ");
  Serial.print(leftSpeed);
  Serial.print(" | RSpeed: ");
  Serial.println(rightSpeed);

  // Control de movimiento
  if (lecturaIzq == 0 && lecturaCen == 0 && lecturaDer == 0) {
    detener();
  } else {
    avanzarProporcional(rightSpeed, leftSpeed);
  }

  // Actualizar lastError
  lastError = error;

  // Pequeño retardo para estabilidad
  delay(50);
}
