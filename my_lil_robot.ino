#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>

// ---------- WIFI ----------
const char* ssid = "iphone";
const char* password = "12345678";

// ---------- SERVOS ----------
Servo footRight, footLeft, bodyRight, bodyLeft;
#define FOOT_RIGHT_PIN 16
#define FOOT_LEFT_PIN 17
#define BODY_RIGHT_PIN 18
#define BODY_LEFT_PIN 19

// ---------- WEB SERVER ----------
WebServer server(80);

// ---------- DANCE LOOP ----------
volatile bool danceLoop = false;
unsigned long lastDanceMillis = 0;
int danceState = 0;
const unsigned long danceInterval = 300;
unsigned long danceLoopStart = 0;

// ---------- MOVEMENT FUNCTIONS ----------
void setAllTo90() {
  footRight.write(90);
  footLeft.write(90);
  bodyRight.write(80);
  bodyLeft.write(90);
}

void moveForward() {
   footRight.write(120);
  footLeft.write(60);
  bodyRight.write(60);
  bodyLeft.write(120);
  delay(500);

  // Step 2: Move feet backward, body forward
  
}
 
void moveBackward() {
  footRight.write(60);
  footLeft.write(110);
  bodyRight.write(110);
  bodyLeft.write(60);
  delay(500);

 
}
void moveLeft() {
  footRight.write(120);
  footLeft.write(90);
  bodyRight.write(90);
  bodyLeft.write(70);
}

void moveRight() {
  footRight.write(90);
  footLeft.write(120);
  bodyRight.write(70);
  bodyLeft.write(90);
}

void stopMovement() {
  setAllTo90();
}

void oneLegStand() {
 footRight.write(20);
  footLeft.write(80);
    bodyRight.write(90);
  bodyLeft.write(90);
}

void danceOnce() {
  // Step 1: Lean right, lift left
  bodyRight.write(70);
  bodyLeft.write(110);
  footLeft.write(120);
  footRight.write(90);
  delay(300);
 footLeft.write(120);
  footRight.write(30);
  delay(300);
  footLeft.write(110);
  footRight.write(40);
  delay(300);
  footRight.write(90);
    delay(500);
  // Step 2: Lean left, lift right
  bodyRight.write(110);
  bodyLeft.write(70);
  footRight.write(120);
  footLeft.write(90);
  delay(300);

  // Step 3: Both forward
  bodyRight.write(90);
  bodyLeft.write(90);
  footRight.write(70);
  footLeft.write(70);
  delay(300);

  // Step 4: Both back
  footRight.write(110);
  footLeft.write(110);
  delay(300);

  // Step 5: Wiggle
  bodyRight.write(80);
  bodyLeft.write(100);
  delay(200);
  bodyRight.write(100);
  bodyLeft.write(80);
  delay(200);

  setAllTo90();
}

// ---------- HANDLE COMMAND ----------
void handleCommand() {
  String move = server.arg("move");
  String act  = server.arg("act");
  
  if (move == "forward") {
    if (act == "start") moveForward();
    else stopMovement();
  } else if (move == "backward") {
    if (act == "start") moveBackward();
    else stopMovement();
  } else if (move == "left") {
    if (act == "start") moveLeft();
    else stopMovement();
  } else if (move == "right") {
    if (act == "start") moveRight();
    else stopMovement();
  } else if (move == "stop") {
    danceLoop = false;
    stopMovement();
  } else if (move == "oneleg") {
    oneLegStand();
  } else if (move == "dance") {
    danceOnce();
  } else if (move == "dance_loop") {
    if (act == "start") {
      danceLoop = true;
      lastDanceMillis = millis();
      danceLoopStart = millis();
      danceState = 0;
    } else {
      danceLoop = false;
      setAllTo90();
    }
  }

  server.send(200, "text/plain", "OK");
}

