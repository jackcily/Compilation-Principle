#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define  MAXCHAR 50  //每个字符的最大值
#define  MAXNUM  100 //栈增长的高度
#define MAXLEN   100
#define  TRUE  1
#define  FALSE 0

#define YES 1
#define NO -1
#define NOTSURE  0

#define SHIFT 1
#define REDUCE 2
#define GOTO 3
#define ACC 4

//用于存储正规式
typedef struct Regu
{
	char**pregu;
	int num;
}Regu, *pRegu;


//用于存储每一个闭包   一个项目状态
typedef struct Closure
{
	int num;
	char **preguc;
}Closure, *pClosure;
//用于存储项目集
typedef struct Project
{
	int num;
	pClosure pclosure;
}Project, *pProject;


//用于存储分析表的每一行
typedef struct Parse
{
	int num;
	char**pproceed;
}Parse, *pParse;
//用于存储分析表
typedef struct Analyze
{
	int num;
	pParse pparse;
}Analyze, *pAnalyze;

//符号栈
typedef struct
{
	char *symbol;
	int num;
	int top;
}StackSymbol,*pStackSymbol;

//状态栈
typedef struct
{
	int *state;
	int num; 
	int top;
}StackState,*pStackState;

void init_flag(int**flag, int n)
{
	*flag = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i < n; i++)
	{
		(*flag)[i] = FALSE;
	}
}
//输出非终结到空的所有结果
void output_null(int *p, int len)
{
	printf("输出判断空集的结果\n");
	for (int i = 0; i < len; i++)
	{
		printf("%d \n", p[i]);
	}
}
void output_fisrt(char **pFirst, char *pall)
{
	int len = strlen(pall);
	printf("\n************输出 first集合***************\n");
	for (int i = 0; i < len; i++)
	{
		if (pFirst[i])printf("\nFIRST[%c] = %s", pall[i], pFirst[i]);
	}

}
void output_follow(char **pFollow, char *non_terminal)
{
	int len = strlen(non_terminal);
	int i;
	printf("\n******************输出 follow 集*******************\n");
	for (i = 0; i < len; i++)
	{
		printf("\nFollow[%c] = %s", non_terminal[i], pFollow[i]);
	}
	printf("\n");
}

//判断一个string中是否包含某一个字符  如果包含返回true
int have(char *string, char symbol)
{
	char *p = string;
	while (*p != '\0')
	{
		if (*p == symbol)return true;
		p++;
	}
	return false;
}

// 从命令行读取语句  存储到文本中
// 只允许一条一条读入
void InputToFile(char *filepath)
{
	FILE *fp;
	char string[20];
	char tmp[20];
	int flag_num = 1;
	printf("请输入正规式   ~代表空  $代表开始符号\n");
	fgets(string, sizeof(string), stdin);
	while (strcmp(string, "") == 0)
		fgets(string, sizeof(string), stdin);

	fp = fopen(filepath, "w");
	if (fp == NULL)
	{
		printf("文件无法打开\n");
		exit(0);
	}

	while (string[0] >= 'A'&&string[0] <= 'Z'&&string[1] == '-'&&string[2] == '>'&&string[3] != '\0')
	{
		int i = 3;
		char *p = string + 3;
		char *q = string + 3;


		if (flag_num == 1) //如果是输入的第一个产生式
		{
			tmp[0] = '$'; tmp[1] = '-'; tmp[2] = '>'; tmp[3] = string[0]; tmp[4] = '\0';
			fprintf(fp, "%s\n", tmp);
		}
		flag_num++;
		fprintf(fp, "%s\n", string);   //将该行语句存储到文本中
		fgets(string, sizeof(string), stdin);  //读取下一行文本
	}
	fclose(fp);
}

// 输出读取到的正规式
void OutPutRegu(pRegu pregu)
{
	printf("输出正规式\n");
	for (int i = 0; i < pregu->num; i++)
	{
		printf("\n[%d] %s", i, pregu->pregu[i]);
	}
}
void GetFromFile(pRegu pregu, char *filepath)
{
	char tmp_regu[MAXCHAR];
	FILE *fp;
	fp = fopen(filepath, "r");
	if (!fp)
	{
		printf("文件打开失败\n");
		exit(1);
	}

	pregu->num = 0;
	pregu->pregu = NULL;
	while (!feof(fp))
	{
		fscanf(fp, "%s", tmp_regu);
		if (feof(fp))break;
		pregu->pregu = (char**)realloc(pregu->pregu, (pregu->num + 1) * sizeof(char*));
		pregu->pregu[pregu->num] = (char*)malloc(strlen(tmp_regu) + 1 * sizeof(char));
		strcpy(pregu->pregu[pregu->num], tmp_regu);
		pregu->num++;
	}
	fclose(fp);
}
//将终结符和非终结符分类
void GetSymbol(pRegu pregu, char**non_terminal, char**terminal)
{
	int index;
	char *p = NULL;
	if (!*non_terminal)
	{
		*non_terminal = (char*)malloc(sizeof(char));
		(*non_terminal)[0] = '\0';
	}
	if (!*terminal)
	{
		*terminal = (char*)malloc(sizeof(char));
		(*terminal)[0] = '\0';
	}

	for (int i = 0; i < pregu->num; i++)
	{
		p = pregu->pregu[i];
		while (*p != '\0')
		{
			//跳过 "->"
			if (*p == '-')
			{
				p += 2;
				continue;
			}
			//如果遇到非终结符
			if ('A' <= *p&&*p <= 'Z' || '$' == *p)
			{
				index = strlen(*non_terminal);
				*non_terminal = (char*)realloc(*non_terminal, sizeof(char)*(index + 2));
				(*non_terminal)[index] = '\0';

				//如果该非终结符已经加入过 跳过
				if (have(*non_terminal, *p)) p++;
				else
				{
					index = strlen(*non_terminal);
					(*non_terminal)[index] = *p;
					(*non_terminal)[++index] = '\0';
					p++;
				}
			}
			//如果遇到空 跳过
			else if (*p == '~')
			{
				p++;
				continue;
			}
			//如果遇到终结符
			else
			{
				index = strlen(*terminal);
				(*terminal) = (char*)realloc((*terminal), sizeof(char)*(index + 2));
				(*terminal)[index] = '\0';

				//如果该终结符已经加入过
				if (have(*terminal, *p))p++;
				else
				{
					index = strlen(*terminal);
					(*terminal)[index] = *p;
					(*terminal)[++index] = '\0';
					p++;
				}

			}


		}
	}
}

