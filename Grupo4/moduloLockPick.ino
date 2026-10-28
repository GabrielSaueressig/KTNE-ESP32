#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Pinos fixos do hardware TTGO T-Display ST7789
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL    4  // Backlight
#define ADC_EN   14  // Alimentação da tela

#define LED_VERDE 26
#define LED_VERMELHO 25

const int POTENCIOMETRO_GAZUA_PIN = 32;
const int POTENCIOMETRO_DIREITO_PIN = 33;
const int POT_GAZUA_MAX_VALUE = 3530;


int anguloResposta = 0;

// Estado global para controlar o redesenho da tela
int g_anguloGazuaAnterior = -1;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

/**
 * Configura e inicializa a energia, backlight e interface visual basica do display TTGO.
 */
void setupDisplay() {
  // 1. Liga os pinos de energia e backlight
  pinMode(ADC_EN, OUTPUT);
  digitalWrite(ADC_EN, HIGH);
  
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  delay(100); // Aguarda estabilização da tensão

  // 2. Inicializa o painel ST7789 (135x240) em modo paisagem
  tft.init(135, 240);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // 3. Desenha a interface visual estatica inicial
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("LOCKPICK MODULE");

  tft.drawCircle(120, 80, 40, ST77XX_BLUE);
}

/**
 * Le o potenciometro da gazua, atualiza o display se houver mudanca e retorna o angulo em graus (0 a 180).
 */
int lerAnguloGazua(int pinPotenciometro) {
  long soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += analogRead(pinPotenciometro);
    delayMicroseconds(50);
  }
  int rawValue = soma / 10;

  int angulo = map(rawValue, 0, POT_GAZUA_MAX_VALUE, 0, 180);
  return constrain(angulo, 0, 180);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  pinMode(POTENCIOMETRO_GAZUA_PIN, INPUT);
  pinMode(POTENCIOMETRO_DIREITO_PIN, INPUT);

  anguloResposta = random(180);

  // analogReadResolution(12);

  // analogSetPinAttenuation(POTENCIOMETRO_GAZUA_PIN, ADC_11db);
  // analogSetPinAttenuation(POTENCIOMETRO_DIREITO_PIN, ADC_11db);
  // analogReference(DEFAULT);

  Serial.print("Angulo Resposta: ");
  Serial.println(anguloResposta);
  setupDisplay();

}

void loop() {


  int angulo = lerAnguloGazua(POTENCIOMETRO_GAZUA_PIN);
  int anguloDireito = lerAnguloGazua(POTENCIOMETRO_DIREITO_PIN);

  if((angulo >= anguloResposta - 10 && angulo <= anguloResposta + 10) && (anguloDireito > 150)){
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_VERDE, HIGH);
  }else if(anguloDireito >= 30 && !(angulo >= anguloResposta - 10 && angulo <= anguloResposta + 10)){
    digitalWrite(LED_VERMELHO, HIGH);
    angulo = 0;
    digitalWrite(LED_VERDE, LOW);
  }

  Serial.print("valor cru esquerdo: ");
  Serial.println(analogRead(POTENCIOMETRO_GAZUA_PIN));
  Serial.print("valor cru direito ");
  Serial.println(analogRead(POTENCIOMETRO_DIREITO_PIN));

  if (angulo != g_anguloGazuaAnterior) {
    tft.fillRect(10, 115, 220, 20, ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(1);
    tft.setCursor(10, 120);
    tft.print("Angulo: ");
    tft.print(angulo);
    g_anguloGazuaAnterior = angulo;
  }


  
  if(anguloDireito < 30){
    digitalWrite(LED_VERMELHO, LOW);
  }

  delay(20);
}