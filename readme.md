GUIDgen - alternative GUID generator GUI.
==============================================

Copyright (c) 2019 Akimov Vitaly.

Screenshots
-----------------

Default window state:
![Default window](https://github.com/duox/guidgen/blob/master/docs/images/screenshot01.png)

List of available GUID generators:
![GUID generators](https://github.com/duox/guidgen/blob/master/docs/images/screenshot02.png)

List of available GUID formats:
![GUID formats](https://github.com/duox/guidgen/blob/master/docs/images/screenshot02.png)

Internal help:
![Help window](https://github.com/duox/guidgen/blob/master/docs/images/screenshot02.png)

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