//用于判断某个产生式右部 是否含终结符  含有返回true
int haveTermainal(char *pregu, char *terminal)
{
	int i;
	char *p = pregu + 3;
	int index = strlen(terminal);
	if (*p == '~')return FALSE;
	while (*p != '\0')
	{
		for (i = 0; i < index; i++)
		{
			if (*p == terminal[i])return TRUE;
		}
		p++;
	}
	return FALSE;
}
//用于判断 某个非终结符是否已经全部删除 全部删除返回 true
int NotExitInRegu(char symbol, char**pregu, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!pregu[i])continue;
		if (symbol == pregu[i][0])return FALSE;
	}
	return TRUE;
}
//用于获取一个字符 在字符串中的位置
int getindex(char *pstr, char symbol)
{
	int index;
	int len = strlen(pstr);
	for (index = 0; index < len; index++)
	{
		if (pstr[index] == symbol)return index;
	}
	printf("找不到该字符1\n");
	return -1;
}
//用于判断是否所有非终结符都能推到空
int all_ars_null(char *pregu, int *pnull, char *non_terminal)
{
	char *p = pregu + 3;
	int index;
	while (*p != '\0')
	{
		index = getindex(non_terminal, *p);
		if (pnull[index] != YES)return FALSE;
		p++;
	}
	return TRUE;
}
//判断右部是否包含 确定不为空的非终结符
int have_no(char*pregu, int *pnull, char*non_treminal)
{
	char *p = pregu + 3;
	int index;
	while (*p != '\0')
	{
		index = getindex(non_treminal, *p);
		if (pnull[index] == NO)return TRUE;
		p++;
	}
	return FALSE;
}
int exit_notsure(int *pnull, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (pnull[i] == NOTSURE) return TRUE;
	}
	return FALSE;
}
void GetNull(int**pnull, pRegu regu, char* non_terminal, char* terminal, int *pleft_rec)
{
	char **pregu = NULL;
	char tmp;
	int j, index, len, k;
	*pnull = (int*)malloc(sizeof(int)*strlen(non_terminal));
	pregu = (char**)malloc(sizeof(char*)*regu->num);
	//首先进行正规式的复制
	for (int i = 0; i < regu->num; i++)
	{
		pregu[i] = (char*)malloc(sizeof(char)*(strlen(regu->pregu[i]) + 1));
		strcpy(pregu[i], regu->pregu[i]);
	}

	for (int i = 0; i < regu->num; i++) (*pnull)[i] = NOTSURE;  //初始化所有状态为不确定

	for (int i = 0; i < regu->num; i++)
	{
		if (!pregu[i])continue;
		//如果遇见直接左递归  直接删除
		if (pregu[i][0] == pregu[i][3])
		{
			tmp = pregu[i][0];
			free(pregu[i]);
			pregu[i] = NULL;

			//如果已经全部删除
			if (NotExitInRegu(tmp, pregu, regu->num))
			{
				index = getindex(non_terminal, tmp);
				(*pnull)[index] = NO;
			}
			continue;
		}
		//如果包含终结符
		if (haveTermainal(pregu[i], terminal))
		{
			tmp = pregu[i][0];
			free(pregu[i]);
			pregu[i] = NULL;
			if (NotExitInRegu(tmp, pregu, regu->num))
			{
				index = getindex(non_terminal, tmp);
				(*pnull)[index] = NO;
			}
		}
		//如果包含空
		else if (pregu[i][3] == '~')
		{
			index = getindex(non_terminal, pregu[i][0]);
			(*pnull)[index] = YES;
			tmp = pregu[i][0];

			for (j = 1; j < regu->num; j++)
			{
				if (!pregu[i])continue;
				if (tmp = pregu[j][0])
				{
					free(pregu[j]);
					pregu[j] = NULL;
				}
			}
		}

	}

	//书中所述第三步
	while (exit_notsure(*pnull, strlen(non_terminal)))
	{
		for (int i = 0; i < regu->num; i++)
		{
			if (!pregu[i])continue;
			pleft_rec[getindex(non_terminal, pregu[i][0])] = TRUE;

			//遍历到第一个不为空的非终结符
			len = strlen(pregu[i]);
			for (k = 3; k < len; k++)
			{
				if (YES == (*pnull)[getindex(non_terminal, pregu[i][k])]) continue;
				else break;
			}

			//如果这符号是非终结符  并且状态不确定
			if (have(non_terminal, pregu[i][k])
				&& (*pnull)[getindex(non_terminal, pregu[i][k])] == NOTSURE
				&&pleft_rec[getindex(non_terminal, pregu[i][k])])
			{
				tmp = pregu[i][0];
				if (pregu[i] != NULL)free(pregu[i]);
				pregu[i] = NULL;

				if (NotExitInRegu(tmp, pregu, regu->num))
				{
					index = getindex(non_terminal, tmp);
					(*pnull)[index] = NO;
				}

				pleft_rec[getindex(non_terminal, tmp)] = FALSE;
				continue;
			}


			//如果右侧都能推出空   标记为空 并删除所有符号
			if (all_ars_null(pregu[i], *pnull, non_terminal))
			{
				index = getindex(non_terminal, pregu[i][0]);
				(*pnull)[index] = YES;
				tmp = pregu[i][0];                                   //此处的删除是我加的

				for (j = 1; j < regu->num; j++)
				{
					if (!pregu[i])continue;
					if (tmp = pregu[j][0])
					{
						free(pregu[j]);
						pregu[j] = NULL;
					}
				}

			}
			//如果右侧包含具有不到空的非终结符
			else if (have_no(pregu[i], *pnull, non_terminal))
			{
				tmp = pregu[i][0];
				free(pregu[i]);
				pregu[i] = NULL;
				if (NotExitInRegu(tmp, pregu, regu->num))
				{
					index = getindex(non_terminal, tmp);
					(*pnull)[index] = NO;
				}
			}



		}


	}

}

