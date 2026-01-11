/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vhasanov <vhasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 04:51:04 by vhasanov          #+#    #+#             */
/*   Updated: 2026/01/11 04:51:07 by vhasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Destroys all mutexes and frees the fork array
** 
** WHY DESTROY MUTEXES?
** - Every pthread_mutex_init needs a corresponding pthread_mutex_destroy
** - Like: if you open a file, you must close it
** - Prevents resource leaks
** 
** WHAT WE DESTROY:
** 1. All fork mutexes (one for each philosopher)
** 2. write_lock (for printf protection)
** 3. death_lock (for someone_died flag)
** 4. meal_lock (for meal data)
** 5. Free the forks array itself
*/
void	destroy_mutexes(t_data *data)
{
	int	i;

	// Destroy each fork mutex
	i = 0;
	while (i < data->nb_philo)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	
	// Destroy the three shared mutexes
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->death_lock);
	pthread_mutex_destroy(&data->meal_lock);
	
	// Free the malloc'd array
	free(data->forks);
}

/*
** Waits for all threads to finish, then cleans up
** 
** WHY pthread_join?
** - When we created threads with pthread_create, they started running
** - We need to wait for them to finish before cleaning up
** - pthread_join() = "wait for this thread to complete"
** 
** ANALOGY:
** - You start 5 workers doing tasks
** - Before you can close the office, you wait for all workers to finish
** - pthread_join is "waiting for workers to finish"
** 
** WHAT WE CLEAN:
** 1. Join all philosopher threads (wait for them to stop)
** 2. Destroy all mutexes
** 3. Free the philosophers array
*/
void	cleanup(t_philo *philos, t_data *data)
{
	int	i;

	// Wait for all philosopher threads to finish
	// This blocks until each thread has returned from philo_routine()
	i = 0;
	while (i < data->nb_philo)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
	
	// Now all threads are stopped - safe to destroy mutexes
	destroy_mutexes(data);
	
	// Free the philosophers array
	free(philos);
}
