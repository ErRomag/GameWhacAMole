//Тестировалось на Arduino IDE 1.8.7
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER_PIN  3 // Пин с пищалкой (англ. «buzzer»)

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Задаем адрес и размерность дисплея.

int currentScore = 0; // Текущее количество набранных очков
int wrongCounter = 0;  // Количество допущенных ошибок

int currentLed = 0;  // Номер выбранного светодиода
int correctBtn = 0;  // Пин правильной кнопки
int wrongBtn1 = 0;   // Пин неправильной кнопки 1
int wrongBtn2 = 0;   // Пин неправильной кнопки 2
int wrongBtn3 = 0;   // Пин неправильной кнопки 3

int updateTime = 800; // Время обновления цикла loop
int gameTime  = 6770;  // Продолжительность одной игры. Число обозначает не секунды, а условные единицы. (6700 у.е. = 40 секундам)

int buttonState = 0;  // Состояния кнопки

int randomLed = 0;    // Результаты ГСЧ

int j = 1; // Счетчик для повышения скорости игры
int i = 0;

void setup()
{
  Serial.begin(9600);
  // Объявление используемых портов для пищалки
  pinMode(BUZZER_PIN, OUTPUT);

  // Объявление функции для корректной работы ГСЧ
  randomSeed(analogRead(1));    // Получаем значения с аналового пина А1, к которому ничего не подключено для того, чтобы каждый новый запуск получать новую ПСП

  // Объявление пинов кнопок, как входов
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);

  // Объявление пинов светодиодов, как выходов
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  downloadMenu();  // Запуск загрузочного меню
}

void loop()
{

  int randomLed = random(0, 4);  // Выбираем случайное число для включения светодиода

  // Проверяем какой выюради светодиод, присваиваем соответсвтующие значения пинов
  if (randomLed == 0) {  // Выбран зеленый

    currentLed = 11;
    correctBtn = 7;
    wrongBtn1 = 4;
    wrongBtn2 = 5;
    wrongBtn3 = 6;
  }
  if (randomLed == 1) {  // Выбран крансый

    currentLed = 10;
    correctBtn = 6;
    wrongBtn1 = 4;
    wrongBtn2 = 5;
    wrongBtn3 = 7;

  }
  if (randomLed == 2) {  // Выбран желтый

    currentLed = 9;
    correctBtn = 5;
    wrongBtn1 = 4;
    wrongBtn2 = 6;
    wrongBtn3 = 7;
  }
  if (randomLed == 3) {  // Выбран синий

    currentLed = 8;
    correctBtn = 4;
    wrongBtn1 = 5;
    wrongBtn2 = 6;
    wrongBtn3 = 7;
  }

  // Основной цикл игры
  while (gameTime >= 0) {  // Ждем, пока не истечет время игры
    buttonState = digitalRead(correctBtn);  // Считываем состояние пина выбранной кнопки
    digitalWrite(currentLed, HIGH); // Подаем питание на выбранные светодиод

    if (digitalRead(wrongBtn1) == HIGH ||  // Проверяем не нажаты ли неправильный кнопки
        digitalRead(wrongBtn2) == HIGH ||
        digitalRead(wrongBtn3) == HIGH )  {

      tone(BUZZER_PIN, 500, 30);  // Включаем пищалку на 50 миллисекунд

      wrongCounter++;  // Увеличиваем счетчик ошибок

      lcd.setCursor(7, 1);  // Выводим на экран
      lcd.print(wrongCounter);

      break;
    }
    if (buttonState == HIGH)  {  // Проверяем нажата ли правильная кнопка
      digitalWrite(currentLed, LOW);  // Снимаем питание со светодиода

      tone(BUZZER_PIN, 500, 30);  // Включаем пищалку на 50 миллисекунд

      currentScore++;  // Увеличиваем счетсик очков

      lcd.setCursor(6, 0);  // Выводим на экран
      lcd.print(currentScore);

      break;
    }
    if (gameTime == 0)  {  // Проверяем не закончилось ли время игры
      digitalWrite(currentLed, LOW);  // Снимаем питание со светодиода

      lcd.clear();  // Выводим на экран
      lcd.setCursor(5, 0);
      lcd.print("TIME`S");
      lcd.setCursor(6, 1);
      lcd.print("OVER");
      delay(1000);

      gameOver();  // Запускаем меню конца игры
    }
    gameTime--;  // Уменьшаем время игры
    Serial.println(gameTime);  // Костыль, без которого игра вылетает, если кто-то сможет его убрать, сообщите ))
  }

  // Чтобы сделать игру интереснее при достижении определнного количества очков время
  // обновления цикла уменьшается, то если светодиоды загораются чаще
  if (currentScore == j * 10) {
    updateTime -= 100;
    j++;
  }

  digitalWrite(currentLed, LOW);  // Снимаем питание со светодиода
  delay(updateTime);  // Задерживаем цикл

  if (wrongCounter > 2) {  // Проверяем количество неправильных нажатий
    lcd.clear();  // Выводим на экран
    lcd.setCursor(6, 0);
    lcd.print("MANY");
    lcd.setCursor(5, 1);
    lcd.print("WRONGS");
    delay(1000);

    gameOver();  // Запускаем меню конца игры
  }

} // to loop

// ----------------------------------------------------------------------------------------------------
// ------------------------------------ ОПИСАНИЕ ФУНКЦИЙ ----------------------------------------------
// ----------------------------------------------------------------------------------------------------

void downloadMenu() { // Функция загрузки заставочного экрана
  lcd.init();                            // Инициализация lcd
  lcd.backlight();                       // Включаем подсветку

  // Выводим текст названия игры (Catch me if you can)
  lcd.setCursor(4, 0);                   // Устанавливаем курсор в начало 1 строки
  lcd.print("WHACK A");
  lcd.setCursor(6, 1);                   // Устанавливаем курсор в начало 2 строки
  lcd.print("MOLE");               // Выводим текст
  delay(2000);                           // задержка перед выводом сообщений о подготовке к игре
  lcd.clear();

  // Выводим текст подготовки к игре (Get ready!, Three!, Two!, One!, Start!)
  lcd.setCursor(3, 0);
  lcd.print("GET READY!");
  delay(1000);
  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.print("THREE!");
  tone(BUZZER_PIN, 500, 50);  // Включаем пищалку на 50 миллисекунд
  delay(1000);
  lcd.clear();

  lcd.setCursor(6, 0);
  lcd.print("TWO!");
  tone(BUZZER_PIN, 500, 50);  // Включаем пищалку на 50 миллисекунд
  delay(1000);
  lcd.clear();

  lcd.setCursor(6, 0);
  lcd.print("ONE!");
  tone(BUZZER_PIN, 500, 50);  // Включаем пищалку на 50 миллисекунд
  delay(1000);
  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.print("START!");
  tone(BUZZER_PIN, 500, 1000);  // Включаем пищалку на 50 миллисекунд
  delay(1000);
  lcd.clear();

  //Выводим текст количества очков (Score: 0)
  lcd.setCursor(0, 0);
  lcd.print("SCORE:");
  lcd.setCursor(6, 0);
  lcd.print(currentScore);

  lcd.setCursor(0, 1);
  lcd.print("WRONGS:");
  lcd.setCursor(7, 1);
  lcd.print(wrongCounter);
}

void gameOver() {
  delay(1000);

  digitalWrite(8, HIGH);  // Снимаем питание со светодиода
  digitalWrite(9, HIGH);  // Снимаем питание со светодиода
  digitalWrite(10, HIGH);  // Снимаем питание со светодиода
  digitalWrite(11, HIGH);  // Снимаем питание со светодиода
  delay(500);
  digitalWrite(8, LOW);  // Снимаем питание со светодиода
  digitalWrite(9, LOW);  // Снимаем питание со светодиода
  digitalWrite(10, LOW);  // Снимаем питание со светодиода
  digitalWrite(11, LOW);  // Снимаем питание со светодиода

  tone(BUZZER_PIN, 400, 50);  // Включаем пищалку на 50 миллисекунд
  delay(200);
  tone(BUZZER_PIN, 400, 100);  // Включаем пищалку на 50 миллисекунд
  delay(200);
  tone(BUZZER_PIN, 400, 150);  // Включаем пищалку на 50 миллисекунд
  delay(200);
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("GAME");
  lcd.setCursor(6, 1);
  lcd.print("OVER");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SCORE:");
  lcd.setCursor(6, 0);   // Вывод текущего счета на экран
  lcd.print(currentScore);
  lcd.setCursor(0, 1);
  lcd.print("WRONGS:");
  lcd.setCursor(7, 1);
  lcd.print(wrongCounter);
  exit(0);
}