void AddToFirst(char symbol, char newsym, char *non_terminal, char **pgather)
{
	int index;
	int len;
	index = getindex(non_terminal, symbol);
	//如果该符号的first集合还没有分配过 分配内存病存储
	if (!pgather[index])
	{
		pgather[index] = (char*)malloc(sizeof(char) * 2);
		pgather[index][0] = newsym;
		pgather[index][1] = '\0';
	}
	else
	{
		//如果该符号已经被加入过了 不用再加了
		if (have(pgather[index], newsym))
			return;
		len = strlen(pgather[index]);
		pgather[index] = (char*)realloc(pgather[index], sizeof(char)*(len + 1));
		pgather[index][len] = newsym;
		pgather[index][len + 1] = '\0';
	}
}

//返回该非终结符 是否能转移到空
int can_obtain_null(char *non_terminal, int *pnull, char symbol)
{
	int index;
	if (!have(non_terminal, symbol))return FALSE;
	index = getindex(non_terminal, symbol);
	if (pnull[index] == YES)
		return TRUE;
	else
		return FALSE;
}
//判断右部是不是全都是非终结符
int IsNonterminal(char *pregu, char *terminal)
{
	char *p = pregu;
	for (int i = 3; i < strlen(p); i++)
	{
		if (have(terminal, p[i]))return FALSE;
	}
	return TRUE;
}

// 用于有条件的合并first集合
void  FirstAdd(char symbol, char newsym, int id, char *pall, char **pFirst)
{
	//id 用于判断是否加入  空这个符号
	int indexn = getindex(pall, newsym);
	int j;
	int len;
	//如果待加入的符号需要返回
	if (!pFirst[indexn])return;
	len = strlen(pFirst[indexn]);
	for (int i = 0; i < len; i++)
	{
		if (id == TRUE)
		{
			if ('~' == pFirst[indexn][i])continue;
			AddToFirst(symbol, pFirst[indexn][i], pall, pFirst);
		}
		else
		{
			AddToFirst(symbol, pFirst[indexn][i], pall, pFirst);
		}
	}

}

//获取第一个不能推到空的下标
int getPos(char *pregu, char *non_terminal, int *pnull)
{
	char *p = pregu;
	int i;
	for (int i = 3; i < strlen(p); i++)
	{
		if (!can_obtain_null(non_terminal, pnull, p[i]))
			return i;
	}
	return FALSE;
}
char GetFirst(char symbol, char **pFirst, pRegu pregu, char *pall, char *terminal, char *non_terminal, int*pnull, int *flag, int *pleft_rec)
{
	//flag 用于判断该字符的 first集合是否已经求出过
	//输入符号计算该符号的first集合
	char **p = pregu->pregu;
	char *ptmp = *p;
	int j = 0;

	//如果当前符号的  first集合已经求出  直接返回
	if (flag[getindex(pall, symbol)])return symbol;
	//如果该符号是终结符
	if (have(terminal, symbol))
	{
		AddToFirst(symbol, symbol, pall, pFirst);
		return symbol;
	}

	//如果这个符号是非终结符
	else if (have(non_terminal, symbol))
	{
		//将左递归置为 true 用于标记
		if (!pleft_rec[getindex(non_terminal, symbol)])pleft_rec[getindex(non_terminal, symbol)] = TRUE;

		for (j = 1; j < pregu->num; j++)
		{
			if (p[j])
			{
				//找到一个以当前symbol开头的产生式
				ptmp = p[j];
				if (ptmp[0] != symbol)continue;
				//如果该非终结符就可以转移到空   加入kong
				if (can_obtain_null(non_terminal, pnull, symbol))
				{
					AddToFirst(symbol, '~', pall, pFirst);
				}
				//如果右部以非终结符开始
				if (have(terminal, ptmp[3]))
				{
					AddToFirst(symbol, ptmp[3], pall, pFirst);
				}
				//如果是空
				else if ('~' == ptmp[3])continue;
				//如果是非终结符
				else
				{
					//如果右部产生式不含有终结符 和直接左递归
					if (IsNonterminal(p[j], terminal) && p[j][0] != p[j][3])
					{
						//获取第一个不能推出空的非终结符
						int length = getPos(p[j], non_terminal, pnull);

						//如果不是全都能推到空
						if (length)
						{
							int i;
							for (i = 3; i < length; i++)
							{
								//如果进入了递归死循环  break；
								if (pleft_rec[getindex(non_terminal, ptmp[i])])break;
								FirstAdd(symbol,
									GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
									TRUE, pall, pFirst
								);
							}

							//如果这个符号已经求过first集合，直接加入  否则求解后再加入
							if (pleft_rec[getindex(non_terminal, ptmp[i])])
								FirstAdd(symbol, ptmp[i], FALSE, pall, pFirst);
							else
								FirstAdd(symbol, GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
									FALSE, pall, pFirst);


						}

						//如果右部可以推到全空
						else
						{
							int length = strlen(p[j]);
							int i;
							for (i = 3; i < length; i++)
							{
								//如果进入了递归死循环  break；
								if (pleft_rec[getindex(non_terminal, ptmp[i])])break;
								FirstAdd(symbol,
									GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
									FALSE, pall, pFirst
								);

							}

							AddToFirst(symbol, '~', pall, pFirst);
						}

					}

					//如果含有终结符  或者右递归
					else
					{
						int i = 3;
						ptmp = p[j];
						//如果前一个是空就继续执行
						do
						{
							//防止递归死循环
							if (have(non_terminal, ptmp[i]) && pleft_rec[getindex(non_terminal, ptmp[i])])break;
							FirstAdd(symbol,
								GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
								FALSE, pall, pFirst);
							i++;
						} while (can_obtain_null(non_terminal, pnull, ptmp[i - 1]));

					}

				}

			}
		}
	}
	if (j == pregu->num)flag[getindex(pall, symbol)] = TRUE;

	//回溯
	if (have(non_terminal, symbol))
	{
		pleft_rec[getindex(non_terminal, symbol)] = FALSE;
	}
	return symbol;

}
//返回字符下标
int contain_symbol(char *p, char symbol)
{
	char *ptmp = p;
	int i;
	int length = strlen(ptmp);

	for (i = 3; i < length; i++)
	{
		if (symbol == ptmp[i])return i;
	}
	return FALSE;
}
//合并 follow集
void follow_add(char symbol, char newsym, char *non_terminal, char **pFollow)
{
	int indexf;
	int len;
	if ('~' == newsym)return;
	indexf = getindex(non_terminal, newsym);
	if (!pFollow[indexf])return;
	for (int i = 0; i < strlen(pFollow[indexf]); i++)
	{
		AddToFirst(symbol, pFollow[indexf][i], non_terminal, pFollow);
	}
}
void follow_add_from_first(char symbol, char FirstEle, char *non_terminal, char *pall, char **pFirst, char **pFollow)
{
	int indexf;
	int len;
	indexf = getindex(pall, FirstEle);
	len = strlen(pFirst[indexf]);
	for (int i = 0; i < len; i++)
	{
		if ('~' == pFirst[indexf][i])continue;
		AddToFirst(symbol, pFirst[indexf][i], non_terminal, pFollow);
	}
}
char GetFollow(char symbol, char **pFirst, char **pFollow, pRegu pregu, char *pall, char *non_terminal, int *pnull, int *flag, int *pleft_rec)
{
	char **p = pregu->pregu;
	int j;

	//如果已经求出来过  就直接返回
	if (flag[getindex(non_terminal, symbol)]) return symbol;

	pleft_rec[getindex(non_terminal, symbol)] = TRUE;

	for (j = 0; j < pregu->num; j++)
	{
		if (p[j])
		{
			//找到当前符号在右部产生式的位置
			int pos = contain_symbol(p[j], symbol);
			int length = strlen(p[j]);

			int i;
			char *ptmp = p[j];

			if ('$' == symbol)AddToFirst(symbol, '#', non_terminal, pFollow);

			if (pos == FALSE)continue;

			//如果在最右端
			if (pos == length - 1)
			{
				if (ptmp[0] == ptmp[pos])continue;
				//将左部加入follow集
				else if (pleft_rec[getindex(non_terminal, ptmp[0])])
					follow_add(symbol, ptmp[0], non_terminal, pFollow);
				else
					follow_add(symbol,
						GetFollow(ptmp[0], pFirst, pFollow, pregu, pall, non_terminal, pnull, flag, pleft_rec),
						non_terminal, pFollow);
			}

			//否则
			else
			{
				for (i = pos + 1; i < length; i++)
				{
					//如果该符号不能推到空  加入完就可退出
					if (!can_obtain_null(non_terminal, pnull, ptmp[i]))
					{
						follow_add_from_first(symbol, ptmp[i], non_terminal, pall, pFirst, pFollow);
						break;
					}
					else
					{
						follow_add_from_first(symbol, ptmp[i], non_terminal, pall, pFirst, pFollow);
					}
				}
				//如果所有的符号都被加入
				if (i == length)
				{
					if (pleft_rec[getindex(non_terminal, ptmp[0])])
						follow_add(symbol, ptmp[0], non_terminal, pFollow);
					else
					{
						follow_add(symbol,
							GetFollow(ptmp[0], pFirst, pFollow, pregu, pall, non_terminal, pnull, flag, pleft_rec),
							non_terminal, pFollow);
					}
				}
			}

		}
	}

	pleft_rec[getindex(non_terminal, symbol)] = FALSE;
	flag[getindex(non_terminal, symbol)] = TRUE;
	return symbol;
}
//获取分析表的横坐标
void GetAllSymbol(char **pall, char *non_terminal, char *terminal)
{
	int len = strlen(non_terminal) + strlen(terminal) + 2;
	*pall = (char*)malloc(sizeof(char)*len);
	strcpy(*pall, terminal);
	strcat(*pall, "#");
	strcat(*pall, non_terminal);
}

