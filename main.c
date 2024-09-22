// cc -Wall -Wextra -Werror -g *.c -o philo -lpthread
// ./philo 4 410 200 200 10
// 1-init
#include "philosophers.h"


int is_philo_dead(t_philo *philo)
{
	if (current_time() - philo->last_eat > philo->data->t_die)
	{
		philo->whatsapp=0;
		return (1);
	}
		
	else
		return (0);
}


void	printdead(t_philo *philo, char *str)
{
		printf("%lld %d %s\n", current_time() - philo->data->t_start,
			philo->n, str);
}

void	check_status(t_philo *philo,t_data	*data)
{
	//t_data	*data;
	int		i;

	//data = (t_data *)args;
	while (data->someone_died == 0)
	{
		i = 0;
		while (i < data->n_phil)
		{
			pthread_mutex_lock(&(data->stop));
			pthread_mutex_lock(&(data->print));
			if (current_time() - philo[i].last_eat > data->t_die)
			{
				data->someone_died = 1;
				if(data->n_eat<1 || philo[i].eat_counter < data->n_eat)
					{
						printdead(data->philo, " died");
					}
				pthread_mutex_unlock(&(data->stop));
				pthread_mutex_unlock(&(data->print));
				break ;
			}
			pthread_mutex_unlock(&(data->stop));
			pthread_mutex_unlock(&(data->print));
			pause_time(1);
			i++;
		}
	}
}


// 2-philo
void	start_thread(t_data *data)
{
	int				i;

	i = 0;

	while (i < data->n_phil)
	{
		if (pthread_create(&(data->philo[i].tid), NULL,
				&routin, &(data->philo[i])) != 0)
		{
			perror("pthread_create");
			return ;
		}
		i++;
	}
	check_status((t_philo *)(data->philo),data);
	i = 0;
	while (i < data->n_phil)
	{
		if(pthread_join(data->philo[i].tid, NULL) !=0)
		{
			perror("pthread_create");
			return ;
		}
		i++;
	}
}

t_forks	*take_first_fork(int i, t_data *data)
{
		if (i == data->n_phil - 1)
			return (&(data->forks[0]));
		else
			return (&(data->forks[i]));
	return (NULL);
}

t_forks	*take_second_fork(int i, t_data *data)
{
		if (i == data->n_phil - 1)
		{
			if (data->n_phil == 1)
				return (NULL);
			else
				return (&(data->forks[i]));
		}
		else
			return (&(data->forks[i + 1]));
	return (NULL);
}

void	initialize_philo(t_data *data)
{
	int				i;

	i = 0;
	while (i < data->n_phil)
	{
		data->philo[i].n = i + 1;
		data->philo[i].eat_counter = 0;
		data->philo[i].whatsapp = 1;
		data->philo[i].last_eat = current_time();
		i++;
	}
	i=0;
	while (i < data->n_phil)
	{
		pthread_mutex_init(&(data->forks[i].theforks), NULL);
		data->forks[i].canbused = 1;
		i++;
	}
	i=0;
	while (i < data->n_phil)
	{
		data->philo[i].fork_right=take_first_fork(i, data);
		data->philo[i].fork_left=take_second_fork(i, data);
		data->philo[i].data = data;
		 i++;
	}
	
	start_thread(data);
}

void	free_datas( t_data *data, t_philo *philo)
{
	int i;
	
	i=0;
	pthread_mutex_destroy(&(data->stop));
	pthread_mutex_destroy(&(data->print));
	while (i < data->n_phil)
	{
		pthread_mutex_destroy(&(data->forks[i].theforks));
		i++;
	}
	free(data->forks);
	free(data);
	free(philo);
}

void	parse_variable(char **argv)
{
	t_data			*data;

	data = malloc(sizeof(t_data));
	if (!data)
		perror("Memory allocation failed");
	pthread_mutex_init(&(data->print), NULL);
	pthread_mutex_init(&data->stop, NULL);
	data->n_phil = ft_atoi(argv[1]);
	data->t_die = ft_atoi(argv[2]);
	data->t_eat = ft_atoi(argv[3]);
	data->t_sleep = ft_atoi(argv[4]);
	data->n_eat = 0;
	data->someone_died = 0;
	data->philo = malloc(sizeof(t_philo) * data->n_phil);
	if (!data->philo)
		perror("Memory allocation failed");
	data->forks = malloc(sizeof(t_forks) * data->n_phil);
	if (!data->forks)
		perror("Memory allocation failed");
	if (argv[5])
		data->n_eat = ft_atoi(argv[5]);
	if (argv[5] && data->n_eat == 0)
		return ;
	data->t_start = current_time();
	initialize_philo(data);
	free_datas( data, data->philo);
}

int	main(int argc, char **argv)
{
	if (argc != 5 && argc != 6)
		return (1);
	if (!is_valid_integer(argv))
		return (1);
	parse_variable(argv);
	return (0);
}

//3-print
long long	current_time(void)
{
	struct timeval	current;

	gettimeofday(&current, NULL);
	return (current.tv_sec * 1000 + current.tv_usec / 1000);
}


void	printeat(t_philo *philo, char *str)
{
		pthread_mutex_lock(&(philo->data->print));
		philo->last_eat = current_time();
		printf("%lld %d %s\n", current_time() - philo->data->t_start,
			philo->n, str);
		pthread_mutex_unlock(&(philo->data->print));
}

void	print(t_philo *philo, char *str)
{
		pthread_mutex_lock(&(philo->data->print));
		if (!philo->data->someone_died)
			printf("%lld %d %s\n", current_time() - philo->data->t_start,
			philo->n, str);
		pthread_mutex_unlock(&(philo->data->print));
}

void	pause_time(int t)
{
	long int	time;

	time = current_time();
	while (current_time() - time < t)
		usleep(t / 11);
}

void	taking_right_fork(t_philo *philo)
{
	pthread_mutex_lock(&(philo->fork_right->theforks));
	print(philo, " has taken a fork");
}
void	taking_left_fork(t_philo *philo)
{
	pthread_mutex_lock(&(philo->fork_left->theforks));
	print(philo, " has taken a fork");
}

int	eating2(t_philo	*d)
{
	if (d->fork_left)
	{
		taking_left_fork(d);
		pthread_mutex_lock(&(d->data->stop));
		if (d->data->someone_died)
		{
			pthread_mutex_unlock(&(d->data->stop));
			pthread_mutex_unlock(&(d->fork_right->theforks));
			pthread_mutex_unlock(&(d->fork_left->theforks));
			return (0);
		}
		printeat(d, " is eating");
		d->eat_counter++;
		pthread_mutex_unlock(&(d->data->stop));
		pause_time(d->data->t_eat);
	}
	pthread_mutex_unlock(&(d->fork_right->theforks));
	if (d->fork_left)
		pthread_mutex_unlock(&(d->fork_left->theforks));
	return (1);
}

void	*routin(void *arg)
{
	t_philo		*philo1;

	philo1 = (t_philo *)arg;
	if ((philo1->n) % 2 == 0)
		pause_time(philo1->data->t_eat / 11);

	while (!(is_philo_dead(philo1)) && (philo1->whatsapp) &&  ((philo1->data->n_eat < 1) || (philo1->eat_counter < philo1->data->n_eat)))
	{
		taking_right_fork(philo1);
		if(!eating2(philo1))
			return (NULL);
		if (philo1->eat_counter == philo1->data->n_eat)
			return (NULL);
		print(philo1, " is sleeping");
		pause_time(philo1->data->t_sleep);
		print(philo1, " is thinking");
	}
	return (NULL);
}

//5-utils

int	is_digit(char *args)
{
	int	i;

	i = 0;
	if ((args[i] == '+') && ft_isdigit(args[i + 1]))
		i++;
	while (args[i])
	{
		while (args[i] == ' ' || args[i] == '\t')
			i++;
		if ((args[i] == '+') && !(args[i - 1]))
			i++;
		if (!ft_isdigit(args[i]))
			return (0);
		i++;
	}
	return (1);
}

int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int	ft_atoi(const char *str)
{
	int	nb;
	int	flag;
	int	i;

	nb = 0;
	i = 0;
	flag = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			flag *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		nb = (nb * 10) + (str[i] - 48);
		i++;
	}
	return (nb * flag);
}

int	is_valid_integer(char **args)
{
	int		i;

	i = 2;
	while (args[i])
	{
		if (!is_digit(args[i]))
			return (0);
		i++;
	}
	return (1);
}

