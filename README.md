# RTN_01

Progetto di simulazione rete real-time basato su OMNeT++.

## Panoramica

Il progetto modella una rete Ethernet con:
- nodi terminali con generazione traffico periodico;
- incapsulamento Ethernet e VLAN;
- scheduling su code con priorita;
- switch Ethernet;
- scenario di simulazione complesso nella rete Sim1.

La logica applicativa principale e implementata in src, mentre gli scenari e i parametri di simulazione sono in simulations.

## Struttura del repository

- src: moduli C++, file NED e file msg del modello
- simulations: reti NED e configurazioni ini
- Makefile: build wrapper del progetto

## Requisiti

- OMNeT++ 6.1 (o compatibile con i file generati)
- compilatore C++ supportato da OMNeT++
- ambiente shell con tool di build disponibili

## Build

Dalla cartella principale del progetto:

1. Genera i makefile
   make makefiles

2. Compila il progetto
   make

Pulizia:

- pulizia build corrente
  make clean

- pulizia completa
  make cleanall

## Esecuzione simulazione

Scenario principale configurato in simulations/sim1.ini con rete Sim1.

Avvio rapido dallo script in simulations:

- entra in simulations
- esegui il file run

In alternativa, esegui direttamente il binario compilato usando i path NED corretti.

## Configurazione

- network di default: Sim1
- registrazione risultati scalari e vettoriali abilitata
- parametri traffico, VLAN e deadline definiti in simulations/sim1.ini

## Moduli principali

- AppDispatcher: inoltro tra applicazioni e livello inferiore
- PeriodicTrafficGen: generazione periodica di DataPacket
- EthEncap: incapsulamento e decapsulamento frame Ethernet
- EtherMAC: gestione code trasmissione e ricezione
- RelayUnit: logica di relay

## Output simulazione

I risultati della simulazione sono salvati secondo la configurazione OMNeT++ (file scalari e vettoriali).