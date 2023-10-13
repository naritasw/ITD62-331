float a = 0.0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  Serial.printf("%f %f %d\n",sin(a),cos(a),0);
  a = a + 0.1;
  delay(100);
  }
