# Protocolo de comunicación entre cliente y servidor

Se provee un listado de comandos válidos que entienden las aplicaciones diseñadas. Dichos comandos deben ser pasados a través del objeto `Messenger` del directorio `common`, con una conexión a la otra punta efectuada.

## Cliente
Bajo ningún caso el cliente hace chequeos sobre las posiciones pasadas. *Estas deben ser validadas del lado del servidor!*

- move-[id]-[x]-[y]
- addunit-[id]-[x]-[y]
- removeunit-[id]


## Server

### comandos que envia el server
- addunit-[id]-[x]-[y]-[Team]-[lifePoints]
- addbuilding-[id]-[x]-[y]-[Team]-[lifePoints]
- addnature-[id]-[x]-[y]-[Team]-[lifePoints]
- updateunit-[id]-[state]-[x]-[y]-[lifePoints]-[Team]
- updateoccupant-[id]-[x]-[y]-[lifePoints]
##### the territory of the "id" has the team "team" and the flag position is on the (x,y)
- updateterritory-[id]-[team]-[x]-[y]
##### id: id de la Factory; unitType: tipo de unidad creandose; min - sec: tiempo que falta para siguiente creacion de unidades, minutos y segundos respectivamente; lifePoints: la vida que le queda a la factory; team: el team de la factory (string con id_player)
- updatefactory-[id]-[unitType]-[min]-[sec]-[lifePoints]-[Team]
- updatebullet-[type]-[id]-[x]-[y]
- factorystats-[unitType]-[fireRate]-[min]-[sec]-[life]
#### indica al jugador que gano el juego
- winner
#### indica al jugador que perdio el juego
- loseryousuck
#### indica que el nombre ya esta tomado y no pudiste cambiar el nombre
- "name is already taken"
#### indica que lograste cambiar el nombre
- "Successfully change name to "-[new_name]

### comandos que recibe el server
##### mover unidad con "id" a la posicion (x,y)
- mv-[id]-[x]-[y]
##### indica que la unidad idUnit ataque a target
- atk-[idUnit]-[target]
##### indica que la unidad idUnit cambie el team a target
- grab-[idUnit]-[target]
##### indica que la factory id_factory ejecute un factory_cmd. Los factory_cmd son: "create" que empieza a crear la unidad ya seleccionada; "prev" que selecciona la unidad anterior dejando de crear unidades; "next" que selecciona la unidad siguiente dejando de crear unidades; "current" que devuelve el estado actual de la fabrica
- factory-[id_factory]-[factory_cmd]
#### comando utilizado para volver al menu una vez terminado de jugar
- returntomenu
#### comando para cambiar el id del jugador
- changename-[new_name]