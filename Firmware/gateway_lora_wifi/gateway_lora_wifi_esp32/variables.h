#define VERSION "1.0.2"
#define TIEMPO_CONEXION_WIFI 15000
#define INTENTOS_LORA_INI 3
#define RF95_FREQ 915.0
//-- Tiempo para verificación de red Wifi
#define T_VERIFICAR 60000
//-- Tiempo para encendido del led
#define T_VIVO 2000

// Distribución de pines
#define RFM95_RST 14
#define RFM95_CS 5
#define RFM95_INT 2
#define PIN_LED 26
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_BUTTON 33

//-- Posiciones de memoria fijas. 0-50 para SSID y de 50-100 para password.
//-- Posiciones usadas en librería wfm para grabar SSID y PASS
#define EE_SSID_50 0
#define EE_PASS_50 50

int year_, month_, day_, hour_, min_, sec_;

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//IPAddress servidorMqtt(159, 203, 136, 219);
char servidorMqtt[50] = "demo.daimob.co";
// const char servidorMqtt[50] = "";
char usuarioMqtt[50] = "proycacao2023";
char passMqtt[50] = "sGPs11279DosV3";
char topic[50] = "sena/sgps11279";
//int puertoMqtt = 18843;
int puertoMqtt = 13883;

unsigned long millisReset;
unsigned long wifiMillis;
unsigned long statMillis;

char* msg;
String data_enviar = "";
uint8_t len;

String elId;
bool msg_valido = false;
String sMsg = "";

String tiempo_actual = "";
const char* remote_host = "www.google.com";


String pagina = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<title>DAIMOB WiFi Configuración</title>"
                "<meta charset='UTF-8'>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<style>body{text-align:center;font-family:verdana}a:link,a:visited,a:active {text-decoration: none;font-weight: bold;}.input1{padding-left:5px;border:1px solid #39a900;height:25px;width:100%;border-radius:7px;margin:15px 0}.btn{border:0;border-radius:.3rem;background-color:#39a900;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;margin-top:30px}.container{text-align:left;display:inline-block;min-width:260px;padding:10px;margin:0 10%}.header{text-align:center}</style>"
                "<script>function c(l) {document.getElementById('s').value = l.getAttribute('data-ssid') || l.innerText || l.textContent;document.getElementById('p').value = '';};function f() { var x = document.getElementById('p'); x.type === 'password' ? x.type = 'text' : x.type = 'password'; }</script>"
                "</head>"
                "<body>"
                "<div class='header'>"
                "<h2>Configuración WIFI</h2>"
                // "<h3>Daimob</h3>"
                "<h3>SENA SGPS 11279</h3>"
                "<a class='escanear' href='escanear'>Encontrar redes</a>"
                "</div>"
                "<div class='container'>"
                "<form action='guardarConfig' method='get' novalidate>"
                "SSID:"
                "<input class='input1' autocorrect='off' autocapitalize='none' id='s' name='s' type='text' maxlength='45'>"
                "Password:"
                "<input class='input1' autocorrect='off' autocapitalize='none' id='p' name='p' type='password' maxlength='45'>"
                "<input type='checkbox' onclick='f()'> Mostrar contraseña"
                "<input style='margin-bottom: 15px;' class='btn' type='submit' value='GUARDAR' />"
                "</form>";

String paginafin = "</div>"
                   "</body>"
                   "</html>";

String mensaje = "";

String fpagina() {
  return pagina;
}
String fpaginafin() {
  return paginafin;
}
