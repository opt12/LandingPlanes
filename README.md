# LandingPlanes

Dieses Repo wird benutzt um die Arbeiten der Gruppe "pthreads" im Fachpraktikum 1597 der Fernuni-Hagen im SS2017 zu koordinieren.

## Installationsanleitung

Um die abgelegte Software auszuprobieren ist ein Rechner mit
einer aktuellen Linux-Distribution erforderlich. Notwendig ist
insbesondere:

-   eine aktuelle Installation von Node.js ( ≥ Version 8)
-   eine aktuelle Installation von NPM
-   eine aktuelle installation von bower (`npm install -g bower`)
-   eine aktuelle Version der gdal-Library, deren Header-Dateien unter
    `/usr/include/gdal` liegen müssen
-   eine aktuelle Version des kompilierten gdal-bin Pakets (insbesondere
    `gdalinfo` im PATH)
-   eine laufende Instanz einer MongoDB, welche sich auf
    `mongodb://localhost:27017` ansprechen lässt
-   Terrain Daten (sind z. B. verfügbar unter [Digitale Geländemodelle von Deutschland](http://data.opendataportal.at/dataset/dtm-germany))

Zunächst wird das gesamte Projekt von Github auf den Rechner geklont:

````git clone git@github.com:opt12/LandingPlanes.git````

Dann muss die Search Engine Komponente gebaut werden:

Die `1597_searchEngine` Komponente und die Bibliotheken `plane_library.a`
und `libGeoTiffHandler.a` können gemeinsam erstellt
werden:

````bash
cd LandingPlanes/src/1597_searchEngineWrapper/
make
````

Nun muss der Landing Client gebaut werden.

````bash
cd LandingPlanes/src/1597_LandingClient
npm install
bower install
npm run local-build
````
Es ist nun eine Datei `LandingPlanes/src/1597_LandingClient/bundle.js`
erstellt worden.

Als letzte Komponente muss der Database Manager vorbereitet werden.

````bash
cd LandingPlanes/src/1597_DatabaseManager
npm install
````

Nun muss lediglich die Datei
`LandingPlanes/src/1597_DatabaseManager/package.json` überprüft und
gegebenenfalls angepasst werden. Es müssen die Pfade im Skript
````
"start": "node ./bin/www --tiffpath=/home/xxxxx/dev/LandingPlanes/terrain
         --wrapper=../1597_searchEngineWrapper/bin/1597_searchEngineWrapper
         --clientpath=’../1597_LandingClient’
````
angepasst werden.

`tiffpath`: Ist der Pfad zu den GeoTIFF Dateien. (Der muss gesetzt werden,
da aktuell nur der Dateiname übergeben wird, nicht der Pfad)

`wrapper`: Ist der Pfad zur kompilierten `1597_searchEngineWrapper`-Applikation. Diese wird
automatisch gestartet und über einen Unix-Socket mit Daten gefüttert.

`clientpath`: Der Pfad zum Verzeichnis für die React-Applikation, also den
`1597_LandingClient`.

`terminal`: Ist ein optionaler Parameter um festzulegen, welche
Terminal-Emulation benutzt werden soll. `xterm` sollte auf allen Maschinen
funktionieren, falls der Defaultwert `gnome-terminal` nicht verfügbar ist.
Der Parameter kann dann noch mit `--terminal=’xterm’` hinzugefügt werden.

Wenn diese Vorbereitungen abgeschlossen sind, kann das System mit

````bash
cd LandingPlanes/src/1597_DatabaseManager
npm start
````
gestartet werden und über das öffnen von
[http://localhost:3000](http://localhost:3000) bedient werden.

Ein manueller Merge von überlappenden Landebahnen kann angestoßen werden,
wenn in einem eigenen Terminal das npm-Skript `cleanup` aufgerufen wird:

````bash
cd LandingPlanes/src/1597_DatabaseManager
npm run cleanup
````

