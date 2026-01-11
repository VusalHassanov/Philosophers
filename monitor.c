#include "philo.h"

/*
** Checks if all philosophers have eaten enough times
** IMPORTANT: Check this BEFORE death check to stop simulation on time
*/
int	check_all_ate(t_philo *philos, t_data *data)
{
	int	i;
	int	finished;

	// If no meal requirement, skip this check
	if (data->nb_must_eat == -1)
		return (0);
	
	finished = 0;
	i = 0;
	
	// Count how many philosophers reached meal goal
	while (i < data->nb_philo)
	{
		pthread_mutex_lock(&data->meal_lock);
		if (philos[i].meals_eaten >= data->nb_must_eat)
			finished++;
		pthread_mutex_unlock(&data->meal_lock);
		i++;
	}
	
	// If everyone finished eating...
	if (finished == data->nb_philo)
	{
		pthread_mutex_lock(&data->death_lock);
		data->someone_died = 1;  // Set flag to stop simulation
		pthread_mutex_unlock(&data->death_lock);
		return (1);  // Simulation complete!
	}
	
	return (0);  // Not everyone finished yet
}

/*
** Checks if a specific philosopher has starved to death
*/
int	check_philosopher_death(t_philo *philo)
{
	long long	time_since_meal;

	// Get time since last meal (protected by mutex)
	pthread_mutex_lock(&philo->data->meal_lock);
	time_since_meal = get_time() - philo->last_meal_time;
	pthread_mutex_unlock(&philo->data->meal_lock);
	
	// Check if they starved
	if (time_since_meal > philo->data->time_to_die)
	{
		// Lock death_lock to check and set someone_died
		pthread_mutex_lock(&philo->data->death_lock);
		if (!philo->data->someone_died)  // Only first death counts
		{
			philo->data->someone_died = 1;  // Mark that someone died
			pthread_mutex_unlock(&philo->data->death_lock);
			
			// Print death message
			pthread_mutex_lock(&philo->data->write_lock);
			printf("%lld %d died\n", 
				get_time() - philo->data->start_time, philo->id);
			pthread_mutex_unlock(&philo->data->write_lock);
			
			return (1);  // Someone died
		}
		pthread_mutex_unlock(&philo->data->death_lock);
	}
	
	return (0);  // Still alive
}

/*
** Main monitoring routine - runs in the main thread
** FIXED: Check meals FIRST to stop simulation immediately when goal is reached
*/
void	*monitor_routine(void *arg)
{
	t_philo	*philos;
	int		i;

	philos = (t_philo *)arg;
	
	// Small delay to let philosophers start
	usleep(1000);
	
	// Infinite loop until someone dies or everyone finishes
	while (1)
	{
		// IMPORTANT: Check meal count FIRST
		// This prevents extra meals after reaching the goal
		if (check_all_ate(philos, philos[0].data))
			return (NULL);  // Everyone finished - simulation over
		
		// Then check for deaths
		i = 0;
		while (i < philos[0].data->nb_philo)
		{
			if (check_philosopher_death(&philos[i]))
				return (NULL);  // Someone died - simulation over
			i++;
		}
		
		// Sleep briefly to not waste CPU
		usleep(1000);  // 1 millisecond
	}
	
	return (NULL);
}