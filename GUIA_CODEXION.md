# GUÍA UNIVERSITARIA: Codexion

## Una clase paso a paso para entender y construir el proyecto

**Duración estimada:** 12–18 horas (repartidas en varios días; no lo hagas de golpe)

> **Cómo usar esta guía.** Esto es un **mapa**, no la solución. Léela, piensa, y **escribe tú el código**.
> En cada lección tienes: qué estudiar, qué tener en cuenta, preguntas para ti y un *checkpoint*
> (“no pases hasta saber hacer X”). Cada vez que dudes, pregúntame a mí o a un compañero.
> Recuerda las reglas del subject (`AGENTS.md`): Norm, sin globales, sin libft, funciones autorizadas,
> `-Wall -Wextra -Werror -pthread`, sin leaks.
> Este índice **no es definitivo**: lo iremos ajustando sobre la marcha.

---

## ⭐ DÓNDE ESTAMOS AHORA (actualizado 2026-10-21)

> **Esta es la sección que te dice qué hacer a continuación. Empieza siempre por aquí.**

| # | Hito | Estado | Qué falta |
|---|---|---|---|
| — | Estructura de carpetas + Makefile + header | ✅ **Hecho** | nada |
| M0 | Compila, norminette y sin relink | ✅ **Hecho** | nada |
| M1 | Parsing y validación de argumentos | ✅ **Hecho** | *(opcional)* overflow aparcado: `99999999999` cuela |
| M2 | Estado compartido e inicialización | ✅ **Hecho** | nada |
| M3 | Hilos y ciclo del coder | ✅ **Hecho** | nada (interleaving real llega en M4) |
| M4 | Adquirir/soltar dongles (mutex + cond) | 🔴 **EMPEZADO — NO COMPILA** | 2 errores de compilación + `take_dongle`/`release_dongle` por escribir |
| M5 | Cooldown | ⬜ Pendiente | — |
| M6 | Heap y planificación FIFO | ⬜ Pendiente | — |
| M7 | Planificación EDF | ⬜ Pendiente | — |
| M8 | Monitor y detección de burnout | ⬜ Pendiente | — |
| M9 | Log serializado | ⬜ Pendiente | — |
| M10 | Terminación y limpieza (sin leaks) | ⬜ Pendiente | — |
| V | Verificación (norma, leaks, tests, README) | ⬜ Pendiente | — |
| D | Defensa (recode + preguntas) | ⬜ Pendiente | — |

**En una frase:** el programa **valida argumentos, reserva todo el estado, lanza `n` hilos coder
que hacen el ciclo compile→debug→refactor, los espera y limpia**. Ahora toca que **compitan por
los dongles**, que es donde aparecen los problemas de verdad (M4).

### ✅ Cerrado en M3

`make re` ✅, `norminette .` ✅ (5/5 archivos), valgrind ✅ (0 leaks, 7 allocs/7 frees).
Los 4 hilos salen alineados por fase — **no es un bug**: nada los frena todavía, así que van al
mismo ritmo. El entrelazado real se prueba en M4, cuando los dongles hagan esperar a unos y a otros.

### 🔧 Lo que falla AHORA MISMO

**El proyecto NO compila.** M4 está empezado a medias: añadiste `pthread_cond_t cond` a
`t_dongle`, abriste `take_dongle`/`release_dongle` en `coder.c` y empezaste a tocar `init.c`,
pero quedaron tres cosas sin cerrar.

Esto es literalmente lo que dice el compilador ahora mismo:

```
init.c:26:21: error: too few arguments to function call, expected 2, have 0
                pthread_cond_init();
init.c:82:29: error: expected identifier
        pthread_cond_destroy(&sim->);
```

| # | Dónde | Qué pasa | Lo que debería ser |
|---|---|---|---|
| 1 | `init.c:26` | `pthread_cond_init()` sin argumentos: hay que decir **qué** condición se inicializa | `pthread_cond_init(&sim->dongles[i].cond, NULL);` — dentro del bucle, con `&` porque `cond` es un **objeto**, no un puntero |
| 2 | `init.c:82` | `pthread_cond_destroy(&sim->)` apunta a la nada: en `t_sim` **no hay** `cond`, está dentro de cada `t_dongle` | `pthread_cond_destroy(&sim->dongles[i].cond);` — **dentro del bucle** de los dongles, justo al lado del `pthread_mutex_destroy` |
| 3 | `coder.c:15` | `take_dongle(t_sim sim, t_coder coder)`: pasa structs **por valor** y el 2º parámetro es el coder, cuando la función necesita **el dongle** | `take_dongle(t_sim *sim, t_dongle *dongle)` (y `release_dongle` lo mismo). Ver «Mapa de variables» |

Y un cuarto, **latente**: todavía no ha petado porque el camino de error no se ha ejecutado.

| # | Dónde | Qué pasa | Lo que debería ser |
|---|---|---|---|
| 4 | `main.c:41` y `main.c:58` | **Doble `cleanup`**: si un `pthread_create` falla, `run_sim` llama a `cleanup(sim)` y luego `main` vuelve a llamar a `cleanup(&sim)`. Como `cleanup` pone `dongles = NULL`, la segunda vez hace `pthread_mutex_destroy(&sim->dongles[i]...)` sobre `NULL` → **segfault** | que `cleanup` se llame **una sola vez**: desde `main` siempre, y `run_sim` solo avisa del fallo (por ejemplo devolviendo `int`) |

> **Regla del día:** el primer objetivo es que `make re` vuelva a estar verde. No escribas
> una línea nueva hasta que los errores estén arreglados. Arreglar errores de compilación
> **es** el trabajo del hito, no una pausa del trabajo.

### ⚠️ Deudas anotadas (no bloquean, pero no se olvidan)

| Deuda | Dónde | Cuándo se ataca |
|---|---|---|
| Overflow en el parseo (`99999999999` cuela) | `parser.c` `check_range` | al final, si sobra tiempo (`safe_atoi`) |
| `printf` sin formato del subject en `coder.c` | `coder.c` | **M9** (log serializado) |
| `while (compile_times < required)` en el coder | `coder.c` | **M8** (lo decide el monitor, no el coder) |
| `init` llama a `cleanup` con mutexes no inicializados si falla un `malloc` | `init.c` | cuando se toque `init` de nuevo |
| **Doble `cleanup`** (segfault latente, aún sin ejecutar) | `main.c` `run_sim` + `main` | **M4** — hoy mismo |
| No se comprueba el retorno de `pthread_create` en producción | `main.c` | ya hecho en el camino de error ✅ |

### Regla para avanzar

1. Abre la lección del hito que toca (abajo está el enlace).
2. Léela entera **antes** de escribir código.
3. Escribe el código tú; si algo no cabe en la Norm, es señal de que la función hace demasiado → pártela.
4. Antes de marcar ✅: `make` limpio + `norminette coders/` a 0 + la prueba del checkpoint.
5. Commit pequeño y marca la casilla en la tabla de arriba.

### Índice → qué lección mirar según el hito

| Hito | Lección |
|---|---|
| M0 | [Lección 17](#lección-17-m0--estructura-makefile-y-norm) |
| M1 | [Lección 18](#lección-18-m1--parsing-y-validación) |
| M2 | [Lección 19](#lección-19-m2--estado-compartido-e-inicialización) |
| M3 | [Lección 20](#lección-20-m3--hilos-y-el-ciclo-del-coder) |
| M4 | [Lección 21](#lección-21-m4--adquirirsoltar-dongles-mutex--condition-variable) ← **estás aquí** |
| M5 | [Lección 22](#lección-22-m5--cooldown) |
| M6 | [Lección 23](#lección-23-m6--el-heap-y-la-planificación-fifo) |
| M7 | [Lección 24](#lección-24-m7--planificación-edf) |
| M8 | [Lección 25](#lección-25-m8--el-monitor-y-la-detección-de-burnout) |
| M9 | [Lección 26](#lección-26-m9--log-serializado) |
| M10 | [Lección 27](#lección-27-m10--terminación-y-limpieza-sin-leaks) |

---

## Índice

**PARTE 0 — Antes de escribir código**
- Lección 0: Mentalidad y herramientas

**PARTE I — Los fundamentos (concurrencia)**
- Lección 1: ¿Qué es un hilo? Proceso vs. hilo
- Lección 2: Condiciones de carrera y mutex
- Lección 3: Variables de condición (esperar sin quemar CPU)
- Lección 4: Interbloqueo (deadlock) y condiciones de Coffman
- Lección 5: Inanición (starvation) y equidad
- Lección 6: El tiempo: `gettimeofday`, `usleep` y precisión
- Lección 6.5: Arrays, memoria dinámica y el bucle que recorre *(añadida tras M2)*

**PARTE II — Entender el problema Codexion**
- Lección 7: El modelo (coders, estados, dongles)
- Lección 8: Los 8 argumentos y su validación
- Lección 9: El log y su formato (serialización)
- Lección 10: El cooldown del dongle
- Lección 11: Burnout y deadlines
- Lección 12: Los planificadores: FIFO y EDF
- Lección 13: La cola de prioridad (heap) propia

**PARTE III — Diseño de la arquitectura**
- Lección 14: Estructuras de datos y organización de archivos
- Lección 15: El monitor como observador
- Lección 16: Diseñar dos veces (módulos profundos)

**PARTE IV — Manos a la obra (por hitos)** · *(✅ = hecho, 🔴 = en curso, ⬜ = pendiente)*
- Lección 17: M0 — Estructura, Makefile y Norm · **✅**
- Lección 18: M1 — Parsing y validación · **✅**
- Lección 19: M2 — Estado compartido e inicialización · **✅**
- Lección 20: M3 — Hilos y el ciclo del coder · **✅**
- Lección 21: M4 — Adquirir/soltar dongles (mutex + cond) · 🔴 **EN CURSO**
- Lección 22: M5 — Cooldown · ⬜
- Lección 23: M6 — El heap y la planificación FIFO · ⬜
- Lección 24: M7 — Planificación EDF · ⬜
- Lección 25: M8 — El monitor y la detección de burnout · ⬜
- Lección 26: M9 — Log serializado · ⬜
- Lección 27: M10 — Terminación y limpieza (sin leaks) · ⬜

**PARTE V — Verificación**
- Lección 28: Norminette y estilo
- Lección 29: Leaks y sanitizers
- Lección 30: Pruebas y casos límite
- Lección 31: El README

**PARTE VI — Defensa**
- Lección 32: La recode
- Lección 33: Preguntas típicas de evaluación

**ANEXO**
- Glosario
- Checklist final
- Mapa del subject → dónde se cumple
- Recursos para estudiar

---

# PARTE 0: ANTES DE ESCRIBIR CÓDIGO

## LECCIÓN 0: Mentalidad y herramientas

### 0.1. Una frase que resume el proyecto

Todo Codexion cabe en una idea: **muchos hilos compitiendo por recursos escasos, y tú eres el árbitro**.
Los “dongles” son el recurso escaso; el “monitor” es el árbitro; el “cooldown” y el “planificador” son las reglas del juego.

La dificultad no está en escribir mucho código, sino en escribir **poco código que sea correcto a la vez**. La concurrencia castiga la improvisación: un fallo aparece 1 de cada 10.000 ejecuciones.

### 0.2. Herramientas que vas a usar (instálalas y pruébalas ya)

| Herramienta | Para qué | Cómo se usa |
|---|---|---|
| `cc` (gcc/clang) | compilar | `cc -Wall -Wextra -Werror -pthread *.c` |
| `norminette` | comprobar la Norm | `norminette coders/` |
| `valgrind` | fugas de memoria | `valgrind --leak-check=full ./codexion ...` |
| `valgrind --tool=helgrind` | condiciones de carrera | `valgrind --tool=helgrind ./codexion ...` |
| `-fsanitize=thread` | detectar *data races* | compilar con extra y ejecutar |
| `git` | guardar versiones | commit pequeño tras cada hito |

Si faltan, se instalan así:

```bash
uv tool install norminette        # norminette, sin romper el python del sistema
sudo apt-get install valgrind     # valgrind (pide contraseña)
```

> **Aviso real (pasó en M2):** las extensiones "42 formatter" de VS Code reformatean tu código al
> guardar con reglas **distintas** al norminette real (quitaban los espacios de `a * b`, que la Norm
> exige). Desinstálalas o desactiva `formatOnSave`. El juez es la CLI: `norminette coders/`.

**El bucle de trabajo que funciona** (probado en M2): escribes tu versión → la pegas y razonas las
dudas → compuebas con `make` y `norminette` **tras cada cambio pequeño**, no al final. El compilador
y el norminette son revisores gratis que no se cansan.

### 0.3. Cómo pedir ayuda sin hacer trampa

El subject es claro: puedes usar IA para tareas repetitivas, pero **solo puedes entregar lo que entiendes**.
Regla personal: si no puedes explicar una línea, no la entregas. Usa esta guía para estudiar y a mí para
preguntarme *por qué*, no para que te escriba el programa.

### Checkpoint

- [ ] Sé compilar un “hola mundo” con `-pthread` y ejecutarlo.
- [ ] Sé qué hace `norminette` y `valgrind`.
- [ ] Tengo claro que **no puedo** usar variables globales ni libft.

---

# PARTE I: LOS FUNDAMENTOS

> Antes de tocar Codexion, entiende cinco conceptos. Sin ellos, el código es magia.

## LECCIÓN 1: ¿Qué es un hilo? Proceso vs. hilo

### 1.1. Analogía

- Un **proceso** es un edificio: tiene su propia dirección, sus propias tuberías, su propio almacén.
- Un **hilo** es una persona dentro del edificio. Varias personas comparten el almacén y las tuberías
  (la **memoria**), pero cada una tiene su propio escritorio (su pila, *stack*) y sus notas privadas.

Consecuencia clave: dos hilos que usan **la misma variable** se pisan. Dos hilos con **variables locales**
van a lo suyo.

### 1.2. En C

- `pthread_create(&t, NULL, función, argumento)` → nace un hilo que ejecuta `función(argumento)`.
- `pthread_join(t, NULL)` → el hilo principal espera a que `t` termine.
- El **argumento** es un `void *`: normalmente le pasas un puntero a una estructura con todo lo que el hilo necesita.

La función de un hilo tiene **firma fija**: `void *mi_hilo(void *arg)`. Dentro casteas el argumento
(`t_coder *c = arg;`) y devuelves `NULL` al terminar.

```c
pthread_t   t;
t_coder     c;

pthread_create(&t, NULL, coder_loop, &c);  // coder_loop corre EN PARALELO con main
pthread_join(t, NULL);                      // main se bloquea hasta que coder_loop acabe
```

Dos detalles: `pthread_create` devuelve 0 si fue bien (hay que comprobarlo), y **no se puede hacer
`join` de un hilo que no se creó**.

### 1.3. Join, detach... y por qué el proceso muere con los hilos dentro

Un hilo no tiene vida propia: vive **dentro** del proceso. Cuando `main` hace `return 0`, el SO mata
al instante todos los hilos, aunque estén a medio `printf`. Ejecuta este experimento:

```c
void	*lenta(void *a)
{
	(void)a;
	sleep(2);
	printf("hilo: termine\n");
	return (NULL);
}

int	main(void)
{
	pthread_t	t;

	pthread_create(&t, NULL, lenta, NULL);
	return (0);   // sin join: "hilo: termine" NUNCA aparece
}
```

Hay dos formas de gestionar el final de un hilo, y se eligen con una sola pregunta:
**¿necesito esperar a que acabe?**

| | `pthread_join(t, NULL)` | `pthread_detach(t)` |
|---|---|---|
| ¿Main espera? | Sí, se bloquea | No |
| ¿Sabes cuándo terminó? | Sí | No |
| ¿Puedes hacer join luego? | Sí | No (es error) |
| Recursos del hilo | Los recoge el join | Los recoge el SO |
| Útil para | "nadie apaga las luces hasta que salga el último" | tareas sueltas sin orden |

Codexion usa **join**: `main` debe esperar a que todos los hilos terminen antes de `cleanup`
(destruir mutex con hilos vivos que los usan = desastre). Patrón para todo el proyecto:
**crear todos → join de todos → cleanup**.

En Codexion: **cada coder es un hilo**, y hay **un hilo extra: el monitor**.

### Qué tener en cuenta

- Sin `pthread_join`, el `main` puede terminar y matar los hilos antes de que hagan nada.
- `pthread_create` devuelve un código de error: hay que comprobarlo.
- Como no hay variables globales, cada hilo recibe un puntero a una **estructura compartida** (“la mesa”).

### Preguntas para ti

1. ¿Por qué dos hilos que incrementan la misma variable a la vez dan un resultado “imposible”?
2. ¿Qué se comparte entre hilos y qué no?

### Checkpoint

- [ ] Sé explicar con mis palabras la diferencia entre pila (privada) y memoria compartida.

---

## LECCIÓN 2: Condiciones de carrera y mutex

### 2.1. La carrera

Si dos hilos hacen `contador = contador + 1` a la vez, la operación no es atómica:
leer → sumar → escribir. Los interleaveados dan pérdidas. Eso es una **condición de carrera** (*race condition*).

### 2.2. Mutex

Un **mutex** (`pthread_mutex_t`) es un candado: solo un hilo puede tenerlo a la vez.

- `pthread_mutex_lock(&m)` → cierra con llave (si está ocupado, espera).
- `pthread_mutex_unlock(&m)` → abre.
- `pthread_mutex_init` / `pthread_mutex_destroy` → crear/destruir.

Regla de oro: **sección crítica pequeña**. Coge el mutex, haz el mínimo, suelta. No hagas `usleep` con el mutex cogido.

### 2.3. La carrera en vivo (experimento que puedes ejecutar)

5 hilos sumando 200.000 veces cada uno a la misma variable, **sin mutex**:

```c
void	*trabajo(void *a)
{
	int	i;

	(void)a;
	i = 0;
	while (i < 200000)
	{
		compartida = compartida + 1;
		i++;
	}
	return (NULL);
}
```

Salida real: `esperado: 1000000 | real: 317101`. Se pierden ~700.000 sumas. ¿Por qué?
`compartida + 1` son 3 pasos (leer → sumar → escribir) y el SO intercambia entre hilos a mitad:
A lee 1000, B lee 1000, A escribe 1001, B escribe 1001 → una suma desaparece.

El mismo programa con `lock` → `compartida++` → `unlock` da **1.000.000 siempre**.

En Codexion, esto es exactamente lo que pasaría si dos coders "leen" el mismo dongle libre a la vez:
ambos creerían que se lo llevan. El mutex es lo que impide la doble lectura.

### Qué tener en cuenta

- El **estado de cada dongle** (libre / ocupado / en cooldown) es un dato compartido → necesita mutex.
- El **log** también es un recurso compartido: sin mutex, dos mensajes se mezclan en la misma línea.
- Olvidar un `unlock` en un camino de error = todo se congela.

### Checkpoint

- [ ] Sé señalar qué datos de Codexion necesitan mutex y por qué.

---

## LECCIÓN 3: Variables de condición (esperar sin quemar CPU)

### 3.1. El problema

Un coder quiere un dongle que está ocupado. Opciones malas:

- **Busy-wait**: preguntar en bucle “¿ya está?” → quema CPU y es feo.
- **Dormir a ciegas**: `usleep` fijo → o te pasas o te quedas corto.

La solución es la **variable de condición** (`pthread_cond_t`): te duermes hasta que alguien te avise.

### 3.2. El patrón correcto (memorízalo)

```c
pthread_mutex_lock(&m);
while (condición_no_se_cumple)          // while, NO if
    pthread_cond_wait(&cv, &m);         // suelta m y duerme; al despertar recupera m
/* ... sección crítica ... */
pthread_mutex_unlock(&m);
```

Y el que cambia el estado hace:
```c
pthread_mutex_lock(&m);
estado = listo;
pthread_cond_broadcast(&cv);            // o signal
pthread_mutex_unlock(&m);
```

### 3.3. La variante que necesitarás

`pthread_cond_timedwait(&cv, &m, &deadline)` despierta solo: **como máximo** hasta el tiempo indicado.
Te hará falta para no dormir indefinidamente cuando hay un *deadline* que vigilar.

### Qué tener en cuenta

- `while`, nunca `if`: pueden despertarte por “falsos despertares”.
- `pthread_cond_timedwait` usa un **tiempo absoluto** (desde la época Unix), no “5 ms desde ahora”.

### Checkpoint

- [ ] Sé escribir el patrón productor/consumidor con `while` + `cond_wait`.

---

## LECCIÓN 4: Interbloqueo (deadlock) y condiciones de Coffman

### 4.1. El problema clásico

Dos coders necesitan **dos** dongles. Si el coder 1 coge el izquierdo y el coder 2 coge el que le queda
a la derecha de 1… y cada uno espera al otro, **nadie avanza jamás**. Eso es *deadlock*.

### 4.2. Las cuatro condiciones de Coffman (las 4 se deben dar a la vez)

1. **Exclusión mutua**: el recurso no se puede compartir.
2. **Retención y espera**: tienes uno y esperas otro.
3. **Sin expropiación**: nadie te quita el dongle.
4. **Espera circular**: A espera a B, B espera a A.

Para evitar el deadlock, **rompe al menos una**. En Codexion la forma típica es romper la circularidad con
un **árbitro central** (la mesa decide quién coge los dos dongles) o adquiriendo **siempre en el mismo orden**.

### Qué tener en cuenta

- El README pide una sección **“Blocking cases handled”**: aquí es donde justificas cómo evitas Coffman.
- Un *deadlock* no da error: simplemente el programa **se queda colgado**. Aprende a “leer” un cuelgue.

### Preguntas para ti

1. Si todos cogieran primero el dongle izquierdo, ¿qué condición de Coffman se cumple y por qué colgaría?

### Checkpoint

- [ ] Sé explicar mi estrategia anti-deadlock y qué condición de Coffman rompe.

---

## LECCIÓN 5: Inanición (starvation) y equidad

### 5.1. Justicia

Si el dongle siempre se lo lleva “el más rápido”, un coder lento **nunca compila** → su deadline vence → burnout.
Eso es **inanición**.

La solución es el **planificador**: cuando varios piden a la vez, la mesa decide **por reglas**, no por suerte.

- **FIFO**: el que pidió primero, primero. Sencillo y justo “por turno”.
- **EDF**: el que tiene el deadline de burnout más cercano, primero. Justo “por urgencia”.

### Qué tener en cuenta

- Sin una **cola de espera** por dongle, no puedes decidir “quién va primero”: cada petición necesita
  guardar *a quién* pertenece y *cuándo* llegó (o *qué deadline* tiene).
- Con `edf` el subject exige que **nadie se agote** si los parámetros son viables.

### Checkpoint

- [ ] Sé decir qué información debe guardar cada solicitud para poder ordenarla.

---

## LECCIÓN 6: El tiempo: `gettimeofday`, `usleep` y precisión

### 6.1. Medir y esperar

- `gettimeofday` → **mira la hora** actual en milisegundos. Sirve para: el `timestamp_in_ms` del log,
  calcular deadlines y comprobar cooldowns.
- `usleep` → **duerme** el hilo un número de microsegundos (¡ojo: micro, no mili!).

### 6.2. El problema de la precisión

`usleep(1000)` **no** duerme exactamente 1 ms: el sistema operativo puede tardar más. Por eso:

- El burnout debe loguearse **dentro de los 10 ms** posteriores al momento real.
- Por eso existe un **monitor separado**: vigila continuamente los deadlines y actúa en cuanto se cumplen,
  en vez de confiar en que un `usleep` sea exacto.

### Qué tener en cuenta

- Un solo reloj coherente: usa `gettimeofday` en TODAS las medidas (no mezcles fuentes).
- Convierte `struct timeval` (seg, µs) a milisegundos: `tv_sec*1000 + tv_usec/1000`.
- `usleep` con valor 0 es válido; `usleep` enorme = hilo muerto.

### Checkpoint

- [ ] Sé convertir un `struct timeval` a milisegundos y comparar dos tiempos.

---

## LECCIÓN 6.5: Arrays, memoria dinámica y el bucle que recorre

> **Añadida tras M2**, porque todos los bugs reales de ese hito vinieron de aquí. Léela despacio:
> son 20 minutos que te ahorran horas.

### 6.5.1. ¿Por qué un array y no variables sueltas?

El número de coders lo decide el usuario al arrancar: `./codexion 7 ...` o `./codexion 50 ...`.
En C, las variables se declaran con tamaño fijo **al compilar**: no puedes escribir
`t_coder c1, c2, ..., cN` porque N no la sabes. La única respuesta es **reservar memoria en
tiempo de ejecución**:

```c
sim->dongles = malloc(sizeof(t_dongle) * sim->n_coders);
```

Eso crea **un bloque contiguo** de n posiciones y devuelve su dirección:

```
sim->dongles ──▶ [ dongle 0 ][ dongle 1 ][ dongle 2 ]
                   índice 0    índice 1    índice 2
```

Un bloque = **una sola reserva = un solo `free`** al final. Los "elementos" no existen aparte:
son posiciones dentro del bloque, y se acceden con `sim->dongles[i]`.

### 6.5.2. La regla de oro: reservar una vez, rellenar n veces

El error más repetido de M2 fue meter el `malloc` **dentro** del bucle:

```c
while (i < n)
{
	sim->dongles = malloc(...);   // ❌ CREA UN BLOQUE NUEVO en cada vuelta
	...
	i++;
}
```

Traza con `n = 3`: se crean 3 arrays; los 2 primeros quedan **huérfanos** (leak); el puntero solo
guarda el último. Y solo rellenas un elemento por vuelta, del bloque que acabas de tirar.

```c
sim->dongles = malloc(...);       // ✅ UNA reserva, fuera del bucle
while (i < n)
{
	/* rellena dongles[i] */      // ✅ el bucle SOLO rellena
	i++;
}
```

**Mentalidad:** el `malloc` **crea** el bloque entero de golpe; el bucle **recorre** poniendo cada
posición a su valor. Crear n veces = perder n-1 bloques.

### 6.5.3. Índices de 0 a n-1 (y el truco del `%`)

Los índices de un array van de `0` a `n - 1`. Un bucle con `i = 1` deja el elemento **0 con
basura** — y con `n = 1`, `while (i < 1)` **no se ejecuta ni una vez** (caso especial obligatorio
del subject: una persona, un dongle, y quedaría sin inicializar).

Para dar la vuelta a la mesa circular se usa el **resto de la división** (`%`). Con `n = 4`:

| i | i+1 | (i+1) % 4 |
|---|-----|-----------|
| 0 | 1 | 1 |
| 1 | 2 | 2 |
| 2 | 3 | 3 |
| 3 | 4 | **0** ← el último se empareja con el primero |

Con `n = 1`: `(0 + 1) % 1 = 0` → `left == right == &dongles[0]`. Correcto según el subject.

> ⚠️ **Pero ojo (lo verás en M4):** si `left == right`, hacer `take_dongle(left)` **y después**
> `take_dongle(right)` es tomar **el mismo dongle dos veces** → el segundo `take_dongle` se queda
> esperando al primero → **cuelgue eterno**. Con `n = 1` se coge **una sola vez**.

### 6.5.4. Índice ≠ id

Dos números que se confunden fácilmente:

- `i` = posición en memoria, **0-based** (`0 .. n-1`).
- `coder_id` = el número que imprime el log, **1-based** (`1 .. n`). Por eso: `coder_id = i + 1`.

Si empiezas el bucle en `i = 1` "para que el id empiece en 1", pierdes el elemento 0 y los ids
salen desplazados (`2, 3, 4, 5`). El `+1` va **dentro de la asignación**, no en el índice.

### 6.5.5. malloc vs calloc (da igual aquí)

`malloc` deja basura; `calloc` deja ceros. Pero en Codexion **todos** los campos se inicializan a
mano (`is_free = 1` no cabe en un cero; el mutex necesita `pthread_mutex_init`; `coder_id` es
`i + 1`), así que el ahorro del `calloc` son tres líneas y **oculta** la inicialización: si mañana
añades un campo y te olvidas de él, con `calloc` "funciona" hasta que no. Elige `malloc` + init
explícito.

### 6.5.6. Cuatro trampas de sintaxis que causaron bugs reales

1. **`if (!init_dongles)` sin paréntesis** no ejecuta nada: el nombre de una función en C es su
   dirección de memoria (siempre distinta de NULL) → el `if` es siempre falso. Necesitas
   `if (!init_dongles(sim))`. Regla: paréntesis = ejecutar; sin paréntesis = solo el puntero.
2. **`x == NULL` pregunta, `x = NULL` asigna.** `&x == NULL` no pone nada a NULL, solo compara y
   tira la respuesta.
3. **`sim->dongles[i].mutex;` es una sentencia sin efecto**: lee el mutex y descarta el valor.
   Si querías crearlo, falta `pthread_mutex_init(...)`. Si una línea no llama a una función ni
   asigna, normalmente no hace nada.
4. **Falta `return (1)` en un helper**: el llamante lee basura y actúa en base a ella. Toda ruta
   de éxito debe devolver su valor.

### 6.5.7. El orden de `cleanup` (post-mortem de M2)

`cleanup` es el espejo de `init` y se divide en tres zonas con **orden fijo**:

```
1. bucle:   destroy de cada dongles[i].mutex    (usa el array)
2. únicos:  destroy de stop_mutex y log_mutex
3. memoria: free(dongles); free(coders);   y SOLO ENTONCES   = NULL
```

Trampas que ya fallaron una vez:

- `free` **dentro** del bucle → destruyes mutex sobre memoria liberada y haces double free.
- `= NULL` **dentro** del bucle → la vuelta siguiente desreferencia NULL (segfault) y los `free`
  posteriores son `free(NULL)` (no libran nada → leak). El `NULL` es el **cierre de puertas**:
  al final, después de todo el trabajo.
- `free` elemento a elemento: **no**. Un array = **un solo `free`**.

### Checkpoint

- [ ] Sé dibujar el bloque de memoria de `dongles`/`coders` y las flechas `left`/`right`.
- [ ] Sé decir por qué el `malloc` va fuera del bucle y el `free` también.
- [ ] Con `n = 1`, sé predecir qué hace cada bucle de tu `init` **sin ejecutarlo**.

---

# PARTE II: ENTENDER EL PROBLEMA CODEXION

## LECCIÓN 7: El modelo (coders, estados, dongles)

### 7.1. La historia

Hay `n` personas en un hub circular. En el centro, un compilador cuántico. Sobre la mesa, **`n` dongles USB**
(hay tantos dongles como personas). Para **compilar** hacen falta **dos dongles**: el de tu izquierda y el de tu derecha.

### 7.2. El ciclo de vida de una persona

```
        ┌──────────────────────────────────────────┐
        │                                          │
   (intenta compilar) → compile → debug → refactor ┘
        necesita 2 dongles        t_debug    t_refactor
```

- Mientras compila, no depura ni refactoriza (y viceversa).
- Al terminar de refactorizar, **vuelve a intentar compilar inmediatamente**.

### 7.3. La disposición circular

- Personas numeradas `1..n`. La persona `1` se sienta junto a la `n`.
- La persona `N` está entre `N-1` y `N+1`. Cada una tiene un dongle a cada lado → `n` dongles.
- Caso especial: si `n = 1`, hay **un solo** dongle sobre la mesa.

### Qué tener en cuenta

- “Izquierdo” y “derecho” deben ser deterministas: define una convención (p. ej. coder `i` coge `i` y `(i+1) % n`)
  y no la cambies. Aquí nacen la mayoría de los deadlocks.
- El estado de cada persona cambia y **cada cambio se loguea**.

### Checkpoint

- [ ] Sé dibujar en papel la mesa con 4 personas, sus 4 dongles y quién toca qué dongle.

---

## LECCIÓN 8: Los 8 argumentos y su validación

### 8.1. La lista (todas obligatorias)

```
number_of_coders  time_to_burnout  time_to_compile  time_to_debug
time_to_refactor  number_of_compiles_required  dongle_cooldown  scheduler
```

| Argumento | Significado |
|---|---|
| `number_of_coders` | nº de personas **y** nº de dongles |
| `time_to_burnout` | ms sin empezar a compilar antes de agotarse |
| `time_to_compile` | ms compilando (sujetando 2 dongles) |
| `time_to_debug` | ms depurando |
| `time_to_refactor` | ms refactorizando |
| `number_of_compiles_required` | si todos compilan ≥ este nº, fin |
| `dongle_cooldown` | ms que un dongle queda inutilizable tras soltarlo |
| `scheduler` | `fifo` o `edf` (exactamente) |

### 8.2. Qué hay que rechazar

- Números **negativos**.
- Valores **no enteros** (p. ej. `12a`, `1.5`, vacío).
- `scheduler` distinto de `fifo` / `edf`.
- Faltan argumentos (distinto de 9 en total, contando el argv[0] del programa).

### Qué tener en cuenta

- Debes **usar** `atoi` y `strcmp` (están autorizadas) — pero `atoi` no detecta basura: revísalo.
- Un `number_of_coders` de 0 probablemente sea inválido: ¿por qué?
- Rechazar = mensaje de error y salida limpia (código ≠ 0), **sin** fugas.

### Checkpoint

- [ ] Sé listar 5 entradas que debo rechazar y cómo lo detecto.

---

## LECCIÓN 9: El log y su formato (serialización)

### 9.1. El formato exacto (no te lo inventes)

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Ejemplo:
```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
```

### 9.2. Serialización

“Serializado” = **cada mensaje se escribe entero, sin que otro se cuele en medio**. Con varios hilos
escribiendo, sin protección obtendrías basura tipo `1 1 is comp0 2 has t...`. Solución: un **mutex de log**.

### Qué tener en cuenta

- Un `printf` de una sola llamada con formato completo es más fácil de serializar que varios `printf` sueltos.
- El log de **burnout** debe salir ≤ 10 ms después del agotamiento real.
- Ojo con el orden: primero “has taken a dongle” (dos veces), luego “is compiling”.

### Checkpoint

- [ ] Sé explicar por qué dos `printf` seguidos pueden mezclarse entre hilos.

---

## LECCIÓN 10: El cooldown del dongle

### 10.1. Qué es

Tras soltar un dongle, no puede volver a cogerse hasta que pasen `dongle_cooldown` **milisegundos**.

### 10.2. Cómo modelarlo

Cada dongle guarda “hasta qué instante está en cooldown” (`cooldown_until`). Quien quiera cogerlo
comprueba si `ahora >= cooldown_until`. Si no, **espera** (usando `pthread_cond_timedwait` para despertar
justo cuando expire).

### Qué tener en cuenta

- El cooldown se aplica **al soltar**, no al coger.
- Si `dongle_cooldown` es 0, el dongle está disponible de inmediato.
- El monitor también puede vigilar si el cooldown hace imposible cumplir un deadline.

### Checkpoint

- [ ] Sé decidir si un dongle está disponible en un instante dado, con una sola comparación.

---

## LECCIÓN 11: Burnout y deadlines

### 11.1. La regla

Una persona se agota si **no empieza a compilar** dentro de `time_to_burnout` ms desde:
- el **inicio de la simulación**, o
- el **inicio de su última compilación**.

Dicho de otro modo: `deadline = last_compile_start + time_to_burnout`.

### 11.2. Quién lo detecta

El **hilo monitor**, que vigila continuamente: si `ahora > deadline` de alguien → “X burned out” y **parar la simulación**.

### Qué tener en cuenta

- La primera vez, `last_compile_start` no existe: el deadline es `inicio_simulación + time_to_burnout`.
- El deadline se **renueva** cada vez que empieza a compilar.
- Precisión: el mensaje debe salir ≤ 10 ms del momento real. De ahí el monitor dedicado.

### Checkpoint

- [ ] Sé calcular el deadline de un coder en cualquier instante de su vida.

---

## LECCIÓN 12: Los planificadores: FIFO y EDF

### 12.1. La decisión

Cuando **varias** personas piden **el mismo** dongle y queda libre, ¿quién lo coge?

- **fifo** → el que pidió **primero**.
- **edf** → el que tiene el **deadline de burnout más próximo** (`last_compile_start + time_to_burnout`).

### 12.2. Cómo se implementa la decisión

Cada dongle tiene una **cola de solicitudes**. Cuando queda libre, eliges una según la política:

- FIFO: cola por **orden de llegada**.
- EDF: cola por **deadline** (la más urgente primero).

### Qué tener en cuenta

- La política es **global** (viene en `argv`), pero cada dongle decide **entre sus propios solicitantes**.
- EDF exige que **nadie muera** con parámetros viables: es la política “segura”.
- Piénsalo como un **árbitro**: nadie coge un dongle por su cuenta sin pasar por el criterio.

### Checkpoint

- [ ] Con 3 peticiones simultáneas (deadlines 500, 200, 800), sé decir el orden en FIFO y en EDF.

---

## LECCIÓN 13: La cola de prioridad (heap) propia

### 13.1. Por qué una “cola de prioridad”

El subject pide explícitamente implementar un **heap** (montículo) para la planificación, porque C89 no
trae una cola de prioridad estándar.

- Un **min-heap** guarda elementos con una **clave** (en FIFO: el nº de llegada; en EDF: el deadline).
- Da siempre el **mínimo** en `O(1)` y lo inserta/extrae en `O(log n)`.

### 13.2. La idea (sin código)

Un heap es un **árbol binario completo** guardado en un array. Regla: el padre ≤ sus hijos.
Insertar y extraer “suben/bajan” el elemento hasta recolocarlo (`sift up` / `sift down`).

### Qué tener en cuenta

- Necesitas poder **comparar** con dos criterios (llegada o deadline). Un truco limpio: un campo `priority`
  que rellenas según el scheduler.
- También suele hacer falta **quitar un elemento concreto** (si un coder deja de querer el dongle).
- Ojo con los índices y con los tamaños al hacer `malloc`/`realloc`.

### Checkpoint

- [ ] Sé explicar en papel cómo insertar y extraer del heap, y qué orden da.

---

# PARTE III: DISEÑO DE LA ARQUITECTURA

## LECCIÓN 14: Estructuras de datos y organización de archivos

### 14.1. La gran decisión: nada de globales

Sin variables globales, **todo el estado** vive en una estructura y se pasa por puntero. Esquema típico
(de nombres, tú eliges):

- `t_sim`: la simulación entera (argumentos, estado, flags de parada, el log-mutex).
- `t_coder`: nº, hilo, `last_compile_start`, nº de compilaciones, punteros a sus dos dongles.
- `t_dongle`: mutex propio, estado (libre/cooldown), `cooldown_until`, cola de espera (heap).
- `t_request`: quién pide un dongle + su clave (llegada o deadline).

### 14.2. Archivos sugeridos (dentro de `coders/`)

No hay una única forma. Una organización razonable:

```
coders/
├── Makefile
├── codexion.h        (o varios .h)
├── main.c            arranque y join
├── parse.c           validación de argumentos
├── init.c            reserva e inicialización
├── dongle.c          coger/soltar, cooldown, cola
├── heap.c            cola de prioridad
├── coder.c           ciclo de cada persona
├── monitor.c         vigilancia de deadlines
├── log.c             escritura serializada
├── time.c            helpers de tiempo
└── cleanup.c         liberar todo
```

### Qué tener en cuenta

- La Norm limita (nombre de funciones, una función por responsabilidad, etc.): consulta el PDF y pasa `norminette` a menudo, no al final.
- Un solo sitio que crea/destruye cada recurso: si lo creas en `init.c`, destrúyelo en `cleanup.c`.

> **Esto es un esquema sugerido, no tu árbol actual.** Hoy el repo es más pequeño:
>
> | Existe ya | Falta escribir |
> |---|---|
> | `main.c`, `parser.c` (aquí aparece como `parse.c`), `init.c`, `coder.c`, `sim.c`, `log.c` | `dongle.c`, `heap.c`, `monitor.c`, `time.c`, `cleanup.c` |
>
> Además, `cleanup()` vive de momento en `init.c:70`, no en un `cleanup.c` propio, y
> `take_dongle()` / `release_dongle()` están en `coder.c`. No hace falta crear los
> ficheros vacíos por adelantado: los abres cuando toque (M4, M6, M7).

### Checkpoint

- [ ] Sé dibujar mis estructuras y qué campos lleva cada una, y quién los modifica.

---

## LECCIÓN 15: El monitor como observador

### 15.1. Qué es

Un **hilo más** cuyo único trabajo es **observar** deadlines y decidir cuándo se agota alguien.
No compila, no debuggea: vigila.

### 15.2. Por qué separado

- Detecta el burnout con precisión (≤ 10 ms).
- Puede revisar **a la vez** el estado de todos los coders.

### 15.3. Cómo se coordina

El monitor lee el estado compartido bajo los mutex correspondientes. Detectado un burnout: marca “parar”
y el resto de hilos lo ven (con su mutex / cond) y salen.

### Qué tener en cuenta

- Debe leer `last_compile_start` de forma segura (mutex), no a lo loco.
- Debe ser **rápido**: no puede dormir 100 ms; se despierta con frecuencia o con `cond_timedwait`.

### Checkpoint

- [ ] Sé explicar en qué se diferencia el monitor de los hilos coder.

---

## LECCIÓN 16: Diseñar dos veces (módulos profundos)

Antes de implementar la parte difícil (el arbitraje de dongles), **piensa dos diseños** y compáralos:

- **Diseño A**: cada coder intenta coger sus dos dongles por su cuenta (con mutex por dongle).
- **Diseño B**: un **árbitro central** (o la mesa) concede los pares de dongles según el planificador.

Compáralos en: simplicidad, riesgo de deadlock, facilidad para cumplir FIFO/EDF, testabilidad.

Pista de teoría de diseño (skill `codebase-design`): un **módulo profundo** es mucho comportamiento detrás
de una interfaz pequeña. ¿Qué interfaz mínima necesitaría tu “mesa”? Algo como
`pedir_dongles(coder)` / `soltar_dongles(coder)` esconde todo el arbitraje.

### Checkpoint

- [ ] He escrito en papel los pros y contras de A y B y he elegido uno **con un motivo**.

---

# PARTE IV: MANOS A LA OBRA (POR HITOS)

> Regla de cada hito: **compila, pasa norminette y no tiene leaks** antes de pasar al siguiente.
> Commitea al terminar cada uno.

## LECCIÓN 17: M0 — Estructura, Makefile y Norm

**Estado: ✅ HECHO.**

Crea `coders/`, tu `.h`, y el `Makefile` con las reglas `$(NAME) all clean fclean re`.
Flags: `-Wall -Wextra -Werror -pthread`, compilador `cc`, **sin relink**.

Qué se consiguió:
- `coders/Makefile`, `coders/codexion.h`, `coders/main.c`.
- `make`, `make re` y `make fclean` funcionan; el enlace usa `$(CFLAGS)` (incluye `-pthread`).
- Norminette limpio en los archivos que existen.

- [x] `make` produce `codexion`; `make re` no deja basura; `make fclean` limpia el binario.

---

## LECCIÓN 18: M1 — Parsing y validación

**Estado: ✅ HECHO.**

Implementa: contar argumentos, convertir a números, validar (negativos, no enteros, scheduler), y guardarlos
en tu estructura de simulación.

Qué se consiguió:

- `main` comprueba que hay exactamente 9 tokens (`argc == 9`).
- `check_number(char **args)` valida **formato** de los 7 argumentos numéricos: todos los
  caracteres deben ser dígitos y debe haber al menos uno (`j == 0` detecta la cadena vacía).
  Rechaza `12a`, `1.5`, `-4`, `abc` y `""`.
- `check_scheduler(char **args)` exige exactamente `fifo` o `edf`.
- `parse_args` orquesta las dos anteriores y devuelve 0/1; `main` sale con código ≠ 0 si falla.
- Mensajes de error por `stderr` con `\n`. Norminette limpio.

Lo que **queda fuera de M1** a propósito:

- El rechazo por **valor** (p. ej. `number_of_coders == 0`) se decidirá al construir la `t_sim` (M2).
- El **desbordamiento** (`99999999999` pasa el formato) se detectará al convertir, no en `check_number`.
- Los números todavía **no se guardan**: no hay struct hasta M2.

- [x] Rechazo entradas inválidas sin leaks (`12a`, `""`, `-4`, `fifoX`, faltan args → `exit 1`).
- [ ] Guardo todos los parámetros en mi struct (**esto es parte de M2**).

---

## LECCIÓN 19: M2 — Estado compartido e inicialización

**Estado: ✅ HECHO.**

Reserva el array de dongles y de coders, inicializa mutex y cond de cada dongle, crea el mutex del log.
Define la convención izquierda/derecha.

Qué se consiguió (commits `f06142a`..`d1880cd`):

- `init.c` con `init_dongles`, `init_coders` (helpers `static`), `init` y `cleanup`.
- `malloc` **una vez** por array, bucles `i = 0` rellenan cada elemento (id 1..n, `left = dongles[i]`,
  `right = dongles[(i + 1) % n]` → el `%` da la vuelta a la mesa circular; con `n = 1`, left == right).
- Mutex por dongle + `stop_mutex` + `log_mutex` creados en `init`, destruidos en `cleanup`
  **antes** de los `free`, y `= NULL` al final.
- `main` llama a `parse_args → init → cleanup`; valgrind: `All heap blocks were freed`.
- Norminette OK en los 4 archivos. Herramientas instaladas en el equipo: `norminette` (uv) y `valgrind` (apt).

- [x] `init` crea todo y `cleanup` lo destruye simétricamente (valgrind limpio).

---

## LECCIÓN 20: M3 — Hilos y el ciclo del coder

**Estado: ✅ HECHO.**

Crea `n` hilos coder + 1 monitor. Cada coder, por ahora **sin dongles reales**, hace un ciclo
simple imprimiendo estados (con `usleep`), para verificar que arrancan y terminan bien.

**Lectura previa obligatoria:** Lección 1.2 y 1.3 (crear, esperar y por qué `join`).
Los conceptos ya los tienes: esto es cablearlos.

### Lo que ya está hecho

- `coder.c` con `coder_loop`: bucle compile→debug→refactor con `usleep`.
- `run_sim` en `main.c`: crea `n` hilos en un bucle, `join_threads` los espera.
- Comprobación del return de `pthread_create` en el camino de error (`!= 0`), con `join` de los
  hilos vivos antes de `cleanup`.
- `coder.c` añadido al `SRC` del Makefile.
- `make re` ✅, `norminette .` ✅, valgrind ✅ (0 leaks).

### Decisiones que tomaste (y por qué)

1. **La función del hilo** es `void *coder_loop(void *arg)`, recibe `&sim->coders[i]`.
2. **`pthread_create` devuelve 0 = bien.** El error es `!= 0` (ojo: al revés que tus `check_*`).
3. **`break` antes de `i++`** en el bucle de creación: así `i` = número de hilos que SÍ existen.
4. **`join` antes de `cleanup`**: nunca liberar `sim->coders` con hilos vivos usándolo.
5. **`join_threads(sim, i)`**: une solo los `i` vivos. En el caso normal `i == n`; en el error, `i < n`.

### ⚠️ Sobre el "interleaving"

Los 4 hilos salen **alineados por fase** (todos Compile, todos Debug...). **No es un bug:** ahora
mismo nada los frena, todos hacen el mismo `usleep` y arrancan casi a la vez. El entrelazado real
aparecerá en **M4**, cuando los dongles hagan esperar a unos y a otros.

### Pendiente que NO es de M3

- El monitor todavía no existe (llega en M8).
- El `printf` da trazas; el log con formato exacto es M9.
- El `while (compile_times < required)` del coder cambiará en M8 (lo decide el monitor).

### Checkpoint (cumplido)

- [x] `make re` ✅ + `norminette .` ✅ + `valgrind --leak-check=full` ✅.
- [x] El ciclo compile→debug→refactor sale de todos los coders.
- [x] El programa termina solo (todos los hilos acaban y `main` sale) sin quedar colgado.

---

## LECCIÓN 21: M4 — Adquirir/soltar dongles (mutex + condition variable)

**Estado: 🔴 EN CURSO — el código NO compila. Arréglalo antes de seguir.**

Añade el estado real de los dongles y la sección crítica: solo se compila teniendo los **dos**.
Aquí es donde blindas contra el **deadlock**.

> **Orden de trabajo de esta lección**
> 1. Arreglar los errores de compilación de `init.c` (están arriba, en «Lo que falla AHORA MISMO»).
> 2. Cambiar la firma de `take_dongle` / `release_dongle` (hoy pasan structs **por valor**).
> 3. Leer el **mapa de variables** de abajo hasta que puedas decir qué es cada una **de memoria**.
> 4. Escribir `take_dongle` y `release_dongle`.
> 5. Llamarlas desde `coder_loop` con el orden de adquisición **correcto** (⚠️ no es el que crees).
> 6. Checkpoint.

### Punto de partida (lo que YA existe, no hay que crearlo)

| Ya existe | Dónde | Qué hace |
|---|---|---|
| `pthread_mutex_t mutex` dentro de `t_dongle` | `codexion.h` | **un mutex por dongle** (decisión tomada) |
| `pthread_cond_t cond` dentro de `t_dongle` | `codexion.h` | añadido en M4 — **falta inicializarlo y destruirlo** |
| `int is_free` | `t_dongle` | el estado del recurso (1 = libre) |
| `long cooldown_until` | `t_dongle` | para M5, ya reservado |
| `pthread_mutex_init` / `pthread_mutex_destroy` | `init.c` | creado en `init_dongles`, destruido en `cleanup` |
| `t_dongle *left` / `t_dongle *right` | `t_coder` | ya cableados: `left = dongles[i]`, `right = dongles[(i+1) % n]` |

**Lo que falta:** las funciones de coger/soltar y llamarlas desde `coder_loop`. Nada más.

### 🗺️ MAPA DE VARIABLES DE M4 (léelo antes de escribir una línea)

Que quede **clarísimo qué es cada cosa**. Todas estas variables aparecen en los ejemplos de abajo.

#### La confusión de base: `t_dongle` vs `t_dongle *`

| Se escribe | Qué es | Ejemplo |
|---|---|---|
| `t_dongle` | el **molde** (el tipo). Solo sirve para decir "de qué va el struct" | `malloc(sizeof(t_dongle) * n)` |
| `t_dongle *` | un **puntero** a un dongle: una dirección. Con él llegas al dongle **real** | `t_dongle *dongle;` |
| `t_dongle dongle` | una **copia** entera del struct, en la pila de la función | ⚠️ **esto es lo que tenías en `coder.c`** |

> ⚠️ **Por qué la copia es un error grave.** Al pasar `t_dongle` por valor, C copia el struct
> **entero** (mutex y cond incluidos) a la pila. Esa copia **no es** el dongle que ven los demás
> hilos: hacer `dongle.is_free = 0` dentro de la función **no cambia nada en la mesa**. Y copiar un
> `pthread_mutex_t` que está en uso es comportamiento indefinido.
>
> **Regla del proyecto:** todos los structs se pasan **por puntero**: `t_sim *`, `t_coder *`,
> `t_dongle *`. Si ves `t_sim sim` o `t_dongle dongle` en una firma, es un error.

#### La tabla grande

| Variable | Tipo | Dónde vive | Qué significa | Quién la escribe | Quién la lee |
|---|---|---|---|---|---|
| `sim` | `t_sim *` | parámetro de `take_dongle`/`release_dongle`; dentro del coder es `c->sim` | puntero a **la mesa**: la simulación entera (argumentos, arrays, flags, mutex del log) | `init` | todos los hilos |
| `c` | `t_coder *` | variable **local** de `coder_loop` | puntero a **"yo"**: esta persona concreta. Es el `arg` que le pasó `pthread_create` | `init_coders` | solo su hilo |
| `c->sim` | `t_sim *` | campo de `t_coder` | atajo para llegar a la mesa sin pasarla como parámetro | `init_coders` | su hilo |
| `c->left` | `t_dongle *` | campo de `t_coder` | **el dongle de mi izquierda** = `&dongles[i]` | `init_coders` | su hilo |
| `c->right` | `t_dongle *` | campo de `t_coder` | **el dongle de mi derecha** = `&dongles[(i+1) % n]` | `init_coders` | su hilo |
| `dongle` | `t_dongle *` | parámetro de `take_dongle`/`release_dongle` | puntero a **el dongle concreto** sobre el que trabajas ahora | — | quien llama |
| `dongle->mutex` | `pthread_mutex_t` | dentro de `t_dongle` | **el candado de ese dongle**; protege `is_free` | `init_dongles` | quien lo usa |
| `dongle->cond` | `pthread_cond_t` | dentro de `t_dongle` | **el timbre de ese dongle**; avisa a los que esperan | `init_dongles` | quien espera |
| `dongle->is_free` | `int` | dentro de `t_dongle` | **el estado**: `1` = libre, `0` = alguien lo tiene | `take_dongle` (→0) y `release_dongle` (→1) | `take_dongle`, para decidir si espera |
| `dongle->cooldown_until` | `long` | dentro de `t_dongle` | instante (ms) hasta el que el dongle está en cooldown. **M5**, todavía quieto | `release_dongle` (M5) | M5 |
| `c->compile_times` | `int` | dentro de `t_coder` | nº de compilaciones que **yo** ya he hecho | mi propio hilo | mi hilo (+ monitor en M8) |
| `c->last_compile` | `long` | dentro de `t_coder` | ms desde el inicio en los que **yo** empecé mi última compilación | mi propio hilo | monitor (M8) |
| `sim->log_mutex` | `pthread_mutex_t` | dentro de `t_sim` | candado del **log** (M9) | `init` | todos (M9) |

#### El dibujo de la mesa

```
sim  (t_sim *)  ─────────────────────────────►  la mesa: la comparten TODOS los hilos
 │
 ├── coders ──▶ [ coder 0 ][ coder 1 ][ coder 2 ]
 │                │         │         │
 │              c->left   c->left   c->left     (t_dongle *)
 │              c->right  c->right  c->right
 │               d0,d1    d1,d2    d2,d0
 │
 ├── dongles ─▶ [ dongle 0 ][ dongle 1 ][ dongle 2 ]   (t_dongle)
 │                │  mutex   │  mutex   │  mutex      ← un candado POR dongle
 │                │  cond    │  cond    │  cond       ← un timbre POR dongle
 │                │  is_free │  is_free │  is_free    ← el estado que se protege
 │
 └── stop_mutex, log_mutex
```

#### Los tres `&` que hay que entender bien

| Se escribe | Significado | Dónde se usa |
|---|---|---|
| `&dongle->mutex` | "la dirección del mutex que está **dentro** del dongle" | `pthread_mutex_lock(&dongle->mutex)` |
| `&sim->dongles[i].cond` | "la dirección de la cond que está dentro del dongle nº `i`" | `pthread_cond_init(&sim->dongles[i].cond, NULL)` |
| `&sim->coders[i].left` | "la dirección del **puntero** `left` que está dentro del coder nº `i`" | cuando le asignas el dongle |

> **Regla:** el `&` va delante de una **variable** (un objeto). Nunca delante de algo que ya es una
> dirección: `&dongle->mutex` sí (el mutex es un objeto), pero `&c->left` **tampoco** — `c->left`
> ya es un puntero, y `&c->left` sería "la dirección del puntero", que es otra cosa distinta.

### Decisión 1: un mutex por dongle (no uno global)

El usuario lo razonó así: *"uno para cada dongle porque se liberan y se comprometen a tiempos
distintos"*. Correcto. Con un solo mutex global, para mirar el dongle 3 bloqueas también el 0, el 1,
el 2... todo el mundo en cola por cualquier dongle. Con uno por dongle, cada conflicto es local.

### Decisión 2: condition variable, NO busy-wait

El usuario eligió **condition variable** y dio el argumento exacto:

- El **busy-wait** gasta CPU y el `usleep` que lo acompaña es impreciso: "puede pasarse o quedarse
  corto". No es justo y no es fiable.
- El **condition variable** duerme de verdad (0% CPU mientras espera) y despierta justo cuando
  alguien avisa.

Y un motivo extra que decidiste sin saberlo: **el cooldown (M5) también necesita espera.** Con `cond`
esa espera encaja de forma natural (`pthread_cond_timedwait`); con busy-wait habría que rehacerlo.
Elegir cond ahora ahorra trabajo en M5.

**Las tres piezas y para qué sirve cada una** (esto es lo que no cuadraba en los ejemplos):

```
t_dongle / is_free      → el ESTADO del recurso   (¿lo tiene alguien?)
pthread_mutex_t mutex   → PROTEGE ese estado      (que se mire de uno en uno)
pthread_cond_t  cond    → CANAL de aviso          (para no preguntar en bucle)
```

La condición **no guarda nada**. Es solo el timbre: "quien espere aquí, que se despierte y **vuelva
a mirar**". El estado sigue en `is_free`. Por eso la condición siempre va con el mutex: uno protege,
el otro avisa.

### El patrón obligatorio — y qué es cada línea

Primero la versión abstracta (la idea):

```
lock(mutex)
while (!puedo)              ← si esto no se cumple, vuelvo a esperar
    cond_wait(cond, mutex)  ← suelta el mutex y duerme; al despertar lo recupera
hago la acción              ← ya tengo el mutex Y la condición se cumple
unlock(mutex)
```

Ahora la versión **con los nombres reales de tu proyecto** (esto es lo que te faltaba: saber qué es
cada cosa):

```c
void	take_dongle(t_sim *sim, t_dongle *dongle)
{
	(void)sim;
	pthread_mutex_lock(&dongle->mutex);
	while (dongle->is_free == 0)
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	dongle->is_free = 0;
	pthread_mutex_unlock(&dongle->mutex);
}
```

| Línea | Qué hace | Por qué |
|---|---|---|
| `void take_dongle(t_sim *sim, t_dongle *dongle)` | declara la función | `sim` y `dongle` son **punteros**: así tocas el dongle de verdad, no una copia |
| `(void)sim;` | "este parámetro no lo uso (todavía)" | con `-Werror`, un parámetro sin usar **rompe la compilación** |
| `pthread_mutex_lock(&dongle->mutex);` | cojo el candado **de ese dongle** | `&` porque `mutex` es un objeto dentro del struct; y el candado es el del dongle, no uno global |
| `while (dongle->is_free == 0)` | mientras esté ocupado, no puedo cogerlo | **`while`, no `if`**: al despertar puede que otro se lo haya llevado |
| `pthread_cond_wait(&dongle->cond, &dongle->mutex);` | duerme hasta que le avisen | suelta el mutex mientras duerme y lo **vuelve a coger** al despertar |
| `dongle->is_free = 0;` | **toma** el dongle | solo puede hacerse teniendo el mutex → nadie más puede hacerlo a la vez |
| `pthread_mutex_unlock(&dongle->mutex);` | suelta el candado | **después** de tocar el estado, nunca antes |

Y la pareja, la otra mitad de la sección crítica:

```c
void	release_dongle(t_sim *sim, t_dongle *dongle)
{
	(void)sim;
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_free = 1;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
```

| Línea | Qué hace | Por qué |
|---|---|---|
| `pthread_mutex_lock(&dongle->mutex);` | coge el candado | sin esto, `is_free = 1` y `broadcast` podrían pisarse con otro hilo |
| `dongle->is_free = 1;` | devuelve el dongle a la mesa | el estado **cambia aquí**, protegido por el mutex |
| `pthread_cond_broadcast(&dongle->cond);` | **toca el timbre**: "hay dongle libre, despierten los que esperan" | `broadcast` y no `signal`: pueden esperar varios en el mismo dongle, y con `signal` solo se despertaría uno (puede que no sea el que le toca) |
| `pthread_mutex_unlock(&dongle->mutex);` | suelta el candado | |

Y una tercera clave, la que se te va a colar: **el `broadcast` va ANTES del `unlock`.** Si avisas
después de soltar el candado, otro hilo puede despertar, no encontrar el dongle libre y volver a
dormirse **sin que nadie lo vuelva a avisar** → el timbre queda mudo.

### 🔴 Decisión 3: el orden de adquisición (CORREGIDA — la anterior provocaba un cuelgue)

> **Aviso.** La versión anterior de esta guía decía *"todos cogen primero el izquierdo, después el
> derecho; así no hay ciclo"*. **Es falso**, y era la idea que tenías en la cabeza. Con `n = 4` eso
> **cuelga el programa**. Aquí está la traza, paso a paso.

#### La traza del deadlock (n = 4, los 4 arrancan a la vez)

Convención del proyecto: el coder de índice `i` (0-based) coge `left = dongles[i]` y
`right = dongles[(i+1) % 4]`.

```
Coder 0:  left = d0   right = d1
Coder 1:  left = d1   right = d2
Coder 2:  left = d2   right = d3
Coder 3:  left = d3   right = d0      ← este es el que cierra el círculo
```

Si todos cogen primero el **izquierdo**, los 4 dongles se reparten sin conflicto (son distintos):

```
t0:  c0 toma d0    c1 toma d1    c2 toma d2    c3 toma d3
t1:  c0 pide d1 → OCUPADO     c1 pide d2 → OCUPADO
     c2 pide d3 → OCUPADO     c3 pide d0 → OCUPADO
t2:  los 4 esperando. NINGUNO tiene sus dos dongles.
     → nadie puede soltar nada. El programa se cuelga PARA SIEMPRE.
```

La espera es circular: `c0 → d1 → c1 → d2 → c2 → d3 → c3 → d0 → c0`.
Se cumplen **las 4 condiciones de Coffman**:

| Condición de Coffman | ¿Se cumple aquí? |
|---|---|
| Exclusión mutua | un dongle, un dueño ✔ |
| Retención y espera | c0 **ya tiene** d0 y **espera** d1 ✔ |
| Sin expropiación | a c0 no le quitan d0 ✔ |
| **Espera circular** | d0 → d1 → d2 → d3 → d0 ✔ |

#### ✅ La regla correcta: orden global por índice

> **Cada coder coge sus dos dongles en orden CRECIENTE de índice: primero el de índice MENOR,
> después el de índice MAYOR.**

| Coder | sus dos dongles | orden correcto | ¿coincide con "izquierdo primero"? |
|---|---|---|---|
| 0 | d0, d1 | d0 → d1 | sí |
| 1 | d1, d2 | d1 → d2 | sí |
| 2 | d2, d3 | d2 → d3 | sí |
| 3 (el último) | d3, d0 | **d0 → d3** | **NO** → coge primero su *derecho* |

Es decir: **todos menos el último cogen el izquierdo primero; el último coge el derecho primero.**
No es una chapuza: es **ordenar los recursos con un orden global** (aquí, el índice del dongle) y
que cada hilo los coja **siempre** en ese orden. Eso **rompe la espera circular** por construcción.

#### Traza con la regla correcta (n = 4)

```
t0:  c0 pide d0 → LIBRE → lo tiene      c3 pide d0 → OCUPADO (lo tiene c0) → espera
t1:  c1 pide d1 → LIBRE → lo tiene      c0 pide d1 → OCUPADO (lo tiene c1) → espera
t2:  c2 pide d2 → LIBRE → lo tiene      c1 pide d2 → OCUPADO (lo tiene c2) → espera
t3:  c2 pide d3 → LIBRE → lo tiene → c2 ya tiene d2 y d3 → compila → suelta
t4:  c1 pide d2 → ahora LIBRE → lo tiene → compila → suelta
t5:  c0 pide d1 → ahora LIBRE → lo tiene → compila → suelta
t6:  c3 pide d0 → ahora LIBRE → lo tiene → pide d3 → LIBRE → lo tiene → compila
```

✅ **Nadie espera en ciclo.** La cadena de esperas arranca en el que espera el índice más bajo
(d0) y **termina** en el que puede completarse (d3).

> **El invariante que hay que repetirse (y que va en el README):** con este orden, un hilo que
> espera **siempre espera un índice MAYOR que todos los que ya tiene**. El que tiene el índice más
> alto nunca está esperando nada → el último de la cadena siempre puede terminar, soltar, y desbloquear
> a todos los demás. Por eso no puede haber ciclo **en ningún interlineado**, no solo en el de arriba.

#### Cómo se implementa el "menor índice primero"

Tus dos dongles son `c->left` y `c->right`, y **los dos son punteros dentro del MISMO array**
(`sim->dongles`). En C, comparar dos punteros **del mismo array** con `<` o `>` **es comparar sus
índices**: lo garantiza el estándar, siempre que no se comparen punteros de arrays distintos.

```c
t_dongle	*first;
t_dongle	*second;

first = c->left;
second = c->right;
if (first > second)
{
	first = c->right;
	second = c->left;
}
take_dongle(c->sim, first);     /* el de índice MENOR: el primero que se coge */
take_dongle(c->sim, second);    /* el de índice MAYOR: el segundo */
```

> ⚠️ **Trampa:** comparar punteros de **arrays distintos** no significa nada (el estándar no lo
> define). Aquí es seguro **porque** `left` y `right` salen los dos de `sim->dongles`. Si algún día
> cambiabas la disposición de la mesa, esta comparación dejaría de ser válida → la alternativa
> blindada es guardar los **índices** en el `t_coder` en vez de los punteros.

#### 🔴 El caso `n = 1`: `left` y `right` son EL MISMO dongle

Con `n = 1`: `left = &dongles[0]` y `right = &dongles[(0 + 1) % 1] = &dongles[0]`.
**Son el mismo dongle.** Si haces `take_dongle(left)` y después `take_dongle(right)`, el segundo
`take_dongle` ve `is_free == 0` (lo acabas de tomar tú) y **se espera a sí mismo → cuelgue eterno**.

```
c0: take_dongle(d0) → is_free = 0, vale
c0: take_dongle(d0) → is_free == 0 → espera… espera… (él mismo es el dueño; nadie va a soltarlo)
    → CUEGUE
```

El subject dice que con una persona hay **un solo dongle** sobre la mesa → se coge **una vez**:

```c
take_dongle(c->sim, first);
if (first != second)
	take_dongle(c->sim, second);
...
release_dongle(c->sim, first);
if (first != second)
	release_dongle(c->sim, second);
```

> `first != second` compara **punteros**: si son el mismo dongle, dan la misma dirección → son
> iguales. Funciona porque los dos punteros salen del mismo array. (Y si `n > 1`, `first` y `second`
> son dongles distintos → la condición se cumple y coge los dos.)


#### La otra opción (la necesitarás en M6): un árbitro central

| | **Opción A — orden por índice** (la de arriba) | **Opción B — árbitro central** |
|---|---|---|
| Idea | cada coder coge sus dongles en un orden global fijo; si no puede, espera | un mutex + una cola en `t_sim`: el coder **pide el par** y el árbitro lo concede cuando puede |
| Deadlock | imposible **por el orden global** | imposible porque **nadie coge medio par**: se conceden los 2 a la vez |
| Coste ahora | nada: es ordenar dos punteros | un mutex nuevo, una cola de peticiones, la lógica de concesión |
| ¿Sirve para FIFO/EDF (M6/M7)? | ❌ no: no hay ningún sitio donde decidir "quién va primero" | ✅ sí: la cola de peticiones **es** el sitio donde FIFO/EDF deciden |
| Riesgo | el orden hay que respetarlo en **todas** las adquisiciones | más código de golpe |

**Recomendación para M4:** Opción A (es lo mínimo que cumple el hito y te hace ver a Coffman en
práctica). Si en M6 el heap te obliga a rehacer el reparto, ese es el precio de entender por qué
existe un árbitro. La Opción B la escribirás igualmente en M6/M7.

⚠️ Es fácil confundir esta pregunta con *"¿quién gana si dos quieren el mismo dongle?"*: eso es el
scheduler (M6 FIFO / M7 EDF) y **no** se resuelve aquí.

### Diseño: el `coder_loop` completo, línea por línea

Prefiere **una función que coge un dongle y otra que lo suelta** — tonta cada una, una sola cosa.
Y como la Norm limita cada función a **25 líneas**, el par de adquisiciones va en su propio helper:

```c
static void	acquire_dongles(t_coder *c)
{
	t_dongle	*first;
	t_dongle	*second;

	first = c->left;
	second = c->right;
	if (first > second)
	{
		first = c->right;
		second = c->left;
	}
	take_dongle(c->sim, first);
	if (first != second)
		take_dongle(c->sim, second);
}

static void	release_dongles(t_coder *c)
{
	release_dongle(c->sim, c->left);
	if (c->left != c->right)
		release_dongle(c->sim, c->right);
}

void	*coder_loop(void *arg)
{
	t_coder	*c;

	c = arg;
	while (c->compile_times < c->sim->compiles_required)
	{
		acquire_dongles(c);
		printf("Compile\n");
		usleep(c->sim->comp_time * 1000);
		release_dongles(c);
		c->compile_times += 1;
		printf("Debug\n");
		usleep(c->sim->debug_time * 1000);
		printf("Refactor\n");
		usleep(c->sim->refactor_time * 1000);
	}
	return (NULL);
}
```

| Pieza | Qué hace | Trampa que evita |
|---|---|---|
| `t_dongle *first; t_dongle *second;` | dos punteros **locales**, al principio del bloque | declararlos a mitad de bloque incumple la Norm; y sin declararlos **no compila** |
| `if (first > second)` → reordenar | **orden global por índice** de Decisión 3 | "izquierdo primero" **cuelga con n ≥ 3** |
| `if (first != second) take_dongle(second)` | solo coge el segundo si son **distintos** | con `n = 1` los dos son el mismo dongle → **se esperaría a sí mismo** |
| `release_dongles(c)` justo tras el `usleep` de compilar | los suelta **antes** de depurar y refactorizar | tener dongles en debug/refactor hambrienta a los vecinos → burnout |
| `c->compile_times += 1;` **después** de soltar | se cuenta al terminar, no al empezar | — |
| `* 1000` en los `usleep` | los argumentos van en **ms** y `usleep` pide **µs** | compilaría 1000 veces más rápido de lo que dice el usuario |

> **El orden en `coder_loop` importa:** `acquire` → compilar → `release` → contar → debug → refactor →
> vuelta a empezar. `printf("Compile")` va **dentro** del par, porque el log del subject pide dos
> `has taken a dongle` antes de `is compiling` (la traducción a ese formato es M9).
>
> **Por qué NO meter "¿cuántos tengo?" dentro de `take_dongle`:** mezclaría estado y condiciones donde
> no tocan. `take_dongle` coge **uno** y punto. Coger dos es responsabilidad del coder (aquí, de
> `acquire_dongles`).

> 📐 **Orden de definición en `coder.c`** (los `static` se declaran antes de usarse):
> `take_dongle` → `release_dongle` → `acquire_dongles` → `release_dongles` → `coder_loop`.
> Así `coder.c` se queda en **exactamente 5 funciones**, el límite de la Norm.

### Cambios en struct y ciclo de vida

1. `t_dongle` → añadir `pthread_cond_t cond;` ✅ (ya está hecho).
2. `init_dongles` → `pthread_cond_init(&sim->dongles[i].cond, NULL);` junto al `pthread_mutex_init`.
3. `cleanup` → `pthread_cond_destroy(&sim->dongles[i].cond);` dentro del bucle, junto al `destroy`
   del mutex.
4. **Prototipos en `codexion.h`** para `take_dongle` y `release_dongle` (hoy no están: la Norm y
   el compilador los exigen).

```c
void	take_dongle(t_sim *sim, t_dongle *dongle);
void	release_dongle(t_sim *sim, t_dongle *dongle);
```

> **Alineación de prototipos:** `void` son 4 letras → **1 tab** (como `cleanup`, `join_threads`...).

### Preguntas antes de escribir

1. ¿Por qué `while` y no `if` alrededor de `cond_wait`?
2. ¿Por qué `broadcast` y no `signal` cuando varios esperan el mismo dongle?
3. ¿Qué condición de Coffman **rompe** el orden por índice, y por qué "izquierdo primero" no la
   rompe?
4. Con `n = 1`, ¿qué dos nombres son iguales y qué pasa si llamas dos veces a `take_dongle`?
5. ¿Por qué el parámetro es `t_dongle *` y no `t_dongle`?

### Checkpoint

- [ ] `make re` vuelve a estar **verde** (los errores de `init.c` y de `coder.c` resueltos).
- [ ] Dos coders no cogen nunca el mismo dongle a la vez, y con **2 coders no se cuelga**.
- [ ] Con `n = 4` **no hay deadlock**: los 4 terminan. (Si cuelga, mira la traza de Decisión 3.)
- [ ] Con `n = 1` termina (no se espera a sí mismo).
- [ ] `norminette coders/` a 0 + `valgrind --leak-check=full` sin fugas.

---

## LECCIÓN 22: M5 — Cooldown

**Estado: ⬜ Pendiente.**

Añade `cooldown_until` y haz que quien quiera el dongle espere (`cond_timedwait`) hasta que expire.

- [ ] Tras soltar, el dongle no vuelve a aparecer libre antes de `dongle_cooldown` ms.

---

## LECCIÓN 23: M6 — El heap y la planificación FIFO

**Estado: ⬜ Pendiente.**

Implementa el heap y la **cola de solicitudes** por dongle. Con `fifo`, concede al que llegó antes.

- [ ] Con muchos coders y `fifo`, se respeta el orden de llegada de las peticiones.

---

## LECCIÓN 24: M7 — Planificación EDF

**Estado: ⬜ Pendiente.**

Cambia la clave del heap a `deadline` cuando el scheduler es `edf`.

- [ ] Con `edf` y parámetros viables, **nadie se agota**; con `fifo` puede agotarse alguien.

---

## LECCIÓN 25: M8 — El monitor y la detección de burnout

**Estado: ⬜ Pendiente.**

Haz que el monitor vigile todos los deadlines y, al vencer, escriba “X burned out” (≤ 10 ms) y detenga
la simulación.

- [ ] Provoco un burnout a propósito y el mensaje sale a tiempo; todos los hilos paran.

---

## LECCIÓN 26: M9 — Log serializado

**Estado: ⬜ Pendiente.**

Centraliza todas las escrituras en una función que usa el **mutex de log**.

- [ ] Lanzo `n` grande y **nunca** se mezclan dos mensajes en una línea.

---

## LECCIÓN 27: M10 — Terminación y limpieza (sin leaks)

**Estado: ⬜ Pendiente.**

Terminar cuando: alguien se agota **o** todos compilan ≥ `number_of_compiles_required`.
`join` de todos los hilos y `cleanup` total (mutex, cond, heaps).

- [ ] `valgrind --leak-check=full` no reporta fugas ni en el caso normal ni en el de burnout.

---

# PARTE V: VERIFICACIÓN

## LECCIÓN 28: Norminette y estilo

**Estado: ⬜ Pendiente (se comprueba en cada hito).**

`norminette coders/` hasta **0 errores**. Ojo: también cuentan los archivos/funciones extra.

### Reglas que causaron errores reales en M2 (apúntalas)

- **Espacios alrededor de los operadores**: `sizeof(t_dongle) * n`, nunca `sizeof(t_dongle)*n`.
- **Alineación de prototipos en headers**: todos los nombres de función deben caer en la **misma
  columna**. Como `int` (3 letras) y `void` (4) no miden lo mismo, necesitan distinto número de
  tabs: `int` + **2 tabs**, `void` + **1 tab**.

```c
int		init(t_sim *sim);        /* int  + 2 tabs  */
void	cleanup(t_sim *sim);     /* void + 1 tab   */
```

- **Declaraciones al principio** del bloque: `t_sim sim;` va justo tras `{`, no después de un `if`.
- **5 funciones por archivo, 25 líneas por función**: si `init` se pasa, parte en helpers
  (`init_dongles`, `init_coders`); si el archivo llega a 5, crea uno nuevo (`coder.c`, `log.c`...).
- **Archivos de prueba**: cualquier `.c` suelto en `coders/` cuenta para la Norm. Los experimentos
  (como un `testpthread.c`) van fuera o se borran antes del checkpoint.
- **Las extensiones "42 formatter" de VS Code no son el norminette**: pueden formatearte el código
  con reglas distintas (quitando espacios que la Norm exige). El juez es la CLI: `norminette coders/`.

- [ ] Norminette limpio en todo `coders/`.

---

## LECCIÓN 29: Leaks y sanitizers

**Estado: ⬜ Pendiente (se comprueba en M2 cuando exista `cleanup`).**

- `valgrind --leak-check=full`
- `valgrind --tool=helgrind` (carreras)
- Recompila con `-fsanitize=thread` para cazar *data races*

- [ ] Sin fugas y sin *data races* reportados.

---

## LECCIÓN 30: Pruebas y casos límite

**Estado: ⬜ Pendiente (hay pruebas parciales de M1 ya pasadas).**

Prueba a mano (y guarda tus comandos):

- `number_of_coders = 1` (¡un solo dongle!)
- Parámetros **viables** vs **inviables** para EDF.
- `dongle_cooldown = 0` y valores grandes.
- Muchos coders (10–50) para forzar carreras.
- Entradas inválidas (`-1`, `abc`, `fifoX`, faltan args).

- [ ] Tengo una lista de comandos de prueba reproducibles.

---

## LECCIÓN 31: El README

Incluye (mira `AGENTS.md`): primera línea en cursiva, **Descripción**, **Instrucciones**, **Recursos**
(con uso de IA), y las dos secciones obligatorias: **Blocking cases handled** y
**Thread synchronization mechanisms**.

- [ ] El README está completo y explica *por qué*, no solo *qué*.

---

# PARTE VI: DEFENSA

## LECCIÓN 32: La recode

Durante la evaluación pueden pedirte un **cambio pequeño** (ajustar comportamiento, unas líneas, un dato nuevo).
Prepárate: ten el proyecto compilando, entiende tus estructuras y sé rápido y seguro tocándolas.

- [ ] Sé hacer un cambio pequeño en caliente sin romper la Norm ni los leaks.

---

## LECCIÓN 33: Preguntas típicas de evaluación

Prepárate para explicar **por qué**, no solo **qué**:

1. ¿Cómo evitas el deadlock? ¿Qué condición de Coffman rompes?
2. ¿Cómo garantizas que el log no se mezcla?
3. ¿Qué diferencia hay entre FIFO y EDF y cuándo se agota alguien?
4. ¿Por qué un monitor separado y no comprobar dentro de los coders?
5. ¿Cómo detectas el burnout dentro de los 10 ms?
6. ¿Qué pasa si `dongle_cooldown` es gigante? ¿Y si `number_of_coders` es 1?
7. ¿Dónde está el cooldown y por qué al soltar?

- [ ] Sé responder a todas sin mirar el código.

---

# ANEXO

## Anexo Rápido: tabla de diagnóstico (síntoma → causa)

> Añadida tras M2. Guárdala: cuando algo falle, busca el síntoma y casi seguro está aquí.

| Síntoma | Causa probable | Lección |
|---|---|---|
| `error: expected expression before '...'` | código a medio escribir (el `...` no es C) | 0 |
| `unused parameter` con `-Werror` | la función aún no usa su argumento | — |
| El programa compila pero no hace nada visible | falta una llamada en `main` (código "huérfano") | 1.2 |
| `return` con valor en `void` / viceversa | la firma no cuadra con el header | 14 |
| `passing argument ... from incompatible pointer type` | le pasas un `t_coder *` donde espera un `pthread_mutex_t *` | 14 |
| Valgrind: "definitely lost" | un `malloc` sin `free` (¿dentro de un bucle?) | 6.5.2 |
| Valgrind: "Invalid read/write" | acceso a índice fuera de rango o a memoria ya liberada | 6.5.3, 6.5.7 |
| Segfault al recorrer el bucle de `cleanup` | `= NULL` **dentro** del bucle | 6.5.7 |
| Programa colgado sin mensaje | falta un `unlock`, o deadlock (Coffman), o `join` faltante | 1.3, 4 |
| El hilo nunca imprime nada | `main` hizo `return` sin `join` (el SO mata los hilos) | 1.3 |
| Primer elemento del array con basura | bucle con `i = 1` en vez de `i = 0` | 6.5.3 |
| ids desplazados en el log (empiezan en 2) | `i = 1` por querer que el id empiece en 1 | 6.5.4 |
| `if (comprobar_algo)` no detecta errores nunca | falta los paréntesis: `if (comprobar_algo())` | 6.5.6 |
| `too few arguments to function call, expected 2, have 0` | a un `mutex_init`/`cond_init` le faltan **qué** y los atributos | 21 |
| `expected identifier` en `x->` | el nombre no existe en ese struct (te has dejado un trozo) | 21, 14 |
| Compila, pero revienta cuando un `pthread_create` falla | doble `cleanup`: la segunda vez sobre `NULL` | 6.5.7 |
| Un `x == NULL` "no asigna nada" | `==` compara; `=` asigna | 6.5.6 |
| `n = 1` funciona pero `n = 4` revienta (o al revés) | caso límite no trazado a mano | 6.5 |
| Un solo coder avanza y el resto espera | mutex global en vez de uno por recurso | 2.2 |
| Dos mensajes del log en la misma línea | falta el mutex de log | 9 |

**Regla de oro del debugging en concurrencia:** traza el bucle **a mano** con `n = 1`, `n = 2` y
`n = 4` antes de ejecutar. Casi todos los bugs de M2 se veían en la primera vuelta del papel.
Y recuerda: la concurrencia castiga 1 de cada 10.000 ejecuciones — si "a veces falla", es una
carrera, no un error de sintaxis.

## Glosario

| Término | Significado |
|---|---|
| **Hilo (thread)** | Persona dentro del proceso; comparte memoria, tiene pila propia |
| **Mutex** | Candado: solo un hilo dentro de la sección crítica |
| **Condición de carrera** | Fallo por acceso concurrente no sincronizado a datos compartidos |
| **Variable de condición** | Permite dormir hasta que alguien avise |
| **Deadlock** | Cuelgue: cada hilo espera a otro para siempre |
| **Coffman** | Las 4 condiciones que deben darse para un deadlock |
| **Starvation** | Un hilo nunca consigue el recurso |
| **Scheduler** | Regla de arbitraje (fifo / edf) |
| **FIFO** | First In First Out: orden de llegada |
| **EDF** | Earliest Deadline First: el deadline más próximo primero |
| **Deadline** | `last_compile_start + time_to_burnout` |
| **Burnout** | Agotamiento: no empezó a compilar a tiempo |
| **Dongle** | Recurso escaso (uno por persona); hacen falta 2 para compilar |
| **Cooldown** | Tiempo que un dongle queda inutilizable tras soltarse |
| **Heap / cola de prioridad** | Estructura que da siempre el mínimo (más urgente) |
| **Monitor** | Hilo que vigila deadlines y detiene la simulación |
| **Sección crítica** | Trozo de código que solo un hilo puede ejecutar a la vez |
| **Serializar (log)** | Escribir mensajes enteros, sin mezclarse |

## Checklist final

- [ ] Compila con `cc -Wall -Wextra -Werror -pthread`, sin relink.
- [ ] `norminette` = 0 errores en **todos** los archivos.
- [ ] Sin variables globales. Sin libft. Solo funciones autorizadas.
- [ ] Sin leaks (`valgrind`) y sin *data races* (`helgrind`/tsan).
- [ ] Rechaza entradas inválidas.
- [ ] Cooldown correcto; planificador FIFO y EDF correctos; EDF no deja morir a nadie.
- [ ] Monitor detecta burnout ≤ 10 ms y para la simulación.
- [ ] Log serializado con el formato exacto.
- [ ] Termina por burnout **o** por compilaciones completadas.
- [ ] README con todas las secciones obligatorias.
- [ ] Sé explicar y modificar cualquier parte (para la defensa).

## Mapa del subject → dónde se cumple

| Regla del subject | Dónde se cumple (archivo/función) |
|---|---|
| Hilos POSIX | `coder.c`, `monitor.c` |
| Mutex por dongle | `dongle.c` |
| Variable de condición | `dongle.c` (espera de dongles/cooldown) |
| Cooldown | `dongle.c` |
| Arbitraje FIFO/EDF | `heap.c` + `dongle.c` |
| Cola de prioridad (heap) | `heap.c` |
| Monitor + burnout ≤ 10 ms | `monitor.c` |
| Log serializado | `log.c` |
| Sin leaks | `cleanup.c` + `valgrind` |
| Sin globales / funciones autorizadas | todo el proyecto (`AGENTS.md`) |

> **De qué ficheros habla esta tabla y cuáles existen ya:**
>
> | Fichero | ¿Existe? | Nota |
> |---|---|---|
> | `main.c`, `parser.c`, `init.c`, `sim.c`, `log.c` | ✅ | M1–M3 |
> | `coder.c` | ✅ | ahora mismo contiene también `take_dongle()` / `release_dongle()` |
> | `dongle.c` | ❌ | lo creas en M4 (puedes dejarlo en `coder.c` si la Norm te obliga a partir) |
> | `heap.c` | ❌ | lo creas en M6 (FIFO/EDF) |
> | `monitor.c`, `cleanup.c` | ❌ | M7 / `cleanup()` vive hoy en `init.c:70` |
>
> O sea: la tabla es el **destino**, no el estado actual. Cuando crees o muevas un
> fichero, actualízala (y la tabla de archivos sugeridos de la Lección 14.2).

## Recursos para estudiar

- `man pthread_create`, `man pthread_mutex_lock`, `man pthread_cond_wait`, `man gettimeofday`.
- **The Little Book of Semaphores** (Allen Downey) — clásico de sincronización.
- Wikipedia: *Dining philosophers problem*, *Coffman conditions*, *Earliest deadline first*.
- Norm de 42: https://github.com/42School/norminette/blob/master/pdf/en.norm.pdf
- El propio subject (`es.subject.pdf`) — léelo antes de cada parte.

---

**Fin de la guía. ¡Mucha suerte con Codexion! Cuando quieras, empezamos por la Lección 1.**