//加 .
void InsertDot(char **pregu, char *pregulate)
{

	int len = strlen(pregulate);
	(*pregu) = (char*)malloc(sizeof(char)*(len + 2));
	for (int i = 0; i <= len; i++)
	{
		if (i < 3)(*pregu)[i] = pregulate[i];
		else if (i == 3)
		{
			if ('~' == pregulate[i])
			{
				(*pregu)[i] = '.';
				(*pregu)[i + 1] = '\0';
				return;
			}
			else
			{
				(*pregu)[i] = '.';
				(*pregu)[i + 1] = pregulate[i];
			}
		}
		else
			(*pregu)[i + 1] = pregulate[i];
	}
}
//初始化
void InitDFA(pProject * ppro)
{
	(*ppro) = (pProject)malloc(sizeof(Project));
	(*ppro)->pclosure = (pClosure)malloc(sizeof(Closure));
	(*ppro)->num = 0;
	(*ppro)->pclosure->num = 0;
	(*ppro)->pclosure->preguc = NULL;
}

//获取 . 后面的符号
char get_symbol_after_dot(char *str)
{
	char *p = str;
	while (*p)
	{
		if ('.' == *p)
			return *(p + 1);
		p++;
	}
	return *p;
}

int Found(char *pregu, pClosure pclosure)
{
	for (int i = 0; i < pclosure->num; i++)
	{
		if (0 == strcmp(pregu, pclosure->preguc[i])) return TRUE;
	}
	return FALSE;
}

//给定初态 计算闭包
void calc_closure(pProject *ppro, int index, pRegu pregu, char *pregulate, char *non_terminal)
{
	//初态直接加入对应的闭包
	int num;
	int len = strlen(pregulate);
	char symbol;
	int i, j;
	char *ptmp_regu = NULL;
	num = (*ppro)[index].pclosure->num;

	//初始化
	(*ppro)[index].pclosure->preguc = (char**)realloc((*ppro)[index].pclosure->preguc, (num + 1) * sizeof(char*));
	(*ppro)[index].pclosure->preguc[num] = (char*)malloc(sizeof(char)*(len + 1));
	strcpy((*ppro)[index].pclosure->preguc[num], pregulate);
	(*ppro)[index].pclosure->num++;
	num = (*ppro)[index].pclosure->num;

	for (i = 0; i < (*ppro)[index].pclosure->num; i++)
	{
		//获取 . 后面跟着的字符
		symbol = get_symbol_after_dot((*ppro)[index].pclosure->preguc[i]);

		//如果后面跟的非终结符  进行拓展
		if (have(non_terminal, symbol))
		{
			for (j = 0; j < pregu->num; j++)
			{
				//如果符合条件  加入
				if (symbol == pregu->pregu[j][0])
				{
					num = (*ppro)[index].pclosure->num;
					InsertDot(&ptmp_regu, pregu->pregu[j]); // jia .

															//如果这个式子已经加入了 跳过
					if (Found(ptmp_regu, (*ppro)[index].pclosure))continue;
					(*ppro)[index].pclosure->preguc = (char**)realloc((*ppro)[index].pclosure->preguc, (num + 1) * sizeof(char*));
					(*ppro)[index].pclosure->preguc[num] = (char*)malloc(sizeof(char)*(len + 10));
					strcpy((*ppro)[index].pclosure->preguc[num], ptmp_regu);
					(*ppro)[index].pclosure->num++;

				}
			}
		}
	}

}

