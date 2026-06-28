/*
 * ============================================================
 *  ESP32 Wi-Fi Exhaust Fan Speed Controller
 *  Device  : AC 220V Exhaust Fan
 *  Power   : 25W
 *  Speed   : 1300 RPM (full)
 *  Module  : RobotDyn AC Dimmer 8A
 *  Board   : ESP32-WROOM
 *  IDE     : PlatformIO · Ubuntu 22.04
 *
 *  WIRING:
 *  Module PWM  ──► D5  (GPIO14)
 *  Module ZC   ──► D2  (GPIO4)
 *  Module VCC  ──► 3.3V
 *  Module GND  ──► GND
 *
 *  AC Side:
 *  AC IN L ──► 220V Live
 *  AC IN N ──► 220V Neutral
 *  LOAD L  ──► Fan Live
 *  LOAD N  ──► Fan Neutral
 * ============================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ── CHANGE THESE ──────────────────────────────────────────
const char* WIFI_SSID     = "AKROBOTICSLAB02 4797";
const char* WIFI_PASSWORD = "12345678";
// ─────────────────────────────────────────────────────────

const int PWM_PIN = 14;   // D5 = GPIO14
const int ZC_PIN  = 4;    // D2 = GPIO4

WebServer server(80);
int  fanSpeed = 0;
bool fanOn    = false;

volatile int dimValue = 128;

// ── RPM estimate for 25W 1300RPM fan ─────────────────────
int estimateRPM(int pct) {
  if (pct == 0)   return 0;
  if (pct < 30)   return 0;
  return map(pct, 30, 100, 390, 1300);
}

// ── Zero Cross Interrupt ──────────────────────────────────
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

// ── Apply speed to hardware ───────────────────────────────
void applySpeed(int pct) {
  pct = constrain(pct, 0, 100);
  if (pct == 0) {
    dimValue = 128;
  } else {
    int mapped = constrain(pct, 30, 100);
    dimValue = map(mapped, 30, 100, 105, 15);
  }
  Serial.printf("[FAN] Speed=%d%% | RPM~%d | dimValue=%d\n",
                pct, estimateRPM(pct), dimValue);
}

// ── HTML Page ─────────────────────────────────────────────
const char HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html><html><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Exhaust Fan Controller</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:sans-serif;background:#0d0d1a;color:#eee;
     min-height:100vh;display:flex;align-items:center;justify-content:center;}
.card{background:#13132a;border:1px solid #1e1e3a;border-radius:24px;
      padding:40px 36px;width:360px;text-align:center;}
.fan-icon{font-size:72px;display:block;margin-bottom:4px;
          transition:transform 0.1s linear;}
.rpm-badge{display:inline-block;background:#0a0a1a;
           border:1px solid #1e1e3a;border-radius:20px;
           padding:4px 14px;font-size:12px;color:#5a5a8a;
           margin-bottom:20px;font-family:monospace;}
.specs{font-size:11px;color:#2a2a5a;margin-bottom:20px;
       font-family:monospace;}
h1{font-size:20px;color:#9aaeff;margin-bottom:4px;}
.sub{font-size:12px;color:#444470;margin-bottom:24px;}
.val{font-size:56px;font-weight:800;color:#fff;line-height:1;}
.val span{font-size:18px;color:#444470;}
.lbl{font-size:11px;color:#333360;letter-spacing:2px;
     text-transform:uppercase;margin:4px 0 20px;}
input[type=range]{-webkit-appearance:none;width:100%;height:6px;
  border-radius:3px;outline:none;cursor:pointer;margin-bottom:16px;
  background:linear-gradient(to right,#5060ff 0%,#5060ff var(--p,0%),
  #1a1a3a var(--p,0%),#1a1a3a 100%);}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;
  width:24px;height:24px;border-radius:50%;background:#fff;
  box-shadow:0 0 0 3px #5060ff;cursor:pointer;}
.presets{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;
         gap:8px;margin-bottom:12px;}
.pr{padding:9px 4px;border-radius:8px;border:1px solid #1e1e3a;
    background:#0f0f22;font-size:11px;font-weight:600;
    color:#4a4a7a;cursor:pointer;}
.pr:hover,.pr.active{border-color:#5060ff;color:#9aaeff;}
.btns{display:grid;grid-template-columns:1fr 1fr;
      gap:10px;margin-bottom:16px;}
button{padding:12px;border-radius:10px;border:1px solid #1e1e3a;
       background:#1a1a30;color:#8a8aaa;
       font-size:13px;font-weight:600;cursor:pointer;}
.off{background:#200a0a;border-color:#4a1a1a;color:#ff6b6b;}
.full{background:#0a1030;border-color:#1a2a6a;color:#6a8aff;}
.status{background:#0a0a1a;border:1px solid #161630;
        border-radius:8px;padding:10px 14px;
        font-size:12px;color:#333360;
        display:flex;align-items:center;gap:8px;}
.dot{width:8px;height:8px;border-radius:50%;
     background:#ff4444;flex-shrink:0;}
.dot.ok{background:#44ff88;}
.dot.run{background:#ff9900;animation:pulse 1s infinite;}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}
</style></head><body>
<div class="card">
  <span class="fan-icon" id="fan">&#127744;</span>
  <div class="rpm-badge" id="rpm">0 RPM</div>
  <h1>Exhaust Fan</h1>
  <p class="sub">ESP32 &middot; AC 220V &middot; 25W</p>
  <p class="specs">Max: 1300 RPM &nbsp;|&nbsp; Min start: 30%</p>
  <div class="val" id="val">__SP__<span>%</span></div>
  <div class="lbl">Fan Speed</div>
  <input type="range" id="sl" min="0" max="100"
         value="__SP__" oninput="onSlider(this.value)">
  <div class="presets">
    <div class="pr" id="p0"  onclick="set(0)"  >OFF</div>
    <div class="pr" id="p1"  onclick="set(40)" >LOW<br><small>~540</small></div>
    <div class="pr" id="p2"  onclick="set(65)" >MED<br><small>~845</small></div>
    <div class="pr" id="p3"  onclick="set(100)">HIGH<br><small>1300</small></div>
  </div>
  <div class="btns">
    <button class="off"  onclick="set(0)"  >&#9216; OFF</button>
    <button class="full" onclick="set(100)">FULL &#9889;</button>
  </div>
  <div class="status">
    <div class="dot" id="dot"></div>
    <span id="st">Connected</span>
  </div>
</div>
<script>
const sl=document.getElementById('sl');
const val=document.getElementById('val');
const fan=document.getElementById('fan');
const dot=document.getElementById('dot');
const st=document.getElementById('st');
const rpm=document.getElementById('rpm');
let t=null,angle=0,spinTimer=null;

function estimateRPM(v){
  if(v==0||v<30)return 0;
  return Math.round(390+(v-30)/70*910);
}

function spinFan(speed){
  clearInterval(spinTimer);
  if(speed==0){fan.style.transform='rotate(0deg)';return;}
  const delay=Math.max(20,300-speed*2.5);
  spinTimer=setInterval(()=>{
    angle=(angle+15)%360;
    fan.style.transform='rotate('+angle+'deg)';
  },delay);
}

function updatePresets(v){
  ['p0','p1','p2','p3'].forEach(p=>
    document.getElementById(p).classList.remove('active'));
  if(v==0)       document.getElementById('p0').classList.add('active');
  else if(v<=50) document.getElementById('p1').classList.add('active');
  else if(v<=80) document.getElementById('p2').classList.add('active');
  else           document.getElementById('p3').classList.add('active');
}

function ui(v){
  v=parseInt(v);
  val.innerHTML=v+'<span>%</span>';
  sl.style.setProperty('--p',v+'%');
  sl.value=v;
  rpm.textContent=estimateRPM(v)+' RPM';
  spinFan(v);
  updatePresets(v);
}

function send(v){
  dot.className='dot'+(v>0?' run':'');
  st.textContent='Setting...';
  fetch('/set?speed='+v)
    .then(r=>{
      dot.className='dot'+(v>0?' ok':'');
      st.textContent=v>0
        ?'Running: '+v+'% (~'+estimateRPM(v)+' RPM)'
        :'Fan OFF';
    })
    .catch(()=>{
      dot.className='dot';
      st.textContent='Connection lost!';
    });
}

function onSlider(v){ui(v);clearTimeout(t);t=setTimeout(()=>send(v),80);}
function set(v){ui(v);send(v);}
ui(__SP__);
</script>
</body></html>
)rawhtml";

void handleRoot() {
  String page = String(HTML);
  page.replace("__SP__", String(fanSpeed));
  server.send(200, "text/html", page);
}

void handleSet() {
  if (server.hasArg("speed")) {
    fanSpeed = constrain(server.arg("speed").toInt(), 0, 100);
    fanOn    = (fanSpeed > 0);
    applySpeed(fanSpeed);
    server.send(200, "text/plain", "OK:" + String(fanSpeed));
  } else {
    server.send(400, "text/plain", "Missing speed");
  }
}

void handleStatus() {
  String json = "{\"speed\":" + String(fanSpeed) +
                ",\"on\":"   + (fanOn ? "true" : "false") +
                ",\"rpm\":"  + String(estimateRPM(fanSpeed)) +
                ",\"watts\":" + String(fanSpeed == 0 ? 0 : fanSpeed * 25 / 100) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== ESP32 Exhaust Fan Controller ===");
  Serial.println("Device : AC 220V 25W 1300RPM");
  Serial.printf("PWM    : GPIO%d (D5)\n", PWM_PIN);
  Serial.printf("ZC     : GPIO%d (D2)\n", ZC_PIN);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(ZC_PIN,  INPUT_PULLUP);
  digitalWrite(PWM_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(ZC_PIN),
                  zeroCross, RISING);
  Serial.println("Dimmer : READY");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Wi-Fi  : Connecting to %s", WIFI_SSID);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
    if (++tries > 40) {
      Serial.println("\nFailed! Restarting...");
      delay(3000); ESP.restart();
    }
  }

  Serial.println("\nWi-Fi  : CONNECTED!");
  Serial.printf("Open   : http://%s\n",
                WiFi.localIP().toString().c_str());

  server.on("/",       HTTP_GET, handleRoot);
  server.on("/set",    HTTP_GET, handleSet);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound([](){
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("Server : STARTED!");
  Serial.println("====================================\n");
}

void loop() {
  server.handleClient();
  delay(2);
}