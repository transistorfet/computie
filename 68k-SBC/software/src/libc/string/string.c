
#include <string.h>


/*
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