// ---------- WEB PAGE ----------
const char* html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<title>Robot Controller</title>
<style>
  :root{
    --bg1:#001f3f;
    --bg2:#ff66b2;
    --box1:#1abc9c;
    --box2:#ff7675;
    --box3:#ffd166;
    --box4:#7f8c8d;
    --text:#ffffff;
  }
  html,body{
    height:100%;
    margin:0;
    font-family:Arial, Helvetica, sans-serif;
  }
  body{
    background: linear-gradient(160deg, var(--bg1) 0%, #00264d 40%, var(--bg2) 100%);
    color:var(--text);
    display:flex;
    flex-direction:column;
    align-items:center;
    justify-content:flex-start;
  }
  header{
    width:100%;
    padding:18px 12px;
    text-align:center;
    box-shadow: 0 3px 8px rgba(0,0,0,0.2);
  }
  h1{ margin:0; font-size:20px; }
  .grid{
    display:grid;
    grid-template-columns: repeat(2, minmax(140px, 1fr));
    gap:14px;
    width:92%;
    max-width:460px;
    margin-top:28px;
  }
  .control{
    background:rgba(255,255,255,0.08);
    border-radius:14px;
    padding:18px 12px;
    display:flex;
    align-items:center;
    justify-content:center;
    font-size:18px;
    font-weight:600;
    color:var(--text);
    box-shadow: 0 6px 18px rgba(0,0,0,0.25);
  }
  .big{ grid-column: 1 / -1; display:flex; gap:12px; justify-content:center; }
  .btn{
    width:100%; padding:14px 10px; border-radius:10px;
    border:none; color:#fff; font-size:18px; font-weight:700; cursor:pointer;
  }
  .btn:active { transform: translateY(1px); }
  .fwd { background: linear-gradient(90deg,var(--box1), #16a085); }
  .bwd { background: linear-gradient(90deg,var(--box2), #e74c3c); }
  .left { background: linear-gradient(90deg,var(--box3), #f39c12); color:#222; }
  .right{ background: linear-gradient(90deg,var(--box4), #95a5a6); }
  .action { background: linear-gradient(90deg,#5b2c6f, #9b59b6); width:48%; }
  .stop { background: linear-gradient(90deg,#2c3e50,#34495e); width:100%; }
  .small-note{ text-align:center; font-size:12px; opacity:0.9; margin-top:12px;}
</style>
</head>
<body>
<header><h1>🤖 Robot Controller</h1></header>
<div class="grid">
  <div class="control"><button class="btn fwd"
    onmousedown="start('forward')" onmouseup="stop('forward')"
    ontouchstart="start('forward')" ontouchend="stop('forward')">Forward</button></div>

  <div class="control"><button class="btn bwd"
    onmousedown="start('backward')" onmouseup="stop('backward')"
    ontouchstart="start('backward')" ontouchend="stop('backward')">Backward</button></div>

  <div class="control"><button class="btn left"
    onmousedown="start('left')" onmouseup="stop('left')"
    ontouchstart="start('left')" ontouchend="stop('left')">Left</button></div>

  <div class="control"><button class="btn right"
    onmousedown="start('right')" onmouseup="stop('right')"
    ontouchstart="start('right')" ontouchend="stop('right')">Right</button></div>

  <div class="control big">
    <button class="btn action" onclick="clickCmd('dance')">Dance ✨</button>
    <button class="btn action"
      onmousedown="start('dance_loop')" onmouseup="stop('dance_loop')"
      ontouchstart="start('dance_loop')" ontouchend="stop('dance_loop')">Dance (hold)</button>
  </div>

  <div class="control"><button class="btn" onclick="clickCmd('oneleg')"> Pose </button></div>
  <div class="control"><button class="btn stop" onclick="clickCmd('stop')">⏹️ STOP</button></div>
</div>
<div class="small-note">Hold to move; release to stop. Dance hold stops after 5 sec.</div>
<script>
  function send(path){ fetch(path).catch(e=>console.log(e)); }
  function start(move){ send("/cmd?move="+move+"&act=start"); }
  function stop(move){ send("/cmd?move="+move+"&act=stop"); }
  function clickCmd(move){ send("/cmd?move="+move); }
</script>
</body>
</html>
)rawliteral";

// ---------- SERVER ----------
void handleRoot() { server.send(200, "text/html", html); }

void setup() {
  Serial.begin(115200);
  delay(200);

  footRight.attach(FOOT_RIGHT_PIN);
  footLeft.attach(FOOT_LEFT_PIN);
  bodyRight.attach(BODY_RIGHT_PIN);
  bodyLeft.attach(BODY_LEFT_PIN);

  setAllTo90();
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Hotspot");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 60) {
    delay(300);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to Hotspot");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to Hotspot.");
  }

  server.on("/", handleRoot);
  server.on("/cmd", handleCommand);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (danceLoop) {
    unsigned long now = millis();
    if (now - danceLoopStart > 5000) { // auto stop after 5 sec
      danceLoop = false;
      setAllTo90();
    } else if (now - lastDanceMillis >= danceInterval) {
      lastDanceMillis = now;
      if (danceState == 0) {
        bodyRight.write(70);
        bodyLeft.write(110);
        footRight.write(120);
        footLeft.write(90);
        danceState = 1;
      } else if (danceState == 1) {
        bodyRight.write(110);
        bodyLeft.write(70);
        footRight.write(90);
        footLeft.write(120);
        danceState = 2;
      } else {
        bodyRight.write(90);
        bodyLeft.write(90);
        setAllTo90();
        danceState = 0;
      }
    }
  }
}
