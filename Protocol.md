# Protocolo de comunicación entre cliente y servidor

Se provee un listado de comandos válidos que entienden las aplicaciones diseñadas. Dichos comandos deben ser pasados a través del objeto `Messenger` del directorio `common`, con una conexión a la otra punta efectuada.

## Cliente
Bajo ningún caso el cliente hace chequeos sobre las posiciones pasadas. *Estas deben ser validadas del lado del servidor!*

- move-[id]-[x]-[y]
- addunit-[id]-[x]-[y]
- removeunit-[id]
