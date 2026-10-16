*This project was created as part of the 42 curriculum by vabad-ro.*

<!-- TODO: reemplaza <login1> por tu login (y los de tu equipo si los hay). -->

# Codexion

<!-- TODO: nombre del proyecto -->

---

## Description

<!-- TODO:
Objetivo del proyecto y breve visión general.
Sugerencia: la simulación (coders, dongles, ciclo compile→debug→refactor),
qué problema de concurrencia modela y qué se implementa (hilos, mutex,
planificadores FIFO/EDF, monitor de burnout).
-->

---

## Instructions

<!-- TODO:
Información relevante de compilación, instalación y/o ejecución.
Sugerencia:
- Requisitos (compilador cc, pthread, norminette...)
- Cómo compilar (make)
- Cómo ejecutar: ./codexion number_of_coders time_to_burnout time_to_compile
  time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
- Ejemplo de ejecución con salida esperada
-->

---

## Resources

<!-- TODO:
Referencias clásicas relacionadas con el tema (documentación, artículos, tutoriales...).
Sugerencia: man pages de pthread, problema de los filósofos comensales,
condiciones de Coffman, EDF, la Norm de 42...
Además: descripción del uso de IA — para qué tareas y en qué partes del proyecto se ha utilizado.
-->

---

## Blocking cases handled

<!-- TODO:
Describir todos los problemas de concurrencia gestionados en la solución:
- Prevención de interbloqueos y condiciones de Coffman
- Prevención de inanición (starvation)
- Gestión del cooldown
- Detección precisa del agotamiento (burnout ≤ 10 ms)
- Serialización del log
-->

---

## Thread synchronization mechanisms

<!-- TODO:
Explicar las primitivas de hilos usadas en la implementación:
- pthread_mutex_t
- pthread_cond_t
- Implementación personalizada de eventos (si aplica)
Cómo coordinan el acceso a los recursos compartidos (dongles, logging,
estado del monitor), con ejemplos de cómo se evitan las condiciones de
carrera y cómo se logra comunicación thread-safe entre las personas que
programan y el monitor.
-->
