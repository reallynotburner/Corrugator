# Corrugator

Cardboard based combat robot that uses an electric chainsaw as a weapon.

## Materials
- Arduino Nano microcontroller
- Toshiba TB6612FNG Dual H-Bridge motor control carrier board
- 2x Inland (Microcenter) 2 channel relay boards for weapon power and weapon direction control.
- MN-6 rx 2.4 GHz 6 channel RC receiver
- custom PCB

## Principle
An Arduino controls 4 SPDT relays.  2 relays are for V+ and GND battery connections to chainsaw.  2 are used to reverse the +/- leads
in order to change direction of saw.  The logic to control the relays and motors is based on the state of the 2.4 GHz receiver pins.