int Str2Int(char *pint)
{
	int  num = 0;
	char *p = pint;
	while (*p != '\0')
	{
		num = num * 10 + (*p - '0');
		p++;
	}
	return num;
}

//可能会出现几个 产生式处于同一个项目集
int ExistDFA(char *pregu, Analyze pana, char *pall, int *pindex)
{
	int index;
	char symbol;
	char *p;
	symbol = get_symbol_after_dot(pregu);
	index = getindex(pall, symbol);
	//如果还空的  返回错误
	if (pana.pparse->pproceed[index] == NULL) return FALSE;
	else
	{
		p = pana.pparse->pproceed[index];

		//移入动作
		if ('S' == p[0])
		{
			p++;
			*pindex = Str2Int(p); //返回需要填写的项目集编号
			return TRUE;
		}
		//goto表
		else if ('0' <= p[0] && p[0] <= '9')
		{
			*pindex = Str2Int(p);
			return TRUE;
		}
		//如果是归约  产生冲突  
		else if ('R' == p[0])
		{
			printf("\nthe productions cannot be SLR(1) analyzed\n");
		}
		return -1;
	}
}
void dot_move_back(char *pregu)
{
	char *p = pregu;
	char tmp;
	while (*p)
	{
		if ('.' == *p)
		{
			tmp = *(p + 1);
			*(p + 1) = '.';
			*p = tmp;
			return;
		}
		else p++;
	}
}

void dot_move_forward(char *pregulate)
{
	char *p = pregulate;
	char tmp;
	while (*p)
	{
		if ('.' == *p)
		{
			tmp = *p;
			*p = *(p - 1);
			*(p - 1) = tmp;
			return;
		}
		p++;
	}
}

void Int2Str(char **pitostr, int inum)
{
	int i;
	int mod;
	int len;

	char temp[MAXCHAR];
	for (i = 0; i<MAXCHAR; i++)
		temp[i] = '\0';

	mod = inum % 10;

	if (0 == inum)
	{
		(*pitostr) = (char *)malloc(sizeof(char) * 2);
		(*pitostr)[0] = '0';
		(*pitostr)[1] = '\0';
		return;
	}

	i = 0;
	while (inum>10)
	{
		temp[i] = mod + '0';
		i++;

		inum = (inum - mod) / 10;

		mod = inum % 10;

	}

	if (mod == 0)
	{

		temp[i] = mod + '0';
		mod = inum / 10;
		i++;
		temp[i] = mod + '0';

		temp[i + 1] = '\0';
	}
	else
	{
		temp[i] = mod + '0';
		temp[i + 1] = '\0';
	}
	len = strlen(temp);
	(*pitostr) = (char *)malloc(sizeof(char)*(len + 1));

	for (i = len; i>0; i--)
		(*pitostr)[len - i] = temp[i - 1];
	(*pitostr)[len] = '\0';

}

int is_newDFA(pProject *ppro, int num, char *pregulate)
{
	int i;
	int j;
	char *pregu = NULL;
	char *ptemp_regulate = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
	strcpy(ptemp_regulate, pregulate);
	dot_move_back(ptemp_regulate);

	//判断在原项目集中是否能找到 该项目集  
	for (i = 0; i <= (*ppro)->num; i++)
	{
		if (Found(ptemp_regulate, (*ppro)[i].pclosure))
		{
		   //如果在该项目集中 找到了 该语句  就遍历该项目集

			for (j = 0; j<(*ppro)[i].pclosure->num; j++)
			{
				/* if it's '.', it's not core */
				if ('.' == (*ppro)[i].pclosure->preguc[j][3])
				{
					continue;
				}

				pregu = (char*)malloc(sizeof(char)*(strlen((*ppro)[i].pclosure->preguc[j]) + 1));
				strcpy(pregu, (*ppro)[i].pclosure->preguc[j]);

				dot_move_forward(pregu);

				if (!Found(pregu, (*ppro)[num].pclosure))
				{


					free(pregu);
					pregu = NULL;
					break;
				}
				/*      else
				printf("\n production %s is in project %s!!",(*ppro)[i].pclosure->pregulate[j],num); */


			}
			if (j == (*ppro)[i].pclosure->num)
			{

				return i;
			}

		}

	}
	if (i>(*ppro)->num)
	{

		return  NO;
	}
	else
		return i;

}

//判断是否存在相同的转移路径
int IsnewDFA(pProject *ppro, int num, char *pregulate)
{
	int i, j;
	char *pregu = NULL;
	char *ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
	strcpy(ptmp_regu, pregulate);
	dot_move_back(ptmp_regu);

	//判断在原项目集中是否能找到该项目集     
	for (i = 0; i <= (*ppro)->num; i++)
	{
		//如果在某个状态集中找到了要添加的 量
		if (Found(ptmp_regu, (*ppro)[i].pclosure))
		{

			for (j = 0; j < (*ppro)[i].pclosure->num; j++)
			{
				if ('.' == (*ppro)[i].pclosure->preguc[j][3])continue;
				pregu = (char*)malloc(sizeof(char)*(strlen((*ppro)[i].pclosure->preguc[j]) + 1));
				strcpy(pregu, (*ppro)->pclosure->preguc[j]);

				dot_move_forward(pregu);
				if (!Found(pregu, (*ppro)[num].pclosure))
				{
					free(pregu);
					pregu = NULL;
					break;
				}
			}
			if (j == (*ppro)[i].pclosure->num)
			{
				return i;
			}
		}
	}
	if (i > (*ppro)->num)return NO;
	else return i;
}


int GetAction(char symbol, char *terminal)
{
	if ('\0' == symbol)return REDUCE;
	else if (have(terminal, symbol))return SHIFT;
	return GOTO;
}

void delete_dot(char *p)
{
	int len;
	int index;
	index = getindex(p, '.');
	len = strlen(p);
	for (int i = index; i < len; i++)
	{
		p[i] = p[i + 1];
	}
}
//构造分析表
void calc_analyze_table(char *pregulate, char **pFollow, pAnalyze *pana, int status, int to_status, pRegu pregu, char *non_terminal, char *terminal, char *pall)
{
	char symbol;
	int index;
	char *ptmp_regu = NULL;
	char *itostr = NULL;
	int len;
	int length;
	int flag;
	int i;
	int j;

	ptmp_regu = pregulate;
	symbol = get_symbol_after_dot(ptmp_regu);
	flag = GetAction(symbol, terminal);

	switch (flag)
	{
	case GOTO:
		index = getindex(pall, symbol);
		Int2Str(&itostr, to_status);

		len = strlen(itostr);
		if ((*pana)[status].pparse->pproceed[index])
		{
			printf("不是SLR（1）文法");
			exit(1);
		}
		(*pana)[status].pparse->pproceed[index] = (char*)malloc(sizeof(char)*(len + 1));
		strcpy((*pana)[status].pparse->pproceed[index], itostr);

		break;
	case REDUCE:
		//如果 . 后面什么也不跟
		ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
		strcpy(ptmp_regu, pregulate);
		delete_dot(ptmp_regu);
		if ('\0' == ptmp_regu) ptmp_regu[3] = '~';

		for (i = 0; i < pregu->num; i++)
		{
			if (0 == strcmp(ptmp_regu, pregu->pregu[i]))break;
		}
		Int2Str(&itostr, i);
		len = strlen(itostr);

		length = strlen(terminal);
		index = getindex(non_terminal, ptmp_regu[0]);

		//扫描所有非终结符  判断是否存在非终结符
		for (j = 0; j < length; j++)
		{
			//判断该终结符是否在follow 集中
			if (!have(pFollow[index], terminal[j]))continue;
			if ((*pana)[status].pparse->pproceed[j])
			{
				printf("不是SLR1");
				exit(1);
			}
			(*pana)[status].pparse->pproceed[j] = (char*)malloc(sizeof(char)*(len + 2));
			//如果左部是起点  接受
			if ('$' == ptmp_regu[0])
			{
				strcpy((*pana)[status].pparse->pproceed[j], "ACC");
			}
			else
			{
				strcpy((*pana)[status].pparse->pproceed[j], "R");
				strcat((*pana)[status].pparse->pproceed[j], itostr);
			}
		}
		break;
	case SHIFT:
		index = getindex(pall, symbol);
		Int2Str(&itostr, to_status);

		len = strlen(itostr);
		if ((*pana)[status].pparse->pproceed[index])
		{
			printf("不是SLR1");
			exit(1);
		}
		(*pana)[status].pparse->pproceed[index] = (char*)malloc(sizeof(char)*(len + 2));

		strcpy((*pana)[status].pparse->pproceed[index], "S");
		strcat((*pana)[status].pparse->pproceed[index], itostr);

		break;
	default:
		printf("不是SLR1");
		exit(1);
	}

}

void GetDFA(pProject *ppro, int index, char **pFollow, pRegu pregu, pAnalyze *pana, char *pall, char *non_terminal, char *terminal)
{
	int num;
	char *pregulate = NULL;
	char tmpchar;
	char *ptmp_regu = NULL;
	int toindex = -1;
	num = (*ppro)[index].pclosure->num;

	//遍历该项目中每一条语句   构造其对应的状态转移表  和  新的后继状态集
	for (int i = 0; i < num; i++)
	{
		pregulate = (*ppro)[index].pclosure->preguc[i];
		tmpchar = get_symbol_after_dot(pregulate);


		if (tmpchar != '\0')
		{
			//如果已经存在相应的DFA  但是不完整  比如 同一个初始状态集 中的几个语句转移到同一个新的后继状态  如 I0 的 第一条和第二条
			if (ExistDFA(pregulate, (*pana)[index], pall, &toindex))
			{
				//如果已经存在转移
				if (toindex > index) //我觉得这个状态一定是存在的  因为从当前状态  转移到后继状态啊
				{
					ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
					strcpy(ptmp_regu, pregulate);
					dot_move_back(ptmp_regu);
					calc_closure(ppro, toindex, pregu, ptmp_regu, non_terminal);  //扩充该项目
				}
			}

			else
			{
				//如果该项目已经完全生成   不用重新生成该产生式转移出去的闭包   如 I4 E->.T  经过T又回到状态2   向前转移
				//此时不需要再重复计算该 产生式的 dfa 只需要计算自身的 分析表
				if (toindex = is_newDFA(ppro, index, pregulate) != NO) //如果能回到自身？
				{
					calc_analyze_table(pregulate, pFollow, pana, index, toindex, pregu, non_terminal, terminal, pall);
				}

				//
				else
				{
					*ppro = (pProject)realloc(*ppro, sizeof(Project)*((*ppro)->num + 2));
					if (!*ppro)
					{
						printf("内存分配错误");
						exit(1);
					}

					//初始化一个新表
					(*ppro)->num++;
					(*ppro)[(*ppro)->num].pclosure = (pClosure)malloc(sizeof(Closure));
					(*ppro)[(*ppro)->num].pclosure->preguc = NULL;
					(*ppro)[(*ppro)->num].pclosure->num = 0;

					ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
					strcpy(ptmp_regu, pregulate);
					dot_move_back(ptmp_regu);

					//传入初态获取终态
					calc_closure(ppro, (*ppro)->num, pregu, ptmp_regu, non_terminal);

					//构造分析表
					calc_analyze_table(pregulate, pFollow, pana, index, (*ppro)->num, pregu, non_terminal, terminal, pall);


				}
			}



		}
		else
		{
			//如果该产生式后面没有东西了   就只需要构造规约的分许表  而不用构造dfa   因为没有转移的后继状态
			calc_analyze_table(pregulate, pFollow, pana, index, NOTSURE, pregu, non_terminal, terminal, pall);
		}

	}

}
void output_closure(pClosure pclosure)
{
	int i;
	for (int i = 0; i < pclosure->num; i++)
	{
		printf("\n %s   ", pclosure->preguc[i]);
	}
}

void output_project(pProject  ppro, int index)
{
	printf("\n****************输出项目************************\n");
	for (int i = 0; i <= index; i++)
	{
		printf("\n-----------------------");
		printf("\nproject[%d] :", i);
		output_closure(ppro[i].pclosure);

	}

}

