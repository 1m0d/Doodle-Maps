# Programozoi Dokumentacio

## A program felepitese

A program 5 reszre van felbontva: main.c, parse\_map.c, map\_to\_graph.c,
a\_star.c, es draw\_map.c. Ezeken kivul egy kulso konyvtar is
hasznalatban van [cJSON](https://github.com/DaveGamble/cJSON) a bemeneti fajl ertelmezesehez.

### main
A main file eloszor leelenorzni a parameterkent kapott fajlt, majd
atalakitja char tombbe. A file\_to\_string() fuggvenyhez a kovetkezo
kodot hasznaltam fel: [stackoverflow](https://stackoverflow.com/a/7856790)
Ennek a pointeret adja at a parse\_map() fuggvenynek, ami egy ParsedMap
structot ad vissza (parse\_map.h) a JSON filebol kinyert adatokkal. A kovetkezo lepesben ezt atadja a create\_graph() fuggvenynek (map\_to\_graph.h),ami egy Graph structot ad vissza. Ezek utan kiszamitja a leggyorsabb, es legrovidebb utat a\_star.c segitsegevel, a grafbol es a ParsedMap map valtozokbol. Vegul pedig kirajzolja a terkepet, a kapott utakkal egyutt.
Ha mindennel kesz van, felszabaditja a dinamikusan lefoglalt memoriat.

### parse\_map
Ket strukturat hasznalok itt, az egyik a ParsedMap amiben minden adatot
a JSON filebol olvasok ki a tile\_count-ot leszamitva, a masik pedig a
Tiles amiben a hexagonok koordinatajat es atkelesi nehezseget tarolom. A
hexagonokat egy tiles tombben tarolom, amire a visszaadott ParsedMap
parsed\_map tiles pointere ramutat. Amikor elkezdtem elorebb jarni a
programban rajottem, hogy hulyeseg volt a koordinatakat tombkent
tarolni, nagyon megkonnyitettem volna a dolgomat, ha coordinate.x
corrdinate.y-kent mentem oket el, de erre mar keson jottem ra.

### map\_to\_graph
A graf node-okbol es edge-ekbol all. A grafban egy Node pointer mutat az
osszes nodera. Es minden node ramutat a sajat edge-eire. Egy tombben
van az osszes edge, de minden node-nak van egy pointere amely egy Edge
pointer tombre mutat, memoriatakarekossag erdekeben.

A fugveny eloszor megkeresi minden node szomszedjait, majd ebbol hozza
letra az edgeket, es rendeli oket a nodeokhoz. A nodeokban el van
tarolva a celtol valo fizikai tavolsag, ez igazabol az a legkisebb szam
amivel el lejhet jutni az adott nodetol a vegpontig hexagonokon
lepkedve. Ezt a find\_distance() fugveny szamolja ki, amihez ezt a kodot
hasznaltam fel: [find\_distance](https://stackoverflow.com/a/25986884)
A graphban ramutatok az osszes edget tartalmazo tombre is, bar erre az
utkereseshez nincs szukseg, de igy konnyebb lesz kesobb felszabaditain a
helyet.

### a\_star
Ez a fuggveny keresi meg az utvonalakat.
Itt ket strukturat hasznalok, az egyik a PriorityQue, ami egy lancolt
lista, ezzel dontom el, hogy melyik nodera mennyire kedvezo ralepni. A
masik pedig a VisitedNodes, ami szinten egy lancolt lista, ebben tarolom
azokat a node-okat, amire mar raleptem, hogy ne menjen elore-hatra az
algoritmus.

Itt, amire erdemes kiterni az, hogy amikor egy node mar benne van a
prioritasi sorban, azzal az esettel kulon kell foglalkozni, erre van az
already\_in\_que() fuggveny.

### draw\_map
Az svg fajlnak egy 10MB-os limitje van, azert hasznalok statikus tombot
az svg stringhez mert igy a legegyszerubb, es mast hasznalva lassabb es
processzor heavy lenne a program. Az svg fajl elejen aliasokat adok a
kepfajloknak, hogy konnyebb legyen eldonteni, hogy hova mi megy. A JSON
fileban megadott hexagon magassagbol szamolom ki a hexagon szelesseget
es az utat jelzo ikonoknak az eltolasat is. A program ugy rajzolja ki a
terkepet, hogy alapbol minden hexagont egy helyre rak, de koordinataktol
fuggoen eltolja oket az alaphelyzetukbol. Az elso hexagon csucsainak
koordinatait a magassagabol es a szelessegebol szamolom ki, de
elhelyezesnel eltolom mindket tengelyen, hogy esztetikuszabb legyen.
