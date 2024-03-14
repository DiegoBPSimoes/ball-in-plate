#include <AccelStepper.h>

#define MOTOR_X_STEP_PIN 2
#define MOTOR_X_DIR_PIN 5
#define MOTOR_Y_STEP_PIN 3
#define MOTOR_Y_DIR_PIN 6

AccelStepper stepperX(AccelStepper::DRIVER, MOTOR_X_STEP_PIN, MOTOR_X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, MOTOR_Y_STEP_PIN, MOTOR_Y_DIR_PIN);

String lastCommandList = ""; // Armazena a última lista de comandos
String commandList = ""; // Para armazenar a lista de comandos recebida
bool processingList = false; // Flag para indicar que estamos processando uma lista de comandos
int repeatCount = 0; // Contador para repetição dos comandos

void setup() {
  Serial.begin(9600);

  stepperX.setMaxSpeed(500); // Velocidade máxima
  stepperX.setAcceleration(800); // Aceleração
  stepperY.setMaxSpeed(500); // Velocidade máxima
  stepperY.setAcceleration(800); // Aceleração
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.startsWith("list")) {
      lastCommandList = input.substring(4);
      commandList = lastCommandList;
      processingList = true;
    } else if (input.startsWith("rep")) {
      int numRepetitions = input.length() > 3 ? input.substring(3).toInt() : 1;
      if (numRepetitions > 0 && !lastCommandList.isEmpty()) {
        repeatCount = numRepetitions;
        commandList = lastCommandList;
        processingList = true;
      }
    } else {
      processCommand(input);
    }
  }

  if (processingList) {
    processListOrRepeat();
  }

  stepperX.run();
  stepperY.run();
}

void processListOrRepeat() {
  if (processingList && !stepperX.isRunning() && !stepperY.isRunning()) {
    if (commandList.length() > 0) {
      int nextComma = commandList.indexOf(',');
      String command;

      if (nextComma != -1) {
        command = commandList.substring(0, nextComma);
        commandList = commandList.substring(nextComma + 1);
      } else {
        command = commandList;
        commandList = "";
      }

      processCommand(command.trim());
    } else if (repeatCount > 0) {
      repeatCount--;
      if (repeatCount > 0) {
        commandList = lastCommandList;
      } else {
        processingList = false;
      }
    } else {
      processingList = false;
    }
  }
}

void processCommand(String command) {
  bool moveX = false, moveY = false;
  long newX = 0, newY = 0;

  if (command.startsWith("setax")) {
    stepperX.setAcceleration(command.substring(5).toInt());
  } else if (command.startsWith("setay")) {
    stepperY.setAcceleration(command.substring(5).toInt());
  } else if (command.startsWith("setvx")) {
    stepperX.setMaxSpeed(command.substring(5).toInt());
  } else if (command.startsWith("setvy")) {
    stepperY.setMaxSpeed(command.substring(5).toInt());
  } else if (command.startsWith("ix")) {
    newX = stepperX.currentPosition() + command.substring(2).toInt();
    stepperX.move(newX - stepperX.currentPosition());
  } else if (command.startsWith("iy")) {
    newY = stepperY.currentPosition() + command.substring(2).toInt();
    stepperY.move(newY - stepperY.currentPosition());
  } else if (command.startsWith("x")) {
    newX = command.substring(1).toInt();
    stepperX.moveTo(newX);
  } else if (command.startsWith("y")) {
    newY = command.substring(1).toInt();
    stepperY.moveTo(newY);
  } else if (command.startsWith("pos")) {
    Serial.print("Posição X: ");
    Serial.print(stepperX.currentPosition());
    Serial.print(", Posição Y: ");
    Serial.println(stepperY.currentPosition());
    Serial.print("Velocidade Máx X: ");
    Serial.print(stepperX.maxSpeed());
    Serial.print(", Aceleração X: ");
    Serial.println(stepperX.acceleration());
    Serial.print("Velocidade Máx Y: ");
    Serial.print(stepperY.maxSpeed());
    Serial.print(", Aceleração Y: ");
    Serial.println(stepperY.acceleration());
  }
}
