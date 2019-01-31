GUIDgen - alternative GUID generator GUI.
==============================================

Copyright (c) 2019 Akimov Vitaly.

Screenshots
-----------------

Default window state:
![Default window](docs/images/screenshot1.jpg)

List of available GUID generators:
![GUID generators](docs/images/screenshot2.jpg)

List of available GUID formats:
![GUID formats](docs/images/screenshot3.jpg)

Internal help:
![Help window](docs/images/screenshot4.jpg)

GUID generators
-----------------

A GUID generator create a GUID which is sent to the selected formatter.

List of supported GUID generators:
- random GUID (implemented via CoCreateGuid),
- null GUID (all bytes are zero),
- invalid GUID (all bytes are 0xFFs),
- manual GUID (allows to manually give a GUID in the standard registry format and format it using one of formatters).

GUID formatters
-----------------

A GUID formatter takes GUID structure and makes a string in some format.

List of currently supported formatters:
- ...
