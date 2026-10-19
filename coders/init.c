/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 20:48:01 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/19 20:53:41 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void	cleaunp(t_sim *sim)
{
	...
}