void output_DFA(pAnalyze pana, char *pall_symbol)
{
	int len;
	int i;
	int j;
	int inum;
	len = strlen(pall_symbol);
	printf("\nDFA:");
	printf("\n-------------------------------------------------------------------\n");
	for (i = 0; i<len; i++)
	{
		if ('$' == pall_symbol[i] || '#' == pall_symbol[i])
			continue;
		printf("\t%c", pall_symbol[i]);
	}
	for (i = 0; i <= pana->num; i++)
	{
		printf("\nP[%d]", i);

		for (j = 0; j<len; j++)
		{
			if (j == getindex(pall_symbol, '$') || j == getindex(pall_symbol, '#'))
				continue;
			if (!pana[i].pparse->pproceed[j]
				|| 'R' == pana[i].pparse->pproceed[j][0]
				|| 'A' == pana[i].pparse->pproceed[j][0])
			{
				printf("\t  ");
			}
			else if ('S' == pana[i].pparse->pproceed[j][0])
			{
				inum = Str2Int(pana[i].pparse->pproceed[j] + 1);
				printf("\tP[%d]", inum);
			}
			else
			{
				inum = Str2Int(pana[i].pparse->pproceed[j]);
				printf("\tP[%d]", inum);
			}
		}
	}

}

void output_parse(pParse pparse, char *pall_symbol)
{
	int i;
	for (i = 0; i<pparse->num; i++)
	{
		if ('$' == pall_symbol[i])
			continue;
		if (!pparse->pproceed[i])
			printf("\t  ");
		else
			printf("\t%s ", pparse->pproceed[i]);

	}
}
void output_analyze_table(pAnalyze pana, char *pall)
{
	int i;
	int len = strlen(pall);
	printf("\n********************分析表********************\n");
	for (i = 0; i < len; i++)
	{
		if ('$' == pall[i])continue;
		printf("\t%c", pall[i]);
	}

	for (i = 0; i <= pana->num; i++)
	{
		printf("\n[%d] ", i);
		output_parse(pana[i].pparse, pall);
	}
}

//获取用户输入
void GetInput(char **pstr, char *terminal)
{
	char t;
	int i = 0;
	printf("请输入\n");
	scanf("%c", &t);
	*pstr = (char*)malloc(sizeof(char) * 2);
	while ('#' != t)
	{
		*pstr = (char*)realloc(*pstr, sizeof(char)*(i + 3));
		(*pstr)[i] = t;
		scanf("%c", &t);
		i++;
	}
	(*pstr)[i] = '#';
	(*pstr)[i + 1] = '\0';
}


//void parsecontent(char *pcontent, pStackState pstate, pStackSymbol psymbol, pRegu pregu, pAnalyze pana, char * pall)
//{
//	char *pre;
//	char *ptmp;
//	int top_state;
//	chat tmp;
//}

#define MAXNUM 100
void stack_symbol_init(pStackSymbol p_stack)
{

	p_stack->symbol = (char*)malloc(sizeof(char)*MAXNUM);
	p_stack->num = MAXNUM;
	p_stack->top = 0;
}


void stack_state_init(pStackState p_stack)
{
	p_stack->state = (int*)malloc(sizeof(int)*MAXNUM);
	p_stack->num = MAXNUM;
	p_stack->top = 0;
}

char get_stack_symbol_top(pStackSymbol p_stack)
{
	return p_stack->symbol[p_stack->top];
}


int get_stack_state_top(pStackState p_stack)
{
	return p_stack->state[p_stack->top];
}
int pop_state(pStackState p_stack)
{

	if (p_stack->top >= 0)
		return p_stack->state[p_stack->top--];
	else
	{
		printf("\n error:at the bottom of the db_stack!exit...\n");
		exit(1);
		return p_stack->state[p_stack->top];
	}
}
char pop_symbol(pStackSymbol p_stack)
{
	if (p_stack->top)
		return p_stack->symbol[p_stack->top--];
	else
	{
		printf("\n error:at the bottom of the db_stack!exit...\n");
		exit(1);
		return p_stack->symbol[p_stack->top];
	}
}
void push_symbol(pStackSymbol p_stack, char t)
{
	if (p_stack->top<p_stack->num)
		p_stack->symbol[++(p_stack->top)] = t;
	else
	{
		p_stack->symbol = (char*)realloc(p_stack->symbol, sizeof(char)*(p_stack->num + MAXNUM));
		p_stack->symbol[++(p_stack->top)] = t;
	}

}
void push_state(pStackState p_stack, int t)
{
	if (p_stack->top<p_stack->num)
		p_stack->state[++(p_stack->top)] = t;
	else
	{
		p_stack->state = (int*)realloc(p_stack->state, sizeof(int)*(p_stack->num + MAXNUM));
		p_stack->state[++(p_stack->top)] = t;
	}


}

void print_symbol_stack(pStackSymbol p_stack)
{
	int i = 1;
	printf("\t");
	while (i <= p_stack->top)
	{
		printf("%c", p_stack->symbol[i]);
		i++;
	}
}

void print_state_stack(pStackState p_stack)
{
	int i = 1;
	printf("\t");
	while (i <= p_stack->top)
	{
		printf("%d", p_stack->state[i]);
		i++;
	}
}

void parsecontent(char *pcontent, pStackState p_state, pStackSymbol p_symbol, pRegu pregu, pAnalyze pana, char *pall_symbol)
{
	char *pre;
	char *ptemp;
	int top_state;
	char temp;
	int convert2num;
	int num;
	int result;
	int i;
	int step = 0;
	char *pregulate;


	stack_state_init(p_state);
	stack_symbol_init(p_symbol);
	push_state(p_state, 0);
	push_symbol(p_symbol, '#');


	printf("\nstep\tstate\t\tsymbol\t\tstring\t\taction\n");
	while ('\0' != *pcontent)
	{

		step++;
		printf("%d", step);
		print_state_stack(p_state);
		printf("\t");
		print_symbol_stack(p_symbol);
		printf("\t\t%s\t", pcontent);
		top_state = get_stack_state_top(p_state);


		/* get the action from ACTION table */
		pre = pana[top_state].pparse->pproceed[getindex(pall_symbol, *pcontent)];

		if (!pre)
		{
			printf("\terror\n");
			exit(1);
		}

		temp = pre[0]; /* get the first symbol from ACTION table */
					   /* printf("%c",temp); */

		switch (temp)
		{
		case 'S': /* SHIFT */
			convert2num = 0;
			printf("\tshift\n");
			convert2num = Str2Int(pre + 1);

			push_state(p_state, convert2num);
			push_symbol(p_symbol, *pcontent);
			pcontent++;

			break;

		case 'R': /* REDUCE */
			convert2num = 0;
			printf("\treduce\n");
			convert2num = Str2Int(pre + 1);

			ptemp = pregu->pregu[convert2num];

			if ('~' != ptemp[3])
			{
				num = strlen(ptemp) - 3;
				for (i = 0; i<num; i++)
				{

					pop_state(p_state);
					pop_symbol(p_symbol);
				}
			}

			/* get top stack state */
			top_state = get_stack_state_top(p_state);

			/* get the left symbol of current production */
			pregulate = pana[top_state].pparse->pproceed[getindex(pall_symbol, ptemp[0])];
			if (!pregulate)
			{
				printf("\nerror:goto table error!");
				exit(1);
			}

			result = Str2Int(pregulate);

			/* push the symbol and state */
			push_state(p_state, result);
			push_symbol(p_symbol, ptemp[0]);


			break;

		case 'A':
			printf("\taccept\n");
			exit(0);
			break;
		default:
			printf("\terror:!");
			exit(1);
		}
	}
}


int main()
{
	Regu regu;
	pProject pPro = NULL;    //用于存储构造的项目集
	pAnalyze pAna = NULL;    //用于存储构造的分许表
	StackState sstate;
	StackSymbol ssymbol;
							 //	StackSymbol sSymbol;

	char *ptmp_reg = NULL;    //用于暂存产生式
	char filepath[] = "productions.txt";  //用于存储产生式的路径


	char *pAllSymbol = NULL;    //用于存储所有符号
	char *non_terminal = NULL;  //用于存储非终结符
	char *terminal = NULL;      //用于存储终结符
	char **pFirst = NULL;     //用于存储first集合
	char **pFollow = NULL;    //用于存储follow集合
	int *pnull = NULL;        //用于存储该非终结符对应的符号是否能到达 空
	int *pleft_rec = NULL;     //用于判断是否是直接左递归
	char *ptmp = NULL;
	int *flag = NULL;
	char *pcontent = NULL; //用于存储用户输入

	int i;
	int k;
	int lenOfAllSymbol;
	int len;

	printf("****************SLR(1)******************\n");

	getchar();

	InputToFile(filepath); // 从命令行读取语句  存储到文本中
	GetFromFile(&regu, filepath); // 从文本中读取正规式
	OutPutRegu(&regu);

	if (0 == regu.num)
	{
		printf("未读取到正规式 结束\n");
		exit(0);
	}
	GetSymbol(&regu, &non_terminal, &terminal);  //将终结符和非终结符分类

	len = strlen(non_terminal) + 1;              //初始化左递归函数
	pleft_rec = (int*)malloc(sizeof(int)*len);
	for (i = 0; i < len; i++)pleft_rec[i] = false;

	//标记所有能转移到 空 的非终结符
	GetNull(&pnull, &regu, non_terminal, terminal, pleft_rec);
	//output_null(pnull, strlen(non_terminal));   用于调试  输出非终结符对空的判断

	printf("输出空结束\n");

	GetAllSymbol(&pAllSymbol, non_terminal, terminal);

	lenOfAllSymbol = strlen(pAllSymbol);
	len = strlen(non_terminal) + 1;

	pFirst = (char**)malloc(sizeof(char*)*lenOfAllSymbol);
	pFollow = (char**)malloc(sizeof(char*)*len);

	//进行初始化
	for (i = 0; i < lenOfAllSymbol; i++)pFirst[i] = NULL;
	for (i = 0; i < len; i++)
	{
		pleft_rec[i] = FALSE;
		pFollow[i] = NULL;
	}


	init_flag(&flag, lenOfAllSymbol);      //初始化 flag 为false  然后进行求解
	for (i = 0; i < lenOfAllSymbol; i++)
	{
		GetFirst(pAllSymbol[i], pFirst, &regu, pAllSymbol, terminal, non_terminal, pnull, flag, pleft_rec);
	}
	output_fisrt(pFirst, pAllSymbol);

	//重新初始化flag
	for (i = 0; i < lenOfAllSymbol; i++)flag[i] = FALSE;

	for (i = 0; i < len - 1; i++)
	{
		GetFollow(non_terminal[i], pFirst, pFollow, &regu, pAllSymbol, non_terminal, pnull, flag, pleft_rec);
	}
	output_follow(pFollow, non_terminal);





	int terminal_num = strlen(terminal);
	ptmp = (char*)malloc(sizeof(char)*(terminal_num + 2));
	if (!ptmp)
	{
		printf("分配内存失败");
	}

	strcpy(ptmp, terminal);
	strcat(ptmp, "#");

	terminal = ptmp;

	InsertDot(&ptmp_reg, regu.pregu[0]);//构造核心
	InitDFA(&pPro);                    //初始化DFA 
	calc_closure(&pPro, 0, &regu, ptmp_reg, non_terminal); //构造初集的闭包

	lenOfAllSymbol = strlen(pAllSymbol);

	//根据初态集构造DFA 和数据状态集

	for (int i = 0; i <= pPro->num; i++)
	{
		//初始化分析表  对应的某行
		pAna = (pAnalyze)realloc(pAna, sizeof(Analyze)*(i + 1));
		pAna->num = i;

		pAna[i].pparse = (pParse)malloc(sizeof(Parse));
		pAna[i].pparse->num = lenOfAllSymbol;
		pAna[i].pparse->pproceed = (char**)malloc(sizeof(char*)*lenOfAllSymbol);

		for (k = 0; k < lenOfAllSymbol; k++)pAna[i].pparse->pproceed[k] = NULL;

		//进行后继状态的遍历  看是否能延伸到新的 状态
		GetDFA(&pPro, i, pFollow, &regu, &pAna, pAllSymbol, non_terminal, terminal);

	}

	output_project(pPro, pPro->num);
	output_DFA(pAna, pAllSymbol);
	output_analyze_table(pAna, pAllSymbol);

	printf("\n此程序可进行 SLR(1) 分析");
	GetInput(&pcontent, terminal);
	parsecontent(pcontent, &sstate, &ssymbol, &regu, pAna, pAllSymbol);

	printf("\n结束\n");
	return 0;

}




//只有 isnewdfa函数是有问题

//需要改动的