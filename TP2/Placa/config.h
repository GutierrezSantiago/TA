// PIN
const int POT_PIN = 32;
const int GREEN_LED_PIN = 23;

// WiFi
const char * SSID = "";
const char * PASS = "";

// Telegram Bot
const String TOKEN = "";
const unsigned long MTBS = 1000;  // mean time between scan messages (Tiempo medio entre scans)

// Telegram Bot Commands
const String CMD_EXIT = "/exit";
const String CMD_READ_HUMIDITY = "/humidity";
const String CMD_READ_TEMPERATURE = "/temperature";
const String CMD_READ_DHT = "/dht";
const String CMD_READ_POTENTIOMETER = "/potentiometer";
const String CMD_TURN_ON_BLUE_LED = "/turnonblueled";
const String CMD_TURN_OFF_BLUE_LED = "/turnoffblueled";
const String CMD_TURN_ON_GREEN_LED = "/turnongreenled";
const String CMD_TURN_OFF_GREEN_LED = "/turnoffgreenled";
const String CMD_INCREASE_POWER_GREEN_LED = "/increasepowergreenled";
const String CMD_DECREASE_POWER_GREEN_LED = "/decreasepowergreenled";
const String CMD_PRINT_MESSAGE = "/printmessage";