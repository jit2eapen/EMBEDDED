#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ── CHANGE THESE ──────────────────────────────────────────
const char* WIFI_SSID     = "Pallikaparambil wifi";
const char* WIFI_PASSWORD = "shiji@123";
// ─────────────────────────────────────────────────────────

const int PWM_PIN = 5;    // D1
const int ZC_PIN  = 4;    // D2

WebServer server(80);
int brightness = 0;

// ── Zero Cross Interrupt ──────────────────────────────────
volatile int dimValue = 0;  // 0=full, 128=off

void IRAM_ATTR zeroCross() {
  if (dimValue == 0) {
    digitalWrite(PWM_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(PWM_PIN, LOW);
  } else if (dimValue < 128) {
    delayMicroseconds(dimValue * 60);
    digitalWrite(PWM_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(PWM_PIN, LOW);
  }
}

void applyBrightness(int pct) {
  pct = constrain(pct, 0, 100);
  if (pct == 0) {
    dimValue = 128;  // fully off
  } else {
    // Map to working range 0-83%
    dimValue = map(pct, 0, 100, 110, 25);
  }
  Serial.printf("[BULB] %d%% → dimValue %d\n", pct, dimValue);
}

// ── HTML Page ─────────────────────────────────────────────
const char HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html><html><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Bulb Controller</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:sans-serif;background:#0d0d1a;color:#eee;
     min-height:100vh;display:flex;align-items:center;justify-content:center;}
.card{background:#13132a;border:1px solid #1e1e3a;border-radius:24px;
      padding:40px 36px;width:340px;text-align:center;}
.bulb{font-size:72px;display:block;margin-bottom:8px;
      filter:brightness(0.3);transition:filter .4s;}
h1{font-size:20px;color:#9aaeff;margin-bottom:4px;}
.sub{font-size:12px;color:#444470;margin-bottom:28px;}
.val{font-size:60px;font-weight:800;color:#fff;line-height:1;}
.val span{font-size:20px;color:#444470;}
.lbl{font-size:11px;color:#333360;letter-spacing:2px;
     text-transform:uppercase;margin:4px 0 24px;}
input[type=range]{-webkit-appearance:none;width:100%;height:6px;
  border-radius:3px;outline:none;cursor:pointer;margin-bottom:18px;
  background:linear-gradient(to right,#5060ff 0%,#5060ff var(--p,0%),
  #1a1a3a var(--p,0%),#1a1a3a 100%);}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;
  width:24px;height:24px;border-radius:50%;background:#fff;
  box-shadow:0 0 0 3px #5060ff;cursor:pointer;}
.presets{display:flex;gap:8px;margin-bottom:12px;}
.pr{flex:1;padding:8px 4px;border-radius:8px;
    border:1px solid #1e1e3a;background:#0f0f22;
    font-size:12px;font-weight:600;color:#4a4a7a;cursor:pointer;}
.pr:hover{border-color:#5060ff;color:#9aaeff;}
.btns{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:18px;}
button{padding:12px;border-radius:10px;border:1px solid #1e1e3a;
       background:#1a1a30;color:#8a8aaa;font-size:13px;font-weight:600;cursor:pointer;}
.off{background:#200a0a;border-color:#4a1a1a;color:#ff6b6b;}
.on{background:#0a1030;border-color:#1a2a6a;color:#6a8aff;}
.status{background:#0a0a1a;border:1px solid #161630;border-radius:8px;
        padding:10px 14px;font-size:12px;color:#333360;
        display:flex;align-items:center;gap:8px;}
.dot{width:8px;height:8px;border-radius:50%;background:#ff4444;}
.dot.ok{background:#44ff88;}
</style></head><body>
<div class="card">
  <span class="bulb" id="bulb">&#128161;</span>
  <h1>Bulb Controller</h1>
  <p class="sub">ESP32 &middot; AC Dimmer &middot; Wi-Fi</p>
  <div class="val" id="val">__BR__<span>%</span></div>
  <div class="lbl">Brightness</div>
  <input type="range" id="sl" min="0" max="100"
         value="__BR__" oninput="onSlider(this.value)">
  <div class="presets">
    <div class="pr" onclick="set(25)">25%</div>
    <div class="pr" onclick="set(50)">50%</div>
    <div class="pr" onclick="set(75)">75%</div>
  </div>
  <div class="btns">
    <button class="off" onclick="set(0)">&#9216; OFF</button>
    <button class="on"  onclick="set(100)">FULL &#9728;</button>
  </div>
  <div class="status">
    <div class="dot ok" id="dot"></div>
    <span id="st">Connected</span>
  </div>
</div>
<script>
const sl=document.getElementById('sl');
const valEl=document.getElementById('val');
const bulb=document.getElementById('bulb');
const dot=document.getElementById('dot');
const st=document.getElementById('st');
let t=null;
function ui(v){
  valEl.innerHTML=v+'<span>%</span>';
  sl.style.setProperty('--p',v+'%');sl.value=v;
  bulb.style.filter=v>0?'brightness('+(0.3+v/100*1.5)+')':'brightness(0.25)';
}
function send(v){
  dot.className='dot';st.textContent='Setting...';
  fetch('/set?brightness='+v)
    .then(r=>{dot.className='dot ok';st.textContent='Brightness: '+v+'%';})
    .catch(()=>{dot.className='dot';st.textContent='Connection lost!';});
}
function onSlider(v){ui(v);clearTimeout(t);t=setTimeout(()=>send(v),80);}
function set(v){ui(v);send(v);}
ui(__BR__);
</script>
</body></html>
)rawhtml";

void handleRoot() {
  String page = String(HTML);
  page.replace("__BR__", String(brightness));
  server.send(200, "text/html", page);
}

void handleSet() {
  if (server.hasArg("brightness")) {
    brightness = server.arg("brightness").toInt();
    applyBrightness(brightness);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing param");
  }
}

void handleStatus() {
  String json = "{\"brightness\":" + String(brightness) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== ESP32 Bulb Controller ===");

  // Setup pins
  pinMode(PWM_PIN, OUTPUT);
  pinMode(ZC_PIN, INPUT_PULLUP);
  digitalWrite(PWM_PIN, LOW);

  // Attach zero cross interrupt
  attachInterrupt(digitalPinToInterrupt(ZC_PIN),
                  zeroCross, RISING);
  Serial.println("Dimmer ready");

  // Connect Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to %s", WIFI_SSID);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++tries > 40) {
      Serial.println("\nFailed! Restarting...");
      delay(3000);
      ESP.restart();
    }
  }

  Serial.println("\nConnected!");
  Serial.printf("Open browser: http://%s\n",
                WiFi.localIP().toString().c_str());

  server.on("/",       HTTP_GET, handleRoot);
  server.on("/set",    HTTP_GET, handleSet);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
  delay(2);
}