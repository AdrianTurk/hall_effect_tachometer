# hall_effect_tachometer
Tacómetro genérico de reemplazo para piezas discontinuadas.

---
### FUNCIONAMIENTO:
><br>&nbsp;Mide velocidad de cinta transportadora, por medio de un sensor de efecto hall, mostrando el tiempo de coccion en un display de 7 segmentos de 4 digitos.<br><br>
>&nbsp; Tambien controla el apagado de un horno en base a un tiempo de retrazo y una entrada N/A para reducir el estres termico.<br><br>

<br>

---
<br>

### B.O.M.:

>- 1 Display TM1637 ([ DataSheet ](https://components101.com/displays/tm1637-grove-4-digit-display-module))
>- 1 Arduino Mini
>- 1 Fuente 110v -> DC 5vcc 500mA
>- 1 Sensor de efecto Hall A3144
>- 1.5m Cable mallado anti-ruido

<br>

---

<br>

### LIBRERIAS:
- https://github.com/avishorp/TM1637<br><br>

---

<br>

### CONEXIONES:

<br>

---
 ARDUINO MINI | DC | TM1637 | SENSOR | LLAVE | RELE CORTE
:-------:|:--:|:------:|:------:|:-----:|:---:
2 |.|.|D_OUT|.|.
3 |.| CLK |.|.|.
4 |.| DIO |.|.|.
5 |.|.|.|COOLING IN|.
6 |.|.|.|.|COOLING OUT
VCC| +5vcc | VCC | VCC | VCC|.
GND| GND | GND | GND |.|GND
---
<br><br>

---
### CONSTANTES DE CONFIGURACIÓN:
---

* FACTOR_ADJ: &nbsp; Establece el factor de ajuste del tiempo de coccion, a mayor factor, la velocidad mostrada se incrementa. Depende de las caracteristicas fisicas de la cinta