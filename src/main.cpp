#define BLYNK_TEMPLATE_ID "TMPL6j0CoiNFB"
#define BLYNK_TEMPLATE_NAME "WaterQuality"
#define BLYNK_AUTH_TOKEN "22tAljdGlbicf3Aj8o1E61uGz0a2XgSH"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Fuzzy.h>

#define ONEWIRE_BUS 4
#define TURBIDITY_PIN 33

char auth[] = "22tAljdGlbicf3Aj8o1E61uGz0a2XgSH";  // Ganti dengan token otentikasi Blynk Anda
char ssid[] = "DODYANA123 4G";   // Ganti dengan nama WiFi Anda
char pass[] = "DodY1118*";

OneWire oneWire(ONEWIRE_BUS); 
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

Fuzzy *fuzzy = new Fuzzy();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // Fuzzy Input for Suhu
  FuzzyInput *suhu = new FuzzyInput(1);
  FuzzySet *dingin = new FuzzySet(0, 8, 15, 23);
  FuzzySet *normal = new FuzzySet(18, 23, 28, 33);
  FuzzySet *panas = new FuzzySet(28, 38, 42, 50);
  suhu->addFuzzySet(dingin);
  suhu->addFuzzySet(normal);
  suhu->addFuzzySet(panas);
  fuzzy->addFuzzyInput(suhu);

  // Fuzzy Input for Ammonia
  FuzzyInput *kekeruhan = new FuzzyInput(2);
  FuzzySet *jernih = new FuzzySet(0, 0, 1, 2);
  FuzzySet *keruh = new FuzzySet(1, 3, 4, 5);
  kekeruhan->addFuzzySet(jernih);
  kekeruhan->addFuzzySet(keruh);
  fuzzy->addFuzzyInput(kekeruhan);


  // Fuzzy Output for PWM Blower
  FuzzyOutput *kualitasair = new FuzzyOutput(1);
  FuzzySet *jelek = new FuzzySet(0, 10, 20, 40);
  FuzzySet *cukup = new FuzzySet(20, 45, 55, 80);
  FuzzySet *bagus = new FuzzySet(60, 80, 90, 100);
  kualitasair->addFuzzySet(jelek);
  kualitasair->addFuzzySet(cukup);
  kualitasair->addFuzzySet(bagus);
  fuzzy->addFuzzyOutput(kualitasair);

  FuzzyRuleAntecedent *dingin_keruh = new FuzzyRuleAntecedent();
  dingin_keruh->joinWithAND(dingin, keruh);
  FuzzyRuleConsequent *rule1_output = new FuzzyRuleConsequent();
  rule1_output->addOutput(jelek);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, dingin_keruh, rule1_output);
  fuzzy->addFuzzyRule(fuzzyRule1);

  FuzzyRuleAntecedent *normal_keruh = new FuzzyRuleAntecedent();
  normal_keruh->joinWithAND(normal, keruh);
  FuzzyRuleConsequent *rule2_output = new FuzzyRuleConsequent();
  rule2_output->addOutput(cukup);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, normal_keruh, rule2_output);
  fuzzy->addFuzzyRule(fuzzyRule2);

  FuzzyRuleAntecedent *panas_keruh = new FuzzyRuleAntecedent();
  panas_keruh->joinWithAND(panas, keruh);
  FuzzyRuleConsequent *rule3_output = new FuzzyRuleConsequent();
  rule3_output->addOutput(jelek);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, panas_keruh, rule3_output);
  fuzzy->addFuzzyRule(fuzzyRule3);

    FuzzyRuleAntecedent *dingin_jernih = new FuzzyRuleAntecedent();
  dingin_jernih->joinWithAND(dingin, jernih);
  FuzzyRuleConsequent *rule4_output = new FuzzyRuleConsequent();
  rule1_output->addOutput(jelek);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, dingin_jernih, rule4_output);
  fuzzy->addFuzzyRule(fuzzyRule4);

  FuzzyRuleAntecedent *normal_jernih = new FuzzyRuleAntecedent();
  normal_jernih->joinWithAND(normal, jernih);
  FuzzyRuleConsequent *rule5_output = new FuzzyRuleConsequent();
  rule2_output->addOutput(cukup);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, normal_jernih, rule5_output);
  fuzzy->addFuzzyRule(fuzzyRule5);

  FuzzyRuleAntecedent *panas_jernih = new FuzzyRuleAntecedent();
  panas_jernih->joinWithAND(panas, jernih);
  FuzzyRuleConsequent *rule6_output = new FuzzyRuleConsequent();
  rule3_output->addOutput(jelek);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, panas_jernih, rule6_output);
  fuzzy->addFuzzyRule(fuzzyRule6);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Inisialisasi Blynk
  Blynk.begin(auth, ssid, pass);

  sensors.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");

  int turbidityValue = analogRead(TURBIDITY_PIN);
  int turbidity = map(turbidityValue, 0, 2000, 100, 0);
  Serial.print("Turbidity Asli: ");
  Serial.println(turbidityValue);
  Serial.print("Turbidity Mapping: ");
  Serial.println(turbidity);

  int inputSuhu = temperatureC;
  int inputKekeruhan = turbidity;

  Serial.println("\n\n\nEntrance: ");
  Serial.print("\tSuhu: ");
  Serial.println(inputSuhu);
  Serial.print("\tKekeruhan: ");
  Serial.println(inputKekeruhan);

  fuzzy->setInput(1, inputSuhu);
  fuzzy->setInput(2, inputKekeruhan);
  //fuzzy->fuzzify();
  //fuzzy->defuzzify(1);
  float kualitasair;

  Serial.println("Result: ");
  Serial.print("\tKualitas Air: ");
  Serial.println(kualitasair);

  delay(6000);

  Blynk.virtualWrite(V0, temperatureC);
  Blynk.virtualWrite(V1, turbidity);

}