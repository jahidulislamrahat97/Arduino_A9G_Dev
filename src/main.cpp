#include <Arduino.h>
#include <A9G.h>

HardwareSerial A9G(2);
GSM gsm(A9G, 1);

const int gsm_pin = 15;

unsigned long tic = millis();
int counter = 0;
char data[50] = "\0";

void execute_command(char *data);

void eventDispatch(A9G_Event_t *event)
{
  // Serial.println("********IN MY eventDispatch ******");
  // Serial.print("event->id:");
  // Serial.println(event->id);
  char buffer[50] = "\0";

  switch (event->id)
  {
  case EVENT_MQTTPUBLISH:
    Serial.print("Topic: ");
    Serial.println(event->topic);
    Serial.printf("**message: %s**\n", event->message);
    execute_command(event->message);
    break;

  case EVENT_NEW_SMS_RECEIVED:
    Serial.print("Number: ");
    Serial.println(event->number);
    Serial.print("Message: ");
    Serial.println(event->message);
    Serial.print("Date & Time: ");
    Serial.println(event->date_time);
    strcpy(buffer, event->message);
    gsm.DeleteMessage(1, ALL_MESSAGE);
    delay(100);
    execute_command(buffer);
    break;
  case EVENT_CSQ:
    Serial.print("CSQ rssi: ");
    Serial.println(event->param1);
    break;

  case EVENT_IMEI:
    Serial.print("IMEI: ");
    Serial.println(event->param2);
    break;

  case EVENT_CCID:
    Serial.print("CCID: ");
    Serial.println(event->param2);
    break;



  case EVENT_CME:
    Serial.print("CME ERROR Message:");
    gsm.errorPrintCME(event->error);
    break;

  case EVENT_CMS:
    Serial.print("CMS ERROR Message:");
    gsm.errorPrintCMS(event->error);
    break;

  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(gsm_pin, OUTPUT);
  digitalWrite(gsm_pin, HIGH);
  delay(4000);
  digitalWrite(gsm_pin, LOW);
  delay(2000);

  gsm.init(115200);
  gsm.EventDispatch(eventDispatch);

  Serial.println("A9G Test Begin !");

  // while (!gsm.bIsReady())
  // {
  //   delay(1000);
  // }

  if (gsm.waitForReady())
  {
    Serial.println("A9G Ready");
  }
  else
  {
    Serial.println("A9G Not Ready");
  }

  // gprs coonnection
  while (!gsm.AttachToGPRS())
  {
    delay(3000);
  }

  while (!gsm.SetAPN("IP", "internet"))
  {
    Serial.println("retry !");
    delay(3000);
  }

  while (!gsm.ActivatePDP())
  {
    delay(3000);
  }

  delay(3000);
  Serial.println("***********Read CSQ ***********");
  gsm.ReadCSQ();

  delay(3000);
  Serial.println("***********Read IMEI ***********");
  gsm.ReadIMEI();

    delay(3000);
  Serial.println("***********Read IMEI ***********");
  gsm.ReadCCID();




  // // SMS function
  // while (!gsm.ActivateTE())
  // {
  //   Serial.println("retry : ActivateTE !");
  //   delay(3000);
  // }

  // while (!gsm.SetFormatReading(1))
  // {
  //   Serial.println("retry : SetFormatReading !");
  //   delay(3000);
  // }

  // while (!gsm.SetMessageStorageUnit())
  // {
  //   Serial.println("retry : SetMessageStorageUnit !");
  //   delay(3000);
  // }

  // gsm.DeleteMessage(1, ALL_MESSAGE);

  // gsm.ReadMessage(16);
  // gsm.CheckMessageStorageUnit();

  // if (gsm.SendMessage("01687223094", "Hello from A9G"))
  // {
  //   Serial.println("SMS Send Success");
  // }
  // else
  // {
  //   Serial.println("SMS Send Failed");
  // }

  // mqtt connection
  // while (!gsm.DisconnectBroker())
  // {
  //   delay(1000);
  // }

  // while (!gsm.ConnectToBroker("broker.hivemq.com", 1883, "akjkkjckad", 120, 0))
  // // while (!gsm.ConnectToBroker("broker.hivemq.com", 1883))
  // {
  //   delay(1000);
  // }

  // while (!gsm.SubscribeToTopic("DMA/SUB", 1, 0))
  // {
  //   delay(1000);
  // }

  // // publish start message
  // if (gsm.PublishToTopic("DMA/PUB", "Started"))
  // {
  //   Serial.println("started message send success");
  // }
  // else
  // {
  //   Serial.println("started message send fail");
  // }
}

void loop()
{
  gsm.executeCallback();

  if (millis() - tic >= 4000)
  {
    counter++;
    sprintf(data, "Hello%d", counter);

    // if (gsm.SendMessage("01687223094", "Hello from A9G"))
    // {
    //   Serial.println("SMS Send Success");
    // }
    // else
    // {
    //   Serial.println("SMS Send Failed");
    // }
    // if (gsm.PublishToTopic("DMA/PUB", data))
    // {
    //   Serial.println("message send success");
    // }
    // else
    // {
    //   Serial.println("message send fail");
    // }
    tic = millis();
  }

  delay(15);
}

void execute_command(char *data)
{
  Serial.println("$$$$$$$$$$$ In Execute command  $$$$$$$$$");
  Serial.print("Command: ");
  Serial.println(data);
  Serial.print("Command len: ");
  Serial.println(strlen(data));

  if (!strcmp(data, "restart"))
  {
    Serial.println("Restarting");
    digitalWrite(gsm_pin, LOW);
    delay(4000);
    ESP.restart();
  }
}