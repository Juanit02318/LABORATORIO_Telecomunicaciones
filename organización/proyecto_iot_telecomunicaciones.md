# Plataforma IoT para Monitoreo Biomédico

> **Laboratorio de Software en Telecomunicaciones — UNSA**  
> Sistema IoT end-to-end para el monitoreo de frecuencia cardíaca (FC) y saturación de oxígeno (SpO₂) mediante ESP32, MAX30102, Wi-Fi, HTTP y MQTT.

**UNIVERSIDAD NACIONAL DE SAN AGUSTÍN DE AREQUIPA**
**FACULTAD DE INGENIERIA DE PRODUCCION Y SERVICIOS**
**ESCUELA DE PROFESIONAL DE INGENIERÍA EN TELECOMUNICACIONES**


> <img width="250" height="307" alt="image" src="https://github.com/user-attachments/assets/51856c7b-7124-4fa4-a163-0e5dd3ff669d" />


**Curso: **
Laboratorio de Software en Telecomunicaciones

**Docente: **
Ing. Mayhua Lopez, Efrain Tito

**Alumnos:**
Arana Puma, Pedro Jesus
Barriga Callo Juan Diego
Butron Benites, Ricardo Alonso
Castillo Huayna, Gael Gandhi
Quea Carlo Kevin Baltazar


**Año y Sección:**
4to año “A”

**Arequipa – Perú**
**2026**


**ÍNDICE**

