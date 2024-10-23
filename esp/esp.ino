#include <ESP8266WiFi.h>

// Налаштування Wi-Fi
const char* ssid = "123";             // Ваш SSID Wi-Fi
const char* password = "12345678";     // Пароль Wi-Fi

// Номер порту для веб-сервера
WiFiServer server(80);

// Змінна для зберігання HTTP-запиту
String header;

// Змінна для стану GPIO пінів
String output5State = "off";
String output4State = "off";

// Призначення GPIO пінів
const int output5 = 5;
const int output4 = 4;
const int buttonPin = 16;  // Пін для кнопки

// Змінні для таймера
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;  // Таймаут у мілісекундах

// Змінна для зберігання стану кнопки
String buttonState = "NOT PRESSED";

void setup() {
  // Ініціалізація серійного порту
  Serial.begin(115200);

  // Налаштовуємо GPIO як виходи та вхід
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Кнопка з підтягувальним резистором

  // Встановлюємо початковий стан GPIO
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);

  // Підключення до Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Виводимо IP-адресу та починаємо сервер
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // Зчитуємо стан кнопки
  if (digitalRead(buttonPin) == HIGH) {
    if (buttonState != "PRESSED") {
      Serial.println("Button pressed!");  // Виводимо в Serial Monitor
    }
    buttonState = "PRESSED";
  } else {
    if (buttonState != "NOT PRESSED") {
      Serial.println("Button released!");  // Виводимо в Serial Monitor
    }
    buttonState = "NOT PRESSED";
  }

  // Перевіряємо підключення клієнта
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";  // Буфер для зчитування рядка запиту
    currentTime = millis();
    previousTime = currentTime;

    // Чекаємо запит від клієнта
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);  // Виводимо запит у Serial Monitor
        header += c;

        // Перевіряємо, чи завершився HTTP-запит
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Надсилаємо HTTP-відповідь
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Відображаємо веб-сторінку
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            
            // Додаємо автооновлення сторінки кожні 5 секунд
            client.println("<meta http-equiv=\"refresh\" content=\"1\">");
            
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            client.println("<body><h1>ESP8266 Web Server</h1>");

            // Відображення стану GPIO 5
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            if (output5State == "off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Відображення стану GPIO 4
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            if (output4State == "off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Відображення стану кнопки
            client.println("<p>Button State: " + buttonState + "</p>");

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Очищаємо заголовок і закриваємо з'єднання
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}