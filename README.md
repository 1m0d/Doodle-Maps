# Utvonaltervezo

### Altalanos leiras

A program egy hexagon alapu terkepet, es ket koordinatat kap bemenetnek.
A ket koordinata a ket pont amik kozott megkeresi a legrovidebb es a
leggyorsabb utat. A legrovidebb es a leggyorsabb ut kozotti kulonbseget
a tile-ok nehezsege teszi lehetove. 0-5-ig terjed a nehezsegi szint,
ahol 1 a legegyszerubb (pl osveny) es 5 a legnehezebb (pl hegy), a 0 pedig
az atkelhetetlent jelenti. Miutan megkapta a terkepet a program egy grafot 
keszit belole, majd A\* (A star) algoritmus felhasznalasaval megkeresi a keresett
utakat. Az utak megtalalasa utan egy vektor alapu kepfajlt general a program amin
vizualizalva vannak a hexagon tile-ok es az utak is.

### Bemenet

Bemenetnek a program egy JSON fajlt fog fogadni. Ebben a fajlban lesz
benne a terkep merete, nehezsegi szintek szerint a tile-ok koordinatai,
es a ket pont amik kozott az utakat keressuk. Alapbol meret alapjan
general egy csak 0 nehezsegu tile-okbol allo terkepet a program, igy
ezeket a tile-okat kulon nem kell megadnunk. Az osszes tobbi tile
nehezsegi szintek szerint vannak csoportositva, koordinatak szerint, vegul
pedig a ket keresett pontot tartalmazza a fajl.
Pelda egy 7x7-es terkepre, ahol 7 tile-on kivul az osszes atkelhetetlen:

```json
{
  "map":{
    "size":[7, 7],
    "tiles":{
      "1":[[5, 3], [1, 2], [2, 4]],
      "2":[[0, 0], [3, 3]],
      "5":[[2, 3], [6, 4]],
    },
    "tiles_of_interest":[[2, 4], [3,3]]
  }
}
```



### Kimenet

A kimenet egy SVG fajl lesz, amiben meglesznek kolonboztetve grafikusan
a kulonbozo nehezsegu tile-ok, es ra lesz rajzolva a kiszamitott legrovidebb, es
leghosszabb ut, kulonbozo szinekkel. *Persze csak akkor, ha a ket megadott
pont kozott lehetseges az utazas.*

### Terkep

A terkep egy offset koordinatas odd-r vizszintes hexagon terkep lesz. Ez
azt jelenti, hogy a legfelso sor az elso sor, es ebben a sorban minden
tile vizszintesen egymas mellett van, viszont igy az oszlopokban a
tile-ok nem tokeletesen fuggolegesek egymassal. 
A kovetkezo kep a 7x7-es meretu terkepet mutatja be koordinatakkal:


![alt text](https://imgur.com/fCVrqn6.png "terkep")
