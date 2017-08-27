<map version="freeplane 1.3.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="Abschlussbericht 1597: Landebahnensuche aus H&#xf6;hendaten" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1503849393217"><hook NAME="MapStyle">

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node">
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right">
<stylenode LOCALIZED_TEXT="default" MAX_WIDTH="600" COLOR="#000000" STYLE="as_parent">
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.note"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important">
<icon BUILTIN="yes"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000">
<font SIZE="18"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,1" COLOR="#0033ff">
<font SIZE="16"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,2" COLOR="#00b439">
<font SIZE="14"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,3" COLOR="#990000">
<font SIZE="12"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,4" COLOR="#111111">
<font SIZE="10"/>
</stylenode>
</stylenode>
</stylenode>
</map_styles>
</hook>
<hook NAME="AutomaticEdgeColor" COUNTER="11"/>
<node TEXT="Aufgabenstellung" POSITION="right" ID="ID_587092141" CREATED="1503849394244" MODIFIED="1503854512917">
<icon BUILTIN="idea"/>
<edge COLOR="#ff0000"/>
<node TEXT="Beschreibung Ausgangsdaten" ID="ID_1911359262" CREATED="1503849402892" MODIFIED="1503849425152"/>
<node TEXT="Beschreibung Zielsetzung" ID="ID_1670126205" CREATED="1503849415979" MODIFIED="1503849426954"/>
</node>
<node TEXT="Architektur" POSITION="right" ID="ID_871946921" CREATED="1503849429082" MODIFIED="1503854519876">
<icon BUILTIN="bookmark"/>
<edge COLOR="#ff00ff"/>
<node TEXT="grobe und feine parallelit&#xe4;t" ID="ID_406549730" CREATED="1503853460388" MODIFIED="1503853465473"/>
</node>
<node TEXT="Datenextraktion aus GeoTIFF" POSITION="right" ID="ID_456707385" CREATED="1503849436219" MODIFIED="1503854519879">
<icon BUILTIN="bookmark"/>
<edge COLOR="#00ffff"/>
<node TEXT="GeoTIFF" ID="ID_376161208" CREATED="1503849464954" MODIFIED="1503849469080"/>
<node TEXT="einlesen" ID="ID_1037399205" CREATED="1503849469507" MODIFIED="1503849471657"/>
<node TEXT="Konvertierung" ID="ID_29401478" CREATED="1503849472714" MODIFIED="1503849476465">
<node TEXT="WGS84" ID="ID_1515281820" CREATED="1503849525506" MODIFIED="1503849528330"/>
<node TEXT="Beschreibung Besonderheuiten Geo daten" ID="ID_1088412927" CREATED="1503849529201" MODIFIED="1503849540003"/>
<node TEXT="Problem. geoTIFF ist nicht strikt nach Norden ausgerichtet" ID="ID_172158484" CREATED="1503849683736" MODIFIED="1503849700872"/>
</node>
<node TEXT="GeoTIFFWrapper" ID="ID_1395036987" CREATED="1503849483923" MODIFIED="1503849492650">
<node TEXT="Kacheln" ID="ID_40671411" CREATED="1503849493739" MODIFIED="1503849495963"/>
</node>
</node>
<node TEXT="Durchmusterung" POSITION="right" ID="ID_187777761" CREATED="1503849541386" MODIFIED="1503854512920">
<icon BUILTIN="idea"/>
<edge COLOR="#ffff00"/>
</node>
<node TEXT="Datenbank Manager" POSITION="right" ID="ID_1840769510" CREATED="1503849547906" MODIFIED="1503854519880">
<icon BUILTIN="bookmark"/>
<edge COLOR="#7c0000"/>
</node>
<node TEXT="Post Processing Merge" POSITION="right" ID="ID_1759914529" CREATED="1503849557098" MODIFIED="1503854519881">
<icon BUILTIN="bookmark"/>
<edge COLOR="#00007c"/>
<node TEXT="langsam weil sehr viele Datenbankzugriffe" ID="ID_677030077" CREATED="1503849621417" MODIFIED="1503849638329"/>
</node>
<node TEXT="GUI auf React Basis" POSITION="right" ID="ID_264021759" CREATED="1503849568514" MODIFIED="1503854519882">
<icon BUILTIN="bookmark"/>
<edge COLOR="#007c00"/>
</node>
<node TEXT="Ausblick und offene Punkte" POSITION="right" ID="ID_301875271" CREATED="1503849578497" MODIFIED="1503854512921">
<icon BUILTIN="idea"/>
<edge COLOR="#7c007c"/>
<node TEXT="Merge als eigenen nachgeschalteten Prozess in CPP" ID="ID_1360884724" CREATED="1503849584978" MODIFIED="1503849604648">
<node TEXT="keine Datenbankzugriffe" ID="ID_882084468" CREATED="1503849605794" MODIFIED="1503849612681"/>
<node TEXT="gecachte Daten vor eintrag in DB" ID="ID_1849853438" CREATED="1503849613304" MODIFIED="1503849619097"/>
</node>
<node TEXT="Richtungskorrektur des GeoTIFF" ID="ID_1729286178" CREATED="1503849668713" MODIFIED="1503849677295"/>
<node TEXT="Kollisionsabfrage mit Objekten aus OSM Datenbasis" ID="ID_1717869428" CREATED="1503849649096" MODIFIED="1503849667216"/>
<node TEXT="EInbettung mehrerer Kacheln aus GeoTIFF Handler per MPI" ID="ID_395230043" CREATED="1503854062485" MODIFIED="1503854078229"/>
<node TEXT="Push f&#xfc;r neue Ergebnisse in Webclient" ID="ID_786116522" CREATED="1503854853489" MODIFIED="1503854862516"/>
</node>
</node>
</map>
