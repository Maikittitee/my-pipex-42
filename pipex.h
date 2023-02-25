/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maikittitee <maikittitee@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 19:22:43 by maikittitee       #+#    #+#             */
/*   Updated: 2023/02/25 20:28:41 by maikittitee      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/errno.h>
# include "include/libft.h"
# include "include/ft_printf.h"

char *ft_strjoin_free(char *s1, char *s2);
char *ft_strjoin_free_back(char *s1, char *s2);
char **get_path(char **env);

# define ARG_ERR 1
# define FORK_ERR 2
# define PIPE_ERR 3
# define FILE_ERR 4
# define CMD_ERR 5

typedef struct s_pipex{
    int pid1;
    int pid2;
	int access_flag1;
	int	access_flag2;
    char **cmd1;
    char **cmd2;

    char **path;
} t_pipex;


#endif

