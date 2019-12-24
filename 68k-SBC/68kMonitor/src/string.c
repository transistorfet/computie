
#include "string.h"


int strlen(const char *str)
{
	int i = -1;
	while (str[++i] != '\0') ;
	return(i);
}

/*
int strnlen(const char *str, int len)
{
	int i;
	for (i = 0;i < len;i++)
		if (str[i] == '\0')
			break;
	return(i);
}
*/

int strcpy(char *dest, const char *src)
{
	int i = 0;
	do {
		dest[i] = src[i];
	} while (src[i++] != '\0');
	return(i);
}

int strncpy(char *dest, const char *src, int len)
{
	int i;
	for (i = 0;i < len;i++) {
		dest[i] = src[i];
		if (src[i] == '\0')
			break;
	}
	return(i);
}

// The only newly written version
int strcmp(const char *str1, const char *str2) {
	while (1) {
		if (*str1 > *str2)
			return 1;
		else if (*str1 < *str2)
			return -1;
		else if (*str1 == *str2 && *str1 == '\0')
			return 0;
		str1++;
		str2++;
	}
}

/*
// Original version from 2009
int strcmp(const char *str1, const char *str2)
{
	int i = 0;

	if (!str1 || !str2)
		return(-1);

	while (str1[i] != '\0' && str2[i] != '\0') {
		if (str1[i] != str2[i]) {
			if (str1[i] > str2[i])
				return(1);
			else
				return(-1);
		}
		i++;
	}
	if (str1[i] > str2[i])
		return(1);
	else if (str1[i] < str2[i])
		return(-1);
	return(0);
}
*/

/*
int strncmp(const char *str1, const char *str2, int len)
{
	int i;

	if (!str1 || !str2)
		return(-1);

	for (i = 0;i < len;i++) {
		if (str1[i] == '\0' && str2[i] == '\0')
			break;
	 	else if (str1[i] != str2[i] || str1[i] == '\0' || str2[i] == '\0') {
			if (str1[i] > str2[i])
				return(1);
			else
				return(-1);
		}
	}
	return(0);
}

int strcnt(const char *str, char ch)
{
	int i = -1, ret = 0;

	while (str[++i] != '\0')
		if (str[i] == ch)
			ret++;
	return(ret);
}

char *strstr(const char *str1, const char *str2)
{
	int j, i = 0;

	if (str2[0] == '\0')
		return(str1);
	while (str1[i] != '\0') {
		j = 0;
		while (str1[i + j] == str2[j]) {
			if (str2[j + 1] == '\0')
				return(&str1[i]);
			j++;
		}
		i++;
	}
	return(0);
}

char *strchr(const char *str, const char ch)
{
	int i = -1;

	while (str[++i] != '\0')
		if (str[i] == ch)
			return(&str[i]);
	return(NULL);
}

char *strrchr(const char *str, const char ch)
{
	int i;

	i = strlen(str);

	while (--i > 0)
		if (str[i] == ch)
			return(&str[i]);
	return(NULL);
}
*/

