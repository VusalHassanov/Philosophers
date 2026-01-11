/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vhasanov <vhasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 04:18:44 by vhasanov          #+#    #+#             */
/*   Updated: 2026/01/11 04:18:55 by vhasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

/* Standard libraries we need */
# include <stdio.h>      // printf
# include <stdlib.h>     // malloc, free
# include <unistd.h>     // usleep, write
# include <pthread.h>    // pthread functions
# include <sys/time.h>   // gettimeofday

/* 
** Structure to hold ALL shared data for the simulation
** This is like the "game rules" that everyone shares
*/
typedef struct s_data
{
	int				nb_philo;        // How many philosophers
	int				time_to_die;     // Max time without eating (ms)
	int				time_to_eat;     // How long eating takes (ms)
	int				time_to_sleep;   // How long sleeping takes (ms)
	int				nb_must_eat;     // Optional: meals needed (-1 if not set)
	int				someone_died;    // Flag: did anyone die?
	long long		start_time;      // When simulation started
	pthread_mutex_t	*forks;          // Array of fork mutexes
	pthread_mutex_t	write_lock;      // Protects printf (no mixed messages)
	pthread_mutex_t	death_lock;      // Protects someone_died flag
	pthread_mutex_t	meal_lock;       // Protects meal-related data
}	t_data;

/* 
** Structure for each individual philosopher
** Each philosopher is a separate thread
*/
typedef struct s_philo
{
	int				id;              // Philosopher number (1 to nb_philo)
	int				meals_eaten;     // How many times they've eaten
	long long		last_meal_time;  // Timestamp of last meal
	pthread_t		thread;          // The thread for this philosopher
	pthread_mutex_t	*left_fork;      // Pointer to left fork mutex
	pthread_mutex_t	*right_fork;     // Pointer to right fork mutex
	t_data			*data;           // Pointer to shared data
}	t_philo;

/* Function prototypes organized by file */

/* utils.c - Helper functions */
int			ft_atoi(const char *str);
long long	get_time(void);
void		ft_usleep(long long time);
void		print_status(t_philo *philo, char *status);

/* init.c - Initialization functions */
int			init_data(t_data *data, int ac, char **av);
int			init_philos(t_philo **philos, t_data *data);
void		init_forks(t_philo *philos, t_data *data);

/* routine.c - What philosophers do */
void		*philo_routine(void *arg);
int			check_death(t_philo *philo);

/* monitor.c - Death checking */
void		*monitor_routine(void *arg);
int			check_all_ate(t_philo *philos, t_data *data);

/* cleanup.c - Memory and resource cleanup */
void		cleanup(t_philo *philos, t_data *data);
void		destroy_mutexes(t_data *data);

#endif
