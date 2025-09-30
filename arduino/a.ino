// ----- Pines -----
const byte PIN_SENSOR = 7;   // S del sensor (activo en LOW por INPUT_PULLUP)
const byte PIN_DATA   = 13;  // A del 74LS164 (DATA serie) - NO usar como LED
const byte PIN_CLK    = 8;   // CLK del 74LS164 (pin 8)

// ----- Tiempos -----
const unsigned long CLOCK_HALF_MS = 750;  // 1 Hz => 500 ms alto + 500 ms bajo
const unsigned long HIT_HOLD_MS   = 750;  // opcional: estirar "1" del sensor

// ----- Estado -----
unsigned long t_last_clk = 0, t_last_hit = 0;
bool clkState = LOW;
bool holdOne  = false;

void setup() {
  pinMode(PIN_SENSOR, INPUT_PULLUP);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLK,  OUTPUT);
  digitalWrite(PIN_CLK, LOW);
  digitalWrite(PIN_DATA, LOW);
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  // Lee sensor y decide si querés estirar el "1"
  int tap = digitalRead(PIN_SENSOR);     // LOW = golpe
  if (tap == LOW) { holdOne = true; t_last_hit = now; }
  if (holdOne && (now - t_last_hit >= HIT_HOLD_MS)) holdOne = false;

  // Mantener el dato estable mientras el reloj está en LOW
  // (Esto asegura setup/hold para el próximo flanco de subida)
  if (clkState == LOW) {
    bool dataBit = (tap == LOW) || holdOne;  // 1 si hay golpe (o estirado)
    digitalWrite(PIN_DATA, dataBit ? HIGH : LOW);
  }

  // Generar el reloj 1 Hz
  if (now - t_last_clk >= CLOCK_HALF_MS) {
    clkState = !clkState;
    digitalWrite(PIN_CLK, clkState);
    t_last_clk = now;
  }
}