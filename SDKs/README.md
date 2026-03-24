# SDKs

Dieses Verzeichnis ist fuer lokale Hersteller-SDKs gedacht, die waehrend der Entwicklung
verwendet werden, aber nicht mit ins Repository sollen.

Struktur:

- `SDKs/Nikon/`
- `SDKs/Canon/`

Wichtig:

- Die Ordner selbst sind versioniert.
- Der Inhalt der Herstellerordner bleibt lokal und wird nicht nach GitHub gepusht.
- Wenn aus einem SDK etwas dauerhaft im Projekt gebraucht wird, muss es bewusst in eine
  projektinterne, commitbare Struktur uebernommen oder im Code sauber angebunden werden.
- Vendor-SDKs sollten nicht direkt aus diesen lokalen Ordnern redistribuiert werden, ohne
  die jeweilige Lizenz zu pruefen.
