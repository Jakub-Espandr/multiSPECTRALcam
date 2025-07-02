int consecutiveZeroCount = 0; // Global variable to track consecutive zeros
const int ZERO_TRIGGER_COUNT = 20; // Number of consecutive zeros required
bool triggeredPrinted = false; // To ensure 'triggered' is printed only once per zero sequence
unsigned long lightCount = 0;
unsigned long stopwatchStart;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(5, LOW);
  Serial.begin(115200); // Fast serial output
  stopwatchStart = millis();
}

void loop() {
  int value = analogRead(A3);
  Serial.print(value);
  Serial.print(" [");
  Serial.print(consecutiveZeroCount);
  Serial.println("]");

  if (value == 0) {
    consecutiveZeroCount++;
    if (consecutiveZeroCount == ZERO_TRIGGER_COUNT && !triggeredPrinted) {
      Serial.println("triggered");
      lightCount++;
      triggeredPrinted = true;
    }
  } else {
    if (triggeredPrinted) {
      Serial.print("Počet nul v sekvenci: ");
      Serial.println(consecutiveZeroCount);
    }
    consecutiveZeroCount = 0;
    triggeredPrinted = false;
  }
} 