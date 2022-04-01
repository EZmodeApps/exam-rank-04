#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct d_data
{
	char	*args[1000];
	int		next;
	int		pipe[2];
}	t_data;

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void	ft_err(char *str1, char *str2)
{
	write(2, str1, ft_strlen(str1));
	if (str2)
		write(2, str2, ft_strlen(str2));
	write(2, "\n", 1);
}

void	ft_parse(t_data *data, char **argv)
{
	int	i;
	int	j;
	int	c;

	c = 0;
	i = 0;
	j = 0;
	while (argv[++i])
	{
		data[c].next = 1;
		if (!strcmp(argv[i], "|"))
		{
			data[c].next++;
			j = 0;
			c++;
		}
		else if (!strcmp(argv[i], ";"))
		{
			j = 0;
			c++;
		}
		else
		{
			data[c].args[j] = argv[i];
			j++;
		}
	}
}

void	ft_run(t_data *data, char **env)
{
	int	i;
	int	res;

	i = -1;
	while (data[++i].next)
	{
		if (data[i].args[0] && !strcmp(data[i].args[0], "cd"))
		{
			if (!data[i].args[1] || data[i].args[2] || data[i].args[1][0] == '-')
				ft_err("error: cd: bad arguments", NULL);
			else if (chdir(data[i].args[1]) == -1)
				ft_err("error: cd: cannot change directory to ", data[i].args[1]);
		}
		else
		{
			if (data[i].next == 2)
				pipe(data[i].pipe);
			res = fork();
			if (!res)
			{
				if (i && data[i - 1].next == 2)
					dup2(data[i - 1].pipe[0], 0);
				if (data[i].pipe[1])
					dup2(data[i].pipe[1], 1);
				if (data[i].args[0] && execve(data[i].args[0], data[i].args, env))
					ft_err("error: cannot execute ", data[i].args[0]);
				exit(1);
			}
			else
			{
				waitpid(res, NULL, 0);
				if (i && data[i - 1].next == 2)
					close(data[i - 1].pipe[0]);
				if (data[i].pipe[1])
					close(data[i].pipe[1]);
			}
		}
	}
}

int	main(int argc, char **argv, char **env)
{
	t_data	*data;

	data = (t_data *)malloc(sizeof(t_data) * 1000);
	if (!data)
	{
		ft_err("error: fatal", NULL);
		exit(1);
	}
	if (argc > 1)
		ft_parse(data, argv);
	ft_run(data, env);
	return (0);
}
