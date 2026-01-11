#include "philo.h"

/*
** Checks if someone has died or simulation ended
*/
int	check_death(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->death_lock);
	if (philo->data->someone_died)
	{
		pthread_mutex_unlock(&philo->data->death_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->death_lock);
	return (0);
}

/*
** Check if this philosopher finished their required meals
*/
int	check_meals_done(t_philo *philo)
{
	int	done;

	done = 0;
	if (philo->data->nb_must_eat == -1)
		return (0);
	
	pthread_mutex_lock(&philo->data->meal_lock);
	if (philo->meals_eaten >= philo->data->nb_must_eat)
		done = 1;
	pthread_mutex_unlock(&philo->data->meal_lock);
	
	return (done);
}

/*
** Philosopher picks up forks and eats
*/
void	philo_eat(t_philo *philo)
{
	// Even philosophers pick forks in different order (deadlock prevention)
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a fork");
	}
	
	// Now holding both forks - can eat!
	print_status(philo, "is eating");
	
	// Update last meal time and meal count
	pthread_mutex_lock(&philo->data->meal_lock);
	philo->last_meal_time = get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->meal_lock);
	
	// Actually eat
	ft_usleep(philo->data->time_to_eat);
	
	// Put forks back
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

/*
** Philosopher sleeps
*/
void	philo_sleep(t_philo *philo)
{
	print_status(philo, "is sleeping");
	ft_usleep(philo->data->time_to_sleep);
}

/*
** Calculate optimal thinking time to prevent starvation
** For odd numbers of philosophers, we need thinking time to space them out
*/
long long	calculate_think_time(t_philo *philo)
{
	long long	think_time;
	
	// For odd numbers, we need thinking to space out eating
	if (philo->data->nb_philo % 2 == 1)
	{
		// Basic formula: ensure we don't create a cycle that's too long
		think_time = (philo->data->time_to_eat * 2) - philo->data->time_to_sleep;
		
		// But limit it to avoid approaching time_to_die
		// Leave margin: cycle should be < time_to_die - time_to_eat
		if (think_time < 0)
			think_time = 0;
		
		// Critical: ensure eat + sleep + think < time_to_die - margin
		// margin = time_to_eat (time needed to grab forks and start eating again)
		if (philo->data->time_to_eat + philo->data->time_to_sleep + think_time 
			>= philo->data->time_to_die - philo->data->time_to_eat)
		{
			think_time = philo->data->time_to_die - philo->data->time_to_eat 
				- philo->data->time_to_sleep - 20;
			if (think_time < 0)
				think_time = 0;
		}
		
		return (think_time);
	}
	return (0);
}

/*
** Philosopher thinks
*/
void	philo_think(t_philo *philo)
{
	long long	think_time;
	
	print_status(philo, "is thinking");
	
	think_time = calculate_think_time(philo);
	
	if (think_time > 0)
		ft_usleep(think_time);
	else
		usleep(100);  // Tiny delay to reduce CPU usage
}

/*
** Main philosopher routine
*/
void	*philo_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	
	// EDGE CASE: Only one philosopher
	if (philo->data->nb_philo == 1)
	{
		print_status(philo, "has taken a fork");
		ft_usleep(philo->data->time_to_die);
		return (NULL);
	}
	
	// Stagger start: even philosophers wait to desynchronize
	if (philo->id % 2 == 0)
	{
		// Wait almost a full eating duration for even philosophers
		ft_usleep(philo->data->time_to_eat - 1);
	}
	
	// Main loop: eat → sleep → think → repeat
	while (!check_death(philo))
	{
		// CRITICAL: Check if we've eaten enough BEFORE attempting to eat
		// This prevents the race where we eat one extra meal
		if (check_meals_done(philo))
			break;
		
		philo_eat(philo);
		
		// Check after eating
		if (check_death(philo) || check_meals_done(philo))
			break;
		
		philo_sleep(philo);
		
		if (check_death(philo) || check_meals_done(philo))
			break;
		
		philo_think(philo);
	}
	
	return (NULL);
}