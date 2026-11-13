Trabajo Final PEV 2 - WEBS
Alumno: Eloisa Gleiser | Legajo: 48357598 | Modo: Idea propia – Multijugador competitivo de capturar zonas | Versión de Unreal: Unreal Engine 5.6.1

Reglas y Mecánica Principal
- Mecánica: WEBS es un videojuego multijugador competitivo en el que los jugadores controlan arañas que deben recorrer el escenario y capturar zonas tejiendo telarañas. También pueden recolectar seda para utilizarla como munición y disparar a sus oponentes, aturdiéndolos temporalmente para facilitar y o defender la captura de sus zonas.
- Cómo se gana: Gana el primer jugador que alcanza los 100 puntos o quien obtiene el puntaje más alto al finalizar el tiempo de partida. Los puntos se consiguen mediante la captura de zonas, las zonas dan distintos puntajes dependiendo de la dificultad del patrón requerido.

Ficha Técnica de Red
RepNotify
RepNotify: bIsStunned en AWebsCharacter -  Actualiza el estado de aturdimiento del personaje.
RepNotify: MatchPhase en AWebsGameState - Actualiza la fase de la partida (espera, cuenta regresiva, juego y finalización).
RepNotify: RemainingTime en AWebsGameState - Actualiza el tiempo restante de la partida.
RepNotify: CountdownRemaining en AWebsGameState - Actualiza la cuenta regresiva antes de comenzar.
RepNotify: WinnerPlayerState en AWebsGameState - Actualiza la información del jugador ganador.
RepNotify: bMatchIsDraw en AWebsGameState - Actualiza el resultado cuando existe un empate.
RepNotify: CapturedZones en AWebsPlayerState - Actualiza la cantidad de zonas controladas por el jugador.
RepNotify: TotalScore en AWebsPlayerState - Actualiza la puntuación del jugador.
RepNotify: PlayerColor en AWebsPlayerState - Actualiza el color asignado al personaje.
RepNotify: OccupyingPlayer en ACaptureZone - Actualiza qué jugador está ocupando una zona.
RepNotify: OwningPlayer en ACaptureZone - Actualiza qué jugador es propietario de una zona.
RepNotify: CurrentSilk en UWebsSilkComponent - Actualiza la cantidad de seda disponible y su representación en el HUD.
RepNotify: bIsAvailable en BP_SilkPickup - Actualiza la visibilidad y colisión de los recursos de seda al recolectarlos y reaparecer.
Server RPC
Server RPC: ServerCompleteCapture en AWebsPlayerController - El servidor valida el patrón dibujado, comprueba los recursos necesarios y procesa la captura.
Server RPC: ServerSetPlayerReady en AWebsPlayerController - El servidor registra que un jugador está listo para comenzar la partida.
Server RPC: ServerTryShoot en UWebsShootingComponent - El servidor valida el disparo, comprueba las condiciones necesarias y procesa sus efectos.
Client RPC
Client RPC: ClientOpenCapturePuzzle en AWebsPlayerController - Abre el minijuego de captura únicamente en la interfaz del cliente correspondiente.
Client RPC: ClientCaptureResult en AWebsPlayerController - Comunica al cliente el resultado de su intento de captura.
NetMulticast RPC
NetMulticast RPC: MulticastShotEffect en UWebsShootingComponent - Reproduce el vfx del disparo de seda para todos los jugadores conectados.

Cómo Probar el Proyecto
1. Abrir el mapa Content/Maps/Test_Level en Unreal Engine.
2. En las opciones de Play (PIE), configurar Number of Players = 2  y  Net Mode = Play As Listen Server.
3. Ejecutar la partida. El servidor esperará un tiempo configurable (setteado en 15 segundos) para permitir que se unan otros jugadores y luego iniciará la cuenta regresiva.
4. Controlar las arañas, recolectar seda, disparar a los oponentes y capturar zonas para acumular puntos.

Prueba con ejecutables: Para probar el proyecto usando la build de Windows en la carpeta de Drive se encuentran dos comprimidos:
- Trabajo_Final_PEV2_EloisaGleiser_Online: 
1. Ejecutar el .exe
2. Un jugador selecciona Online - Crear Partida
3. Los demás Buscar Partida y se unen a la correspondiente

- Trabajo_Final_PEV2_EloisaGleiser_LAN:
1. Ejecutar el .exe
2. Un jugador selecciona LAN- Hostear 
3. Los demás seleccionan LAN - Unirse, ingresando la dirección IP del servidor
