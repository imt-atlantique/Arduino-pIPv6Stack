Arduino-pIPv6Stack
==================

A very light IPv6 stack for [Arduinos](http://www.arduino.cc/) UNO and [Xbee](http://www.digi.com/en/products/wireless/point-multipoint/xbee-series1-module) 
based on [Contiki OS](http://www.contiki-os.org/) network stack.
If you are using our Arduino µIPv6 stack, you're probably wondering why we created a lighter stack ?
Because Arduino UNO has to be IPv6 ready too ! With this stack, you'll be able to join a 6LoWPAN network using
Arduinos UNO (which only have 2kB of RAM).

With pIPv6 stack, we reduced RPL and CoAP functionnalities to keep an ultra small memory footprint.


![Arduino pIPv6 Stack](http://departements.telecom-bretagne.eu/data/rsm/pIPv6%20stack%20diagram.png)


Features
--------


* IP network stack with low-power standards : [6LoWPAN](http://datatracker.ietf.org/wg/6lowpan/charter/), [RPL](http://tools.ietf.org/html/rfc6550), and [CoAP](http://datatracker.ietf.org/doc/draft-ietf-core-coap/)
* Compatible with all [Arduino](http://arduino.cc/en/Main/Products) boards 
* Based on [Contiki OS](http://www.contiki-os.org/) network stack
* Can be used on top of [IEEE 802.15.4](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-series1-module), IEEE 802.3 (Ethernet), IEEE 802.11 (Wifi) MAC layers *

*: We are only providing [XBee Series 1](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-series1-module) interface for now.

Get Started
-----------

To get started using pIPv6 stack, go to the [wiki](https://github.com/telecombretagne/Arduino-pIPv6Stack/wiki). The wiki
contains a step-by-step tutorial and examples necessary to build your first Arduino UNO Wireless Sensor Network.

![Arduino WSN](http://departements.telecom-bretagne.eu/data/rsm/heterogeneous%20wsn%20arch.png)


License
-------
 
This stack is open source software : it can be freely used both in commercial and non-commercial systems and the full source code is available.
