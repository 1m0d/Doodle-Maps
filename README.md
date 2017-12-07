# Doodle Maps

#### TODO translate readme and developer documentaion

### Altalanos leiras

A program egy hexagon alapu terkepet, es ket koordinatat kap bemenetnek.
A ket koordinata a ket pont amik kozott megkeresi a legrovidebb es a
leggyorsabb utat. A legrovidebb es a leggyorsabb ut kozotti kulonbseget
a tile-ok nehezsege teszi lehetove. 0-5-ig terjed a nehezsegi szint,
ahol 1 a legegyszerubb (pl ret) es 5 a legnehezebb (pl hegy), a 0 pedig
az atkelhetetlent jelenti. Miutan megkapta a terkepet a program egy grafot
keszit belole, majd A\* (A star) algoritmus felhasznalasaval megkeresi a keresett
utakat. Az utak megtalalasa utan egy vektor alapu kepfajlt general a program amin
vizualizalva vannak a hexagon tile-ok es az utak is. Az utakat ket
urleny abrazolja, a sarga urleny megy a legrovidebb, a kek pedig a
leggyorsabb uton.

### Hasznalat

```bash
./run <path_to_json_file> > map.svg
```

### Bemenet

Bemenetnek a program egy JSON fajlt fog fogadni. Ebben a fajlban lesz
benne a terkep merete, nehezsegi szintek szerint a tile-ok koordinatai,
es a ket pont amik kozott az utakat keressuk. Alapbol meret alapjan
general egy csak 0 nehezsegu tile-okbol allo terkepet a program, igy
ezeket a tile-okat kulon nem kell megadnunk. Az osszes tobbi tile
nehezsegi szintek szerint vannak csoportositva, koordinatak szerint, vegul
pedig a ket keresett pont.
Ezenkivul a fajlban van meg a rajzolt terkep hexagon magassaga (pixelben
megadva), es a rajzolasi felulet.
Pelda egy 5x5-os terkepre (example\_maps/map1.json):

```json
{
  "map":{
    "size":[5, 5],
    "tiles":{
      "1":[[1, 0], [2, 0], [1, 3], [2, 4], [2, 1]],
      "2":[[1, 2], [3, 3], [0, 0]],
      "3":[[2, 2]],
      "4":[[0, 2]],
      "5":[[0, 1], [3, 2], [4, 4]]
    },
    "tiles_of_interest":[[0, 0], [2, 4]]
  },
  "svg":{
    "hexagon_height":140,
    "draw_area":[1280, 720]
  }
}
```

A json fajl parseolasahoz a [cJSON](https://github.com/DaveGamble/cJSON) konyvtarat hasznalja a program.

### Kimenet

A kimenet egy SVG fajl lesz, amiben meglesznek kolonboztetve grafikusan
a kulonbozo nehezsegu tile-ok, es ra lesz rajzolva a kiszamitott legrovidebb, es
leghosszabb ut, kulonbozo szinekkel.

### Pelda kimenet

Pelda kimenete az example\_maps/map1.json fajlnak:

![example\_out](https://i.imgur.com/14Mu6gO.png, "example_out")

### Terkep

A terkep egy offset koordinatas odd-r vizszintes hexagon terkep lesz. Ez
azt jelenti, hogy a legfelso sor az elso sor, es ebben a sorban minden
tile vizszintesen egymas mellett van, viszont igy az oszlopokban a
tile-ok nem tokeletesen fuggolegesek egymassal.
A kovetkezo kep egy 7x7-es meretu terkepnek mutatja be a koordinatait:

![map\_type](https://imgur.com/fCVrqn6.png "map_type")

---
Credit for artwork used: [Kenney.nl](https://kenney.nl "credit") (CC0)
