/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vhasanov <vhasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 04:20:15 by vhasanov          #+#    #+#             */
/*   Updated: 2026/01/11 04:20:20 by vhasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Validates command line arguments
** We need 4 or 5 arguments (plus program name = 5 or 6 total)
** All must be positive numbers
*/
int	validate_args(int ac, char **av)
{
	int	i;
	int	j;

	// Check if we have correct number of arguments
	if (ac < 5 || ac > 6)
	{
		printf("Error: Wrong number of arguments\n");
		printf("Usage: ./philo number_of_philosophers time_to_die ");
		printf("time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (0);
	}
	
	// Check each argument (skip av[0] which is program name)
	i = 1;
	while (i < ac)
	{
		j = 0;
		// Check if every character is a digit
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				printf("Error: Arguments must be positive numbers\n");
				return (0);
			}
			j++;
		}
		// Convert to int and check if it's positive
		if (ft_atoi(av[i]) <= 0)
		{
			printf("Error: Arguments must be positive numbers\n");
			return (0);
		}
		i++;
	}
	return (1);  // All checks passed
}

/*
** Main function - the heart of our program
** Flow:
** 1. Validate arguments
** 2. Initialize data (parse args, create mutexes)
** 3. Initialize philosophers (create philosopher structures)
** 4. Assign forks to philosophers
** 5. Create threads for each philosopher
** 6. Monitor for death/completion
** 7. Clean up
*/
int	main(int ac, char **av)
{
	t_data	data;      // Shared data structure
	t_philo	*philos;   // Array of philosophers
	int		i;

	// Step 1: Validate arguments
	if (!validate_args(ac, av))
		return (1);
	
	// Step 2: Initialize data structure (parse arguments, create mutexes)
	if (!init_data(&data, ac, av))
		return (1);
	
	// Step 3: Allocate and initialize philosopher structures
	if (!init_philos(&philos, &data))
	{
		destroy_mutexes(&data);  // Clean up mutexes if philo init fails
		return (1);
	}
	
	// Step 4: Assign fork pointers to each philosopher
	init_forks(philos, &data);
	
	// Step 5: Create a thread for each philosopher
	i = 0;
	while (i < data.nb_philo)
	{
		// pthread_create starts a new thread running philo_routine
		if (pthread_create(&philos[i].thread, NULL, philo_routine, &philos[i]))
		{
			printf("Error: Thread creation failed\n");
			cleanup(philos, &data);
			return (1);
		}
		i++;
	}
	
	// Step 6: Main thread monitors for death or completion
	// This function blocks until someone dies or everyone finishes eating
	monitor_routine(philos);
	
	// Step 7: Clean up everything
	cleanup(philos, &data);
	
	return (0);
}