[**1. Requisitos y Arquitectura	3**](#1-requisitos-y-arquitectura	3)
[1.1. Problemática y contexto](#11-problemática-y-contexto)
[1.2. Stakeholders y necesidades](#12-stakeholders-y-necesidades)
[1.3. Requisitos funcionales y no funcionales](#13-requisitos-funcionales-y-no-funcionales)
[1.4. Criterios de aceptación y matriz de requisitos](#14-criterios-de-aceptación-y-matriz-de-requisitos)
[1.5. Arquitectura del sistema](#15-arquitectura-del-sistema)
[**2. Firmware y Modularidad	4**](#2-firmware-y-modularidad	4)
[2.1. Hardware y herramientas utilizadas](#21-hardware-y-herramientas-utilizadas)
[2.2. Arquitectura y estructura del firmware](#22-arquitectura-y-estructura-del-firmware)
[2.3. Implementación y evidencias de funcionamiento](#23-implementación-y-evidencias-de-funcionamiento)
[**3. Comunicación HTTP/MQTT y Contrato de Datos	5**](#3-comunicación-httpmqtt-y-contrato-de-datos	5)
[3.1. Estructura y formato de los datos](#31-estructura-y-formato-de-los-datos)
[3.2. Comunicación HTTP y MQTT](#32-comunicación-http-y-mqtt)
[3.3. Comparación y selección del protocolo](#33-comparación-y-selección-del-protocolo)
[4.3. Resultados y análisis](#43-resultados-y-análisis)
[4.4. Conclusiones de la experimentación](#44-conclusiones-de-la-experimentación)
[**5. Git, Documentación y ADR	6**](#5-git-documentación-y-adr	6)
[5.1. Repositorio y organización del proyecto](#51-repositorio-y-organización-del-proyecto)
[5.2. Historial de commits y evidencias](#52-historial-de-commits-y-evidencias)
[5.3. Documentación del proyecto](#53-documentación-del-proyecto)
[5.4. ADR-01: Selección del protocolo HTTP/MQTT](#54-adr-01-selección-del-protocolo-httpmqtt)
[**6. Validación y Conclusiones	7**](#6-validación-y-conclusiones	7)
[6.1. Validación de requisitos](#61-validación-de-requisitos)
[6.2. Cumplimiento de los objetivos](#62-cumplimiento-de-los-objetivos)
[6.3. Conclusiones](#63-conclusiones)
[6.4. Limitaciones y trabajos futuros](#64-limitaciones-y-trabajos-futuros)
[**7. Referencias Bibliográficas	7**](#7-referencias-bibliográficas	7)
[**8. Anexos	7**](#8-anexos	7)
[8.1. Código fuente](#81-código-fuente)
[8.2. Evidencias y capturas](#82-evidencias-y-capturas)
[8.3. Datos experimentales](#83-datos-experimentales)
[8.4. Evidencias de Git](#84-evidencias-de-git)


# 1. Requisitos y Arquitectura

## 1.1. Problemática y contexto

**Contexto del proyecto**
El proyecto plantea el diseño de una plataforma IoT end-to-end para el monitoreo biomédico continuo, abarcando desde la adquisición física de señales biomédicas en un terminal wearable hasta su procesamiento en servidores Edge/Backend y su visualización en una interfaz operativa en tiempo real.

La solución está orientada al monitoreo de frecuencia cardíaca (FC) y saturación de oxígeno (SpO₂) de un usuario o paciente mediante un dispositivo basado en ESP32 y un sensor biomédico MAX30102. El dispositivo se conecta mediante una red inalámbrica Wi-Fi y transmite las mediciones hacia un sistema de procesamiento mediante los protocolos HTTP y MQTT.

En telecomunicaciones, el correcto funcionamiento lógico de un sistema no es suficiente. También es necesario considerar aspectos como latencia, disponibilidad, pérdida de paquetes, recuperación ante fallas de conectividad, seguridad, escalabilidad y trazabilidad de los datos.

**Formulación del problema**
El problema se centra en las pérdidas de conectividad Wi-Fi durante el monitoreo de variables biomédicas. Las interrupciones del enlace pueden ocasionar pérdida de paquetes, ausencia temporal de información y falta de trazabilidad de las mediciones.

| **Mala formulación**                                                                     | **Buena formulación**                                                                                                                                                                                                                     |
| ---------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| El sistema Wi-Fi falla a veces y los médicos no ven los datos de pulso de los pacientes. | Las pérdidas imprevistas de cobertura Wi-Fi provocan una tasa de pérdida de paquetes superior al 12 % en tramas biomédicas críticas, ocasionando vacíos de monitoreo mayores a 45 segundos sin mecanismos de trazabilidad o buffer local. |

**Problemática real**
El personal de salud y el usuario necesita conocer en tiempo casi real las métricas biomédicas, específicamente frecuencia cardíaca y SpO₂, de forma inalámbrica. Actualmente, las fallas y caídas de conectividad pueden provocar pérdida de datos clínicos sin trazabilidad, dificultando la detección temprana de anomalías fisiológicas.

El sistema propuesto busca resolver esta problemática mediante un terminal IoT capaz de adquirir, validar, identificar y transmitir las mediciones, incorporando mecanismos de reconexión y almacenamiento temporal ante interrupciones de red.

**Objetivo general**
Diseñar e implementar un sistema IoT para el monitoreo continuo de frecuencia cardíaca y saturación de oxígeno mediante un nodo ESP32, utilizando comunicación inalámbrica y mecanismos de recuperación ante fallas de conectividad.

**Objetivos específicos**

- Comprender e implementar metodologías de ingeniería de requisitos orientadas a entornos de telecomunicaciones e IoT.
- Diseñar una arquitectura distribuida de cuatro capas con capacidad de almacenamiento local y sincronización diferida.
- Implementar un firmware modular para adquisición, validación, construcción y transmisión de datos biomédicos.
- Implementar mecanismos de comunicación mediante HTTP y MQTT.
- Validar experimentalmente el impacto de la latencia, RSSI, pérdida de paquetes y tiempo de recuperación.
- Comparar el desempeño de HTTP y MQTT para determinar el protocolo más adecuado para la transmisión continua de telemetría.

**Flujo metodológico**

**image**

## 1.2. Stakeholders y necesidades

**Usuarios participantes**
Se identificaron los principales usuarios y participantes involucrados en el funcionamiento del sistema.


| **ID** | **Stakeholder**                | **Necesidad**                                                                                                     |
| ------ | ------------------------------ | ----------------------------------------------------------------------------------------------------------------- |
| HU-01  | Enfermero / Personal médico    | Visualizar en tiempo real la frecuencia cardíaca y SpO₂ de los pacientes y recibir alertas ante valores anómalos. |
| HU-02  | Paciente / Usuario wearable    | Transmitir su estado de salud y preservar las mediciones durante cortes temporales de Wi-Fi.                      |
| HU-03  | Administrador de red / Soporte | Disponer de telemetría de red como RSSI, secuencia y uptime para diagnosticar problemas de conectividad.          |

**Identificación de stakeholders**

| **Elemento** | **Pregunta clave**              | **Respuesta**                                                                                                                              |
| ------------ | ------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| Contexto     | ¿Dónde ocurre el problema?      | En el monitoreo continuo de un usuario/paciente mediante un dispositivo wearable conectado a una red Wi-Fi.                                |
| Stakeholder  | ¿Quién sufre o usa la solución? | El usuario/paciente y el personal médico o plataforma de telemedicina.                                                                     |
| Necesidad    | ¿Qué capacidad falta?           | Un terminal IoT que entregue telemetría estructurada, valide los datos, mantenga trazabilidad temporal y permita recuperación ante cortes. |
| Dolor        | ¿Qué limitación existe hoy?     | Las desconexiones Wi-Fi pueden provocar pérdida de paquetes y falta de visibilidad del estado del terminal.                                |
| Impacto      | ¿Cómo se mide?                  | Mediante tasa de entrega, tiempo de recuperación, latencia y porcentaje de datos perdidos.                                                 |


**Roles de los stakeholders**

| **Pregunta**                         | **Stakeholder**             |
| ------------------------------------ | --------------------------- |
| ¿Quién opera el dispositivo?         | Usuario / Paciente          |
| ¿Quién administra los accesos?       | Administrador del sistema   |
| ¿Quién recibe las alertas?           | Personal médico / Enfermero |
| ¿Quién configura la infraestructura? | Administrador de red        |
| ¿Quién responde ante fallas?         | Soporte y mantenimiento     |
| ¿Quién audita la seguridad?          | Seguridad / Auditoría IT    |

**Mapeo de interés e influencia**

| **Stakeholder**             | **Nivel de interés** | **Nivel de influencia** |
| --------------------------- | -------------------- | ----------------------- |
| Personal Médico / Enfermero | Alto                 | Alto                    |
| Usuario / Paciente          | Alto                 | Bajo                    |
| Administrador del Sistema   | Medio                | Alto                    |
| Soporte y Mantenimiento     | Medio                | Medio                   |

**Del stakeholder al requisito**

| **Stakeholder** | **Necesidad**                                | **Requisito**                                           | **Criterio de aceptación**                                     |
| --------------- | -------------------------------------------- | ------------------------------------------------------- | -------------------------------------------------------------- |
| Enfermero       | Monitoreo de signos cardíacos en tiempo real | El sistema debe monitorear los signos de los pacientes. | El sistema debe generar una alerta en caso de signos anómalos. |


## 1.3. Requisitos funcionales y no funcionales

**Requisitos funcionales**

| **ID** | **Requisito**                 |
| ------ | ----------------------------- |
| RF-01  | Registrar terminales IoT.     |
| RF-02  | Recibir telemetría biomédica. |
| RF-03  | Almacenar mediciones.         |
| RF-04  | Generar alertas fisiológicas. |

**RF-01: Registrar terminales**
El sistema debe permitir identificar de manera única cada dispositivo IoT/wearable conectado a la plataforma.

**RF-02: Recibir telemetría biomédica**
El sistema debe recibir tramas estructuradas que contengan las métricas de frecuencia cardíaca y SpO₂, junto con información de identificación y tiempo.

**RF-03: Almacenar mediciones**
El sistema debe permitir almacenar las mediciones recibidas conservando su trazabilidad temporal.

**RF-04: Generar alertas fisiológicas**
El sistema debe generar una notificación cuando las mediciones recibidas se encuentren fuera de los rangos establecidos.

**Requisitos no funcionales**

| **ID** | **Requisito**                                            |
| ------ | -------------------------------------------------------- |
| RNF-01 | Latencia de reporte menor a 2 segundos.                  |
| RNF-02 | Disponibilidad superior al 99 %.                         |
| RNF-03 | Protección de credenciales y datos transmitidos.         |
| RNF-04 | Recuperación de la conectividad en menos de 30 segundos. |


**Requisitos complementarios**

**Autonomía energética:** El nodo terminal debe operar continuously por más de 8 horas con una batería Li-Po de 3.7 V/1000 mAh en modo activo de transmisión.

**Factor de forma y ergonomía:** El dispositivo debe utilizar un encapsulado wearable ligero, con peso inferior a 150 g.

**Buffer local:** El terminal debe disponer de memoria capaz de almacenar temporalmente un mínimo de 1000 tramas biomédicas durante períodos sin conectividad.

## 1.4. Criterios de aceptación y matriz de requisitos

**Matriz de trazabilidad de requisitos**

| **ID** | **Descripción**              | **Componente / Diseño**                  | **Evidencia / Prueba**                    |
| ------ | ---------------------------- | ---------------------------------------- | ----------------------------------------- |
| RF-01  | Registrar terminales IoT     | Handshake MQTT + script de auto-registro | Logs del Broker                           |
| RF-02  | Recibir telemetría biomédica | Contrato JSON y parser Edge              | Capturas Wireshark e inspección de topics |
| RF-03  | Almacenar mediciones         | Base de datos / almacenamiento           | Verificación de persistencia y timestamps |
| RF-04  | Generar alertas fisiológicas | Motor de reglas                          | Prueba de disparo de alerta               |
| RNF-01 | Latencia E2E < 2 s (P95)     | MQTT QoS 1                               | Experimento A                             |
| RNF-02 | Disponibilidad > 99 %        | Broker / supervisor                      | Prueba de estabilidad                     |
| RNF-03 | Seguridad y cifrado          | TLS / autenticación                      | Auditoría de seguridad                    |
| RNF-04 | Recuperación < 30 s          | Buffer y reconexión                      | Experimento C                             |

**Evaluación de requisitos no funcionales**

| **Atributo**   | **Métrica**        | **Escenario de prueba**         | **Umbral aceptable** |
| -------------- | ------------------ | ------------------------------- | -------------------- |
| Latencia       | P95 (ms)           | Operación nominal               | < 2000 ms            |
| Disponibilidad | % Uptime           | Prueba continua                 | > 99 %               |
| Resiliencia    | T_rec (s)          | Corte del AP                    | < 30 s               |
| Integridad     | % pérdida de datos | Reconexión y vaciado del buffer | 0 %                  |

El requisito no funcional de mayor impacto es RNF-04, relacionado con la recuperación ante fallas y el almacenamiento temporal de datos. Este requisito implica implementar un mecanismo de almacenamiento temporal y una estrategia de retransmisión que permita recuperar las mediciones después de una interrupción de conectividad.

## 1.5. Arquitectura del sistema

La arquitectura propuesta está compuesta por un wearable con sensores de frecuencia cardíaca y SpO₂ conectado a un ESP32. El ESP32 se encarga de adquirir, validar, identificar y transmitir las mediciones.
Posteriormente, el ESP32 utiliza una conexión Wi-Fi para transmitir la telemetría mediante HTTP o MQTT hacia una API o Broker MQTT. Los datos recibidos son procesados y almacenados en el backend para posteriormente ser consultados por el personal de salud y utilizados en la generación de alertas.La arquitectura se organiza en capas de Dispositivo, Edge, Red/Transporte, Backend/Cloud y Aplicación.

**Componentes principales**

| **Componente**   | **Función**                               |
| ---------------- | ----------------------------------------- |
| MAX30102         | Adquisición de frecuencia cardíaca y SpO₂ |
| ESP32            | Procesamiento y comunicación              |
| Wi-Fi            | Conectividad inalámbrica                  |
| Broker MQTT      | Recepción y distribución de mensajes MQTT |
| API REST         | Recepción de mensajes HTTP                |
| Backend          | Procesamiento de información              |
| Base de datos    | Almacenamiento de mediciones              |
| Motor de alertas | Detección de valores anómalos             |
| Dashboard        | Visualización de información              |


**Responsabilidades por capa**

| **Capa**         | **Responsabilidad**                                                            |
| ---------------- | ------------------------------------------------------------------------------ |
| Dispositivo      | Captura de señales mediante MAX30102.                                          |
| Edge / ESP32     | Procesamiento local, validación, serialización JSON y gestión de conectividad. |
| Red y transporte | Wi-Fi y protocolos HTTP/MQTT.                                                  |
| Backend / Cloud  | Broker/API, procesamiento, almacenamiento y alertas.                           |
| Aplicación       | Visualización de datos y alertas para personal médico.                         |

**Diagrama de contexto**
> **[Insertar imagen aquí]**
**Diagrama de componentes**
> **[Insertar imagen aquí]**
**Diagrama de secuencia**
> **[Insertar imagen aquí]**


# 2. Firmware y Modularidad

## 2.1. Hardware y herramientas utilizadas

| **Categoría**           | **Elemento / Herramienta** | **Descripción / Función**                                                 |
| ----------------------- | -------------------------- | ------------------------------------------------------------------------- |
| Hardware                | ESP32                      | Microcontrolador principal para procesamiento y comunicación inalámbrica. |
| Hardware                | Sensor MAX30102            | Adquisición de pulsioximetría y frecuencia cardíaca.                      |
| Hardware                | Batería Li-Po              | Alimentación del wearable (3.7 V / 1000 mAh).                             |
| Hardware                | Access Point Wi-Fi         | Proporciona la conectividad de red local para el dispositivo.             |
| Software / Herramientas | Arduino IDE / C++          | Entorno de desarrollo y lenguaje para la programación del firmware.       |
| Software / Herramientas | ArduinoJson / PubSubClient | Librerías para serialización de mensajes JSON y cliente MQTT.             |
| Software / Herramientas | MQTT / HTTP                | Protocolos de comunicación de transporte de telemetría.                   |
| Software / Herramientas | Broker MQTT                | Servidor para gestión e intercambio de mensajes.                          |
| Software / Herramientas | Wireshark                  | Herramienta para análisis e inspección del tráfico de red.                |
| Software / Herramientas | Git                        | Sistema de control de versiones para el proyecto.                         |

## 2.2. Arquitectura y estructura del firmware

El firmware está diseñado de manera modular para separar las principales responsabilidades del nodo ESP32.

**Adquisición de datos**
El ESP32 obtiene las mediciones biomédicas provenientes del sensor MAX30102. Las variables principales consideradas son frecuencia cardíaca y saturación de oxígeno.

**Validación**
Las mediciones son verificadas antes de ser transmitidas. El mensaje contiene el campo sensor_valid, que permite identificar si la medición obtenida es válida.
También se considera el campo alert para indicar la presencia de una condición que requiera atención.

**Conectividad Wi-Fi**
La función setupConnectivity() configura el ESP32 en modo estación temporal, las mediciones generadas durante la interrupción se mantienen en un y establece la conexión con la red Wi-Fi.

**Construcción del mensaje**
La función buildJson() construye el mensaje JSON incorporando:

- Identificador del nodo.
- Número de secuencia.
- Timestamp.
- Frecuencia cardíaca.
- SpO₂.
- Estado del sensor.
- Estado de alerta.
- RSSI.

**Transporte HTTP/MQTT**
El firmware contempla funciones separadas para el transporte:
sendHttp() para comunicación HTTP.
publishMqtt() para comunicación MQTT.
La implementación permite comparar ambos mecanismos de transporte.

**Reconexión y recuperación**
La función maintainConnection() verifica continuamente el estado de la conexión MQTT y realiza el proceso de reconexión cuando el enlace se pierde. En caso de implementar almacenamiento temporal, las mediciones generadas durante la interrupción se mantienen en un buffer para ser transmitidas posteriormente.

## 2.3. Implementación y evidencias de funcionamiento

El firmware implementado utiliza las bibliotecas WiFi.h, PubSubClient.h y ArduinoJson.h.
Estructura principal del firmware:

```
setupConnectivity();
maintainConnection();
buildJson();
publishMqtt();
```

**Evidencias de funcionamiento requeridas:**

- Conexión Wi-Fi.
- Lectura del sensor.
- Construcción del JSON.
- Publicación MQTT.
- Comunicación HTTP.
- Reconexión.
- Recepción de datos en el backend.

El código fuente completo se incluirá en los anexos.

# 3. Comunicación HTTP/MQTT y Contrato de Datos

## 3.1. Estructura y formato de los datos

**JSON**

**Identificación**

**Secuencia**

**Timestamp**

**Frecuencia cardíaca y SpO₂**

## 3.2. Comunicación HTTP y MQTT

**HTTP: endpoint y funcionamiento**

**MQTT: broker, topics y QoS**

**Evidencias de comunicación**

## 3.3. Comparación y selección del protocolo

**4. Experimentación y Métricas**
**4.1. Plan experimental y métricas**
**4.2. Experimentos realizados**
**HTTP vs MQTT**

**RSSI/distancia vs pérdida de paquetes**

**Interrupción Wi-Fi y recuperación**

**Periodicidad de muestreo**

## 4.3. Resultados y análisis

**Tablas**

**Gráficas**

**Latencia**

**Tasa de entrega**

**Tiempo de recuperación**

**Análisis de resultados**

## 4.4. Conclusiones de la experimentación

# 5. Git, Documentación y ADR

## 5.1. Repositorio y organización del proyecto

## 5.2. Historial de commits y evidencias

## 5.3. Documentación del proyecto

## 5.4. ADR-01: Selección del protocolo HTTP/MQTT

**Contexto**

**Alternativas**

**Criterios de decisión**

**Decisión**

**Consecuencias**

# 6. Validación y Conclusiones

## 6.1. Validación de requisitos

## 6.2. Cumplimiento de los objetivos

## 6.3. Conclusiones

## 6.4. Limitaciones y trabajos futuros

# 7. Referencias Bibliográficas

# 8. Anexos

## 8.1. Código fuente

## 8.2. Evidencias y capturas

## 8.3. Datos experimentales

## 8.4. Evidencias de Git
