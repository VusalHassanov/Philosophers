/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vhasanov <vhasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 04:28:43 by vhasanov          #+#    #+#             */
/*   Updated: 2026/01/11 04:28:50 by vhasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Converts a string to an integer
** Example: "123" -> 123
** We need this to parse command line arguments
*/
int	ft_atoi(const char *str)
{
	int	result;
	int	i;

	result = 0;
	i = 0;
	// Read each digit and build the number
	// "123" -> 0*10+1=1 -> 1*10+2=12 -> 12*10+3=123
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result);
}

/*
** Gets current time in MILLISECONDS
** We need this to track when philosophers eat and when they die
** 
** gettimeofday gives us:
** - tv_sec: seconds since Jan 1, 1970
** - tv_usec: microseconds (1/1,000,000 of a second)
** 
** We convert to milliseconds: seconds*1000 + microseconds/1000
*/
long long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** More precise sleep function than usleep()
** 
** WHY NOT JUST USE usleep()?
** - usleep() can be inaccurate, especially for small values
** - We need precise timing for eating/sleeping
** 
** HOW IT WORKS:
** - Get start time
** - Keep checking current time until enough time has passed
** - Sleep for 500 microseconds between checks (doesn't burn CPU)
*/
void	ft_usleep(long long time)
{
	long long	start;

	start = get_time();
	while ((get_time() - start) < time)
		usleep(500);  // Sleep a tiny bit to not waste CPU
}

/*
** Prints philosopher status messages in a thread-safe way
** 
** PROBLEM: Multiple threads calling printf at the same time = mixed messages!
** Example without locks:
**   Thread 1: "100 1 is "
**   Thread 2: "100 2 has taken a fork"
**   Thread 1: "eating"
**   Result: "100 1 is 100 2 has taken a forkeating" (MESS!)
** 
** SOLUTION: Use mutex to lock printing
** - Only one thread can print at a time
** 
** ALSO: Check if someone died
** - If someone died, don't print anymore (simulation is over)
** - We check death_lock FIRST to avoid printing after death
*/
void	print_status(t_philo *philo, char *status)
{
	long long	timestamp;

	// First check if someone died
	pthread_mutex_lock(&philo->data->death_lock);
	if (!philo->data->someone_died)
	{
		pthread_mutex_unlock(&philo->data->death_lock);
		
		// Now safe to print
		pthread_mutex_lock(&philo->data->write_lock);
		timestamp = get_time() - philo->data->start_time;
		printf("%lld %d %s\n", timestamp, philo->id, status);
		pthread_mutex_unlock(&philo->data->write_lock);
	}
	else
		pthread_mutex_unlock(&philo->data->death_lock);
}
