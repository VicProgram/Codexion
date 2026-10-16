/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 13:37:19 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/16 19:01:26 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char** argv){

	if	(argc != 9){
        printf(stderr, "Error en el número de argumentos\n");
        return (1);
    }

    if	(strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0){
		printf(stderr, "Error: Scheduler must be 'fifo' or 'edf'\n");
		return (1);
	}
	int i;
	i = 1;
	while (i < argc){
		if(argv[i][0] == "-"){
			printf(stderr, "Error: Negative numbers are not forbidden");
			return (1);
        }
        int value;
        value = atoi(argv[i]);
		if (value < 0){
				printf(stderr, "Can't use negative numbers\n");
				return (1);
			}
			i++;
    }
        
    return 0;
}
