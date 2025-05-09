# Dobro došli na repozitorijum! 
## Ovaj repozitorijum je napravljen u sklopu predmeta Mikrokontrolerski sistemi / Računarska elektronika na Elektrotehničkom fakultetu Univerziteta u Banjoj Luci.
## U nastavku su predstavljene osnovne informacije o predmetu, kao i tema projektnog rada za čije svrhe je ovaj repozitorijum kreiran.

## Sadržaj
1. [O predmetu](#o-predmetu)  
2. [Opis projekta](#opis-projekta)  
3. [Tim](#tim)  
4. [Pravila rada na projektu](#pravila-rada-na-projektu)  
5. [Link ka repozitorijumu](#link-ka-repozitorijumu)
-------------------------------------------------------------------
## O predmetu
Mikrokontrolerski sistemi, odnosno Računarska elektronika su predmeti sa četvrte godine smjera Računarsko inženjerstvo, odnosno smjera Elektronika, redom. 
U okviru predmeta studenti treba da savladaju teorijska i praktična znanja iz oblasti: 
* Arhitektura i organizacija naprednih verzija mikrokontrolera
* Memorijski sistem, magistrale, vektori i prekidni sistem
* Signali mikrokontrolera
* Periferne jedinice na čipu
* Analogni i digitalni ulazi i izlazi
* Povezivanje sa periferijama, komunikacioni interfejsi
* Programiranje mikrokontrolera (upravljačke strukture i potprogrami)
* Primjena u digitalnom upravljanju
* Razvoj projekta sa mikrokontrolerom
  
Više informacija o samom predmetu, načinu bodovanja i zaduženima na istom može se pronaći na [zvaničnom sajtu Elektrotehničkog fakulteta](https://etf.unibl.org/).

---------------------------------------------------------------------
## Opis projekta

Color Sorter je uređaj namjenjen za automatsko razvrstavanje objekata na osnovu njihove boje. Funkcioniše tako što koristi senzore za prepoznavanje boje i mehanički sistem za premještanje predmeta u odgovarajuće posude.

U sklopu projekta koriste se RGB senzor, servo motor, LED dioda, dugme, mikrokontroler i Wi-Fi modul, a koje je potrebno integrisati u funkcionalan sistem. Komunikacioni interfejsi koji se koriste su UART, SPI i I2C interfejs. 

Funkcionalnosti koje sistem treba da realizuje su:
* Prepoznavanje boja i njihovo raspoređivanje u lijevu ili desnu posudu
* Zadavanje smjera razvrstavanja za svaku boju
* Start i stop sistema pomoću dugmeta i iz aplikacije
* Bežična komunikacija između mikrokontrolera i aplikacije putem Wi-Fi modula
* Signalizacija rada sklopa pomoću LED diode

Za više informacija o svakoj komponenti, kao i o razvoju projekta, posjetite [Wiki stranicu](https://github.com/lukavidic/ColorSorter_C/wiki) repozitorijuma.

 -------------------------------------------------------------------
## Tim
Tim čine Boško Branković i Dejana Smiljanić sa smjera Računarsko inženjerstvo, te Jelena Radaković sa smjera Elektronika.

Kontakt informacije:
* Dejana Smiljanić: dejana.smiljanic@student.etf.unibl.org
* Jelena Radaković: jelena.radakovic@student.etf.unibl.org
* Boško Branković: bosko.brankovic@student.etf.unibl.org

  -------------------------------------------------------------------
## Pravila rada na projektu: 

   ### 1. Pravila commit-ovanja: 
   * Commit ima sljedeću strukturu: \
     git commit -m "Issue #broj_issue: Poruka"
   ### 2. Poruka u commit - u:
   * Jasna, sažeta, razumljiva za ostatak tima
   * Isključivo u formatu kao što je navedeno
   ### 3. Ocjena težine:
   * Svaki zadatak mora biti ocijenjen na osnovu težine (bez obzira čiji je to zadatak)
   * Princip ocjenjivanja težine: 1 - veoma lako, 10 - veoma (isuviše) teško
   ### 4. Radni sati:
   * Svaki zadatak mora imati podatak o vremenu provedenom pri radu istog
   * Informaciju o utrošenom vremenu napisati u komentaru zadatka
   ### 5. Zatvaranje issue-a:
   * Zaduženi za konkretan issue je dužan da istestira funkcionalnost koja je njime obuhvaćena
   * Tek nakon testiranja issue se može zatvoriti
     
-------------------------------------------------------------------
## Link ka repozitorijumu
[Klikom na ovaj tekst](https://github.com/lukavidic/ColorSorter_C.git)

-------------------------------------------------------------------
