/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 20:48:01 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/19 21:11:01 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// 1. Reservar la memoria de sim->coders y sim->dongles.
// 2. Comprobar que malloc no devolvió NULL (puede fallar si no hay memoria). Si falla, limpiar lo ya hecho y devolver 0.
// 3. Inicializar cada dongle: pthread_mutex_init de su mutex, is_free = 1, cooldown_until = 0.
// 4. Inicializar cada coder: coder_id (el número 1..n para el log), compile_times = 0, last_compile = 0.
// 5. Enlazar cada coder con sus dongles (left, right) y con la simulación (sim).
// 6. Inicializar los mutex de t_sim: stop_mutex, log_mutex.
// 7. stop = 0 (todavía no hemos parado nada).
// 4. Decisiones que tienes que tomar tú
// - malloc o calloc: si usas calloc, todo empieza a 0
// → acuérdate de poner is_free = 1 a mano (porque 0 significaría "ocupado").
// - La convención izquierda/derecha. Propuesta (fija n = sim->n_coders, indíces 0..n-1):
// Con malloc, la memoria viene con basura y tienes que inicializar todos los campos sí o sí.
// coder[i].left  = &dongles[i]
// coder[i].right = &dongles[(i + 1) % n]
// Ojo con n == 1: ahí left y right apuntan al mismo dongle. Es el caso especial del subject.
// Ahora solo lo enlazas; el lío de "no bloquearse cogiendo dos veces el mismo" llega en M4/M5.
// - Qué devuelve init si algo falla: no puedes dejar memoria suelta. O limpias a mano, o llamas a un cleanup que aguante estados parciales.

int	init(t_sim *sim)
{
	return (0);
}

// 5. Qué tiene que hacer cleanup, en orden inverso
// 1. Destruir los mutex de cada dongle (pthread_mutex_destroy).
// 2. Destruir stop_mutex y log_mutex de t_sim.
// 3. free(sim->dongles).
// 4. free(sim->coders).
// Y pon los punteros a NULL después de liberarlos (buena costumbre: si cleanup se llama dos veces, free(NULL) no explota).
// 6. Trampas concretas para esta función
// - free solo libera el array, no los mutex: los mutex hay que destruirlos explícitamente
// 	 antes del free.
// - No destruyas un mutex que no inicializaste:
// 	 si malloc falló antes de llegar ahí, llamar a pthread_mutex_destroy en basura es un desastre.
// - Los punteros left/right no se liberan: apuntan dentro del array de dongles.
//   Un free sobre ellos sería invalid free (liberar algo que no reservaste). Solo se libera el array.
// - coder[i].sim = sim: no copies t_sim por valor, pasa el puntero. Ellos necesitan la pizarra de verdad.

void	cleanup(t_sim *sim)
{
	...
}

// Qué tiene que hacer init — plan, sin código
// Te lista los pasos en orden y tú escribes:
// 1. sim->coders = malloc(sizeof(t_coder) * sim->n_coders); — y lo mismo para dongles.
// 2. Si cualquiera de los dos malloc es NULL → devolver 0 y no dejar basura.
// 3. Bucle por cada dongle i:
// - pthread_mutex_init(&sim->dongles[i].mutex, NULL);
// - is_free = 1;
// - cooldown_until = 0;
// 4. Bucle por cada coder i:
// - coder_id = i + 1; (el log quiere 1..n)
// - compile_times = 0;
// - last_compile = 0;
// - coder[i].sim = sim;
// - coder[i].left  = &sim->dongles[i];
// - coder[i].right = &sim->dongles[(i + 1) % sim->n_coders];
// 5. pthread_mutex_init(&sim->stop_mutex, NULL); y log_mutex.
// 6. sim->stop = 0;
// 7. return (1);
// Y cleanup, en orden inverso
// 1. Bucle por cada dongle: pthread_mutex_destroy(&sim->dongles[i].mutex);
// 2. pthread_mutex_destroy(&sim->stop_mutex); y log_mutex.
// 3. free(sim->dongles); sim->dongles = NULL;
// 4. free(sim->coders); sim->coders = NULL;
// Dos trampas concretas
// a) init.c son 2 funciones (init, cleanup) → sobra espacio. Puedes crear auxiliares si init se te pasa de 25 líneas. Plan rápido: init probablemente quepa con dos bucles; si no, parte en init_dongles(sim) e init_coders(sim).
// b) left/right NO se liberan. Apuntan dentro del array de dongles. free solo a sim->coders y sim->dongles. Y no liberes dos veces.
// // c) El caso n == 1: con (i + 1) % n = % 1 = 0, entonces left == right == &dongles[0]. Es lo correcto según el subject: una persona, un dongle. Acuérdate en M4.

// Ahora mismo (esta noche):
// 1. init.c: añade #include "codexion.h", borra los comentarios //, y escribe init y cleanup con el plan que te dejé.
// 2. make limpio + norminette coders/ a 0.
// 3. En main, llama a init y cleanup (aunque sea sin arrancar nada) y pasa valgrind.
// Trampas que no se te pueden olvidar:
// - left/right no se liberan (apuntan dentro del array).
// - Los mutex se destroy antes del free del array.
// - is_free = 1 explícito (si usas calloc, el 0 significaría "ocupado").
// - coder_id = i + 1, porque el log numera 1..n.
// Pendiente que queda anotado, no lo pierdas:
// - Overflow en parse_args (aparcado, con un /* TODO */ en check_range).
// - dongle_cooldown = 0 y fases de 0 ms son válidos; solo n_coders y compiles_required deben ser > 0.
// Cuando lo tengas, pégame el init.c y la salida de make/norminette/valgrind y seguimos. Buena sesión.
