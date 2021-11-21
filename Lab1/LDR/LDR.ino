int LDR_pin = 36;
int val = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  val = analogRead(LDR_pin);
  Serial.print("value of LDR(Resistance): ");
  Serial.println(val);
  delay(1000);
}
