# RFHexSniffer

Librería para Arduino que captura una señal digital demodulada proveniente de un receptor RF y muestra la trama recibida en hexadecimal.

> Importante: esta librería no sintoniza directamente 477.92 MHz ni descifra protocolos cifrados.  
> Su función es leer una salida digital de un receptor RF ya demodulado y convertir los pulsos recibidos en bits y bytes en formato hexadecimal.

## Para qué sirve

Esta librería sirve para:

- Leer pulsos digitales de un receptor RF
- Interpretar esos pulsos como bits
- Agrupar los bits en bytes
- Mostrar la trama recibida en hexadecimal por puerto serie

Es útil para señales simples tipo:

- OOK
- ASK
- protocolos propietarios sin cifrado real
- mandos o sensores que entregan una salida digital estable

## Limitaciones

Esta librería **no**:

- sintoniza frecuencias por sí sola
- funciona conectando el Arduino directamente a una antena
- descifra señales cifradas
- entiende automáticamente cualquier protocolo complejo
- sirve con un CC1101 para 477.92 MHz

## Nota importante sobre 477.92 MHz

La frecuencia **477.92 MHz** no entra dentro del rango del **CC1101**, por lo que **no sirve** para esa frecuencia.

Para trabajar en 477.92 MHz necesitas un receptor o transceptor que sí cubra esa banda, por ejemplo:

- un módulo específico para 477.92 MHz
- CC1120 / CC1121
- SX1278 / SX1276 según configuración
- cualquier receptor superheterodino o demodulador adecuado que entregue salida digital

## Cómo funciona

La librería:

1. detecta cambios de estado en un pin digital
2. mide la duración de cada pulso en microsegundos
3. clasifica cada pulso como `0` o `1` según un umbral
4. agrupa los bits en bytes
5. devuelve la trama en hexadecimal

## Instalación

### Opción 1: instalación manual

1. Descarga o copia esta carpeta
2. Renómbrala como `RFHexSniffer`
3. Colócala dentro de tu carpeta de librerías de Arduino

Por ejemplo:

- Windows: `Documentos/Arduino/libraries/RFHexSniffer`
- Linux: `~/Arduino/libraries/RFHexSniffer`
- macOS: `~/Documents/Arduino/libraries/RFHexSniffer`

4. Reinicia el IDE de Arduino

### Opción 2: como ZIP

1. Comprime la carpeta `RFHexSniffer`
2. En el IDE de Arduino entra en:
   `Programa > Incluir Librería > Añadir biblioteca .ZIP`
3. Selecciona el archivo ZIP

## Conexión

Debes usar un receptor RF que ya entregue una salida digital demodulada.

### Conexión típica

- salida digital del receptor RF -> pin 2 del Arduino
- GND del receptor -> GND del Arduino
- VCC del receptor -> alimentación correcta según su módulo

## Ejemplo básico

```cpp
#include <RFHexSniffer.h>

RFHexSniffer rf;

void setup() {
  Serial.begin(115200);

  rf.begin(2);
  rf.setShortLongThreshold(700);
  rf.setFrameGap(5000);
  rf.setBitOrderMSB(true);

  Serial.println("Escuchando...");
}

void loop() {
  rf.update();

  if (rf.available()) {
    char hexBuffer[129];
    rf.readHex(hexBuffer, sizeof(hexBuffer));

    Serial.print("HEX: ");
    Serial.println(hexBuffer);

    rf.clear();
  }
}
