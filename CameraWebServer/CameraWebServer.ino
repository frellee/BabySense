#define BLYNK_TEMPLATE_ID "TMPL6kKxKqlVO"
#define BLYNK_TEMPLATE_NAME "Baby Crying Notification"
#define BLYNK_PRINT Serial

#include "esp_camera.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Sound sensor pin
const int soundSensorPin = 15;

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "MCM DOMAIN";
const char *password = "VivaMCM!";

// ===========================
// Blynk authentication token
// ===========================
char auth[] = "TVDAONpgNKZz33yrspazvfzyFUAzRCjj";

void startCameraServer();
void setupLedFlash(int pin);
void setupSoundSensor(int pin);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 initialized");

  // Initialize Blynk
  Blynk.begin(auth, ssid, password);

  // Setup sound sensor
  setupSoundSensor(soundSensorPin);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        
    s->set_brightness(s, 1);   
    s->set_saturation(s, -2);  
  }
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

  #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
  #endif

  #if defined(CAMERA_MODEL_ESP32S3_EYE)
    s->set_vflip(s, 1);
  #endif

  // Setup LED FLash if LED pin is defined in camera_pins.h
  #if defined(LED_GPIO_NUM)
    setupLedFlash(LED_GPIO_NUM);
  #endif

  // WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // Read from sound sensor (digital mode)
  int soundValue = digitalRead(soundSensorPin);
  Serial.print("Sound sensor value: ");
  Serial.println(soundValue);

  if (soundValue == HIGH) {
    Serial.println("Baby detected!");
    // Send notification to Blynk
    Blynk.virtualWrite(V0, "baby_is_crying");
  }

  Blynk.run();
  delay(2000); // Adjust delay as needed
}

// Function to setup sound sensor pin
void setupSoundSensor(int pin) {
  pinMode(soundSensorPin, INPUT);
}
