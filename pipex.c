/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maikittitee <maikittitee@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 15:28:05 by maikittitee       #+#    #+#             */
/*   Updated: 2023/02/27 10:31:00 by maikittitee      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//EXE		./pipex infile cmd1 cmd2 outfile
//ARG		   0	  1		2	 3		4

//fd[0] is for read from the pipe
//fd[1] is for write to the  pipe


#include "pipex.h"

void	ft_displayerr(int err, char *msg, int errnum)
{
	if (err == ARG_ERR)
		ft_putstr_fd("Invalid number of argument.\n", 2);
	else if (err == FORK_ERR)
		perror("Fork error :");
	else if (err == PIPE_ERR)
		perror("Pipe error :");
	else if (err == FILE_ERR)
		msg = ft_strjoin(msg, ": no such file or directory\n");
	else if (err == CMD_ERR)
		msg = ft_strjoin(msg, ": command not found\n");
	if (msg && (err == FILE_ERR || err == CMD_ERR))
	{
		ft_putstr_fd(msg, STDERR_FILENO);
		free(msg);
	}
	exit (errnum);
}

void	ft_double_free(char **s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		free(s[i]);
		i++;
	}
	free(s);
}

void	ft_free_pipex(t_pipex *pipex)
{
		ft_double_free(pipex->cmd1);
		ft_double_free(pipex->cmd2);
		ft_double_free(pipex->path);
}

int	is_access_cmd(char **path, char **cmd, char *pure_cmd)
{
	char *temp;
	int	i;
	int	access_flag;

	i = 0;
	access_flag = 0;
	temp = cmd[0];
	while (!access_flag && path[i])
	{
		cmd[0] = ft_strjoin(path[i], pure_cmd);
		if (temp)
			free(temp);
		temp = cmd[0];
		if (access(cmd[0], F_OK) == 0) 
			return (1);
		i++;
	}
	return (0);

}

void	ft_find_cmd(t_pipex *pipex, char **av)
{
	//int	i;
	char	*pure_cmd[2];
	//char	*temp;
	
	//temp = NULL;
	pipex->access_flag1 = 0;
	pipex->access_flag2 = 0;
	//i = 0;
	pipex->cmd1 = ft_split(av[2],' ');
	pipex->cmd2 = ft_split(av[3],' ');
	pure_cmd[0] = ft_strdup((pipex->cmd1)[0]);
	pure_cmd[1] = ft_strdup((pipex->cmd2)[0]);
	//temp = (pipex->cmd1)[0];
	if (access((pipex->cmd1)[0], F_OK) == 0)
		pipex->access_flag1 = 1; 
	if (access((pipex->cmd2)[0], F_OK) == 0)
		pipex->access_flag2 = 1;
	// if Leak --> del this 4 line
	if (!pipex->access_flag1)
		pipex->access_flag1 = is_access_cmd(pipex->path,pipex->cmd1, pure_cmd[0]);
	if (!pipex->access_flag2)
		pipex->access_flag2 = is_access_cmd(pipex->path,pipex->cmd2, pure_cmd[1]);
		
	// and uncomment this
	// while (!pipex->access_flag1 && (pipex->path)[i])
	// {
	// 	(pipex->cmd1)[0] = ft_strjoin((pipex->path)[i],pure_cmd[0]);
	// 	if (temp)
	// 		free(temp);
	// 	temp = (pipex->cmd1)[0];
	// 	if (access((pipex->cmd1)[0], F_OK) == 0) 
	// 		pipex->access_flag1 = 1; 
	// 	i++;
	// }
	// i = 0;
	// temp = (pipex->cmd2)[0];
	// while (!pipex->access_flag2 && (pipex->path)[i])
	// {
	// 	(pipex->cmd2)[0] = ft_strjoin((pipex->path)[i], pure_cmd[1]);
	// 	if (temp)
	// 		free(temp);
	// 	temp = (pipex->cmd2)[0];
	// 	if (access((pipex->cmd2)[0], F_OK) == 0) 
	// 		pipex->access_flag2 = 1; 
	// 	i++;
	// }
	if (pure_cmd[0])
		free(pure_cmd[0]);
	if (pure_cmd[1])
		free(pure_cmd[1]);


}

void	ft_child1_process(t_pipex pipex, char **av, char **env, int fd[2])
{
	int	infile_fd;

	infile_fd = open(av[1], O_RDONLY);
	if (infile_fd < 0)
	{
		ft_double_free(pipex.path);
		ft_displayerr(FILE_ERR, av[1], EXIT_FAILURE);
	}
	if (!pipex.access_flag1)
		ft_displayerr(CMD_ERR, av[3], 127);
		
	dup2(infile_fd, STDIN_FILENO);
	dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	close(fd[0]);
	close(infile_fd);
	execve((pipex.cmd1)[0], pipex.cmd1, env);
}

void	ft_child2_process(t_pipex pipex, char **av, char **env, int fd[2])
{
	int	outfile_fd;

	outfile_fd = open(av[4], O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (outfile_fd < 0)
	{	
		ft_double_free(pipex.path);
		ft_displayerr(FILE_ERR, av[4], EXIT_FAILURE);
	}
	if (!pipex.access_flag2)
		ft_displayerr(CMD_ERR, av[3], 127);
	dup2(fd[0], STDIN_FILENO);
	dup2(outfile_fd, STDOUT_FILENO);
	close(fd[0]);
	close(fd[1]);
	close(outfile_fd);
	execve((pipex.cmd2)[0], pipex.cmd2, env);
}


int	main(int ac, char **av, char **env)
{
	int		fd[2];
	t_pipex pipex;
	int	status;
	

	pipex.cmd1 = NULL;
	pipex.cmd2 = NULL;
	if (ac != 5)
	{
		ft_putstr_fd("This program take 4 argument", 2);
		exit(1);
	}
	pipex.path = get_path(env);
	ft_find_cmd(&pipex, av);
	if (pipe(fd) != 0)
	 		ft_displayerr(PIPE_ERR, NULL , errno);
	pipex.pid1 = fork();
	if (pipex.pid1 == -1)
	{
		ft_double_free(pipex.path);
		ft_displayerr(FORK_ERR, NULL, errno);
	}
	if (pipex.pid1 == 0)
		ft_child1_process(pipex, av, env, fd);
	pipex.pid2 = fork();
	if (pipex.pid2 == 0)
		ft_child2_process(pipex, av, env, fd);
	close(fd[0]);
	close(fd[1]);	
	waitpid(pipex.pid1,NULL,0);
	waitpid(pipex.pid2, &status ,0);
	ft_double_free(pipex.path);
	ft_double_free(pipex.cmd1);
	ft_double_free(pipex.cmd2);

	return (status >> 8);
	
}
