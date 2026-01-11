/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vhasanov <vhasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 04:32:17 by vhasanov          #+#    #+#             */
/*   Updated: 2026/01/11 04:32:23 by vhasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Initializes the main data structure
** Parses arguments and creates all the mutexes we need
** 
** WHAT WE DO:
** 1. Parse command line arguments into data structure
** 2. Allocate array of fork mutexes
** 3. Initialize each fork mutex
** 4. Initialize the three shared mutexes
*/
int	init_data(t_data *data, int ac, char **av)
{
	int	i;

	// Parse arguments from strings to integers
	data->nb_philo = ft_atoi(av[1]);          // How many philosophers
	data->time_to_die = ft_atoi(av[2]);       // Max time without eating
	data->time_to_eat = ft_atoi(av[3]);       // How long to eat
	data->time_to_sleep = ft_atoi(av[4]);     // How long to sleep
	data->nb_must_eat = -1;                   // Default: no meal limit
	if (ac == 6)                               // If 5th argument provided
		data->nb_must_eat = ft_atoi(av[5]);   // Set meal limit
	
	// Initialize flags
	data->someone_died = 0;                   // Nobody died yet
	data->start_time = get_time();            // Record start time (for timestamps)
	
	// Allocate array of mutexes (one for each fork)
	// If we have 5 philosophers, we need 5 forks
	data->forks = malloc(sizeof(pthread_mutex_t) * data->nb_philo);
	if (!data->forks)
		return (0);  // Malloc failed
	
	// Initialize each fork mutex
	i = 0;
	while (i < data->nb_philo)
	{
		if (pthread_mutex_init(&data->forks[i], NULL))
		{
			// If init fails, destroy already created mutexes
			while (--i >= 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			return (0);
		}
		i++;
	}
	
	// Initialize the three shared mutexes
	pthread_mutex_init(&data->write_lock, NULL);   // For printf
	pthread_mutex_init(&data->death_lock, NULL);   // For someone_died flag
	pthread_mutex_init(&data->meal_lock, NULL);    // For meal times/counts
	
	return (1);  // Success!
}

/*
** Creates and initializes the array of philosophers
** 
** WHAT WE DO:
** 1. Allocate memory for array of philosophers
** 2. Set each philosopher's initial values
*/
int	init_philos(t_philo **philos, t_data *data)
{
	int	i;

	// Allocate array of philosophers
	*philos = malloc(sizeof(t_philo) * data->nb_philo);
	if (!*philos)
		return (0);  // Malloc failed
	
	// Initialize each philosopher
	i = 0;
	while (i < data->nb_philo)
	{
		(*philos)[i].id = i + 1;                    // ID starts from 1
		(*philos)[i].meals_eaten = 0;               // Haven't eaten yet
		(*philos)[i].last_meal_time = data->start_time;  // "Ate" at start
		(*philos)[i].data = data;                   // Pointer to shared data
		i++;
	}
	return (1);  // Success!
}

/*
** Assigns fork pointers to each philosopher
** 
** THE SETUP (example with 5 philosophers):
**   Forks:  [0]  [1]  [2]  [3]  [4]
**   Philos:  1    2    3    4    5
** 
** Philosopher 1: left=fork[0], right=fork[1]
** Philosopher 2: left=fork[1], right=fork[2]
** Philosopher 3: left=fork[2], right=fork[3]
** Philosopher 4: left=fork[3], right=fork[4]
** Philosopher 5: left=fork[4], right=fork[0]  <- wraps around!
** 
** IMPORTANT: It's a circular table!
** - Last philosopher's right fork is the first fork
** - That's why we use modulo: (i + 1) % nb_philo
*/
void	init_forks(t_philo *philos, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_philo)
	{
		// Left fork is the one at current index
		philos[i].left_fork = &data->forks[i];
		
		// Right fork is the next one (wraps around at the end)
		philos[i].right_fork = &data->forks[(i + 1) % data->nb_philo];
		
		i++;
	}
}
