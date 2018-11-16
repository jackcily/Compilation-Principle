#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define  MAXCHAR 50  //ÿ���ַ������ֵ
#define  MAXNUM  100 //ջ�����ĸ߶�
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

//���ڴ洢����ʽ
typedef struct Regu
{
	char**pregu;
	int num;
}Regu, *pRegu;


//���ڴ洢ÿһ���հ�   һ����Ŀ״̬
typedef struct Closure
{
	int num;
	char **preguc;
}Closure, *pClosure;
//���ڴ洢��Ŀ��
typedef struct Project
{
	int num;
	pClosure pclosure;
}Project, *pProject;


//���ڴ洢�������ÿһ��
typedef struct Parse
{
	int num;
	char**pproceed;
}Parse, *pParse;
//���ڴ洢������
typedef struct Analyze
{
	int num;
	pParse pparse;
}Analyze, *pAnalyze;

//����ջ
typedef struct
{
	char *symbol;
	int num;
	int top;
}StackSymbol,*pStackSymbol;

//״̬ջ
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
//������սᵽ�յ����н��
void output_null(int *p, int len)
{
	printf("����жϿռ��Ľ��\n");
	for (int i = 0; i < len; i++)
	{
		printf("%d \n", p[i]);
	}
}
void output_fisrt(char **pFirst, char *pall)
{
	int len = strlen(pall);
	printf("\n************��� first����***************\n");
	for (int i = 0; i < len; i++)
	{
		if (pFirst[i])printf("\nFIRST[%c] = %s", pall[i], pFirst[i]);
	}

}
void output_follow(char **pFollow, char *non_terminal)
{
	int len = strlen(non_terminal);
	int i;
	printf("\n******************��� follow ��*******************\n");
	for (i = 0; i < len; i++)
	{
		printf("\nFollow[%c] = %s", non_terminal[i], pFollow[i]);
	}
	printf("\n");
}

//�ж�һ��string���Ƿ����ĳһ���ַ�  �����������true
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

// �������ж�ȡ���  �洢���ı���
// ֻ����һ��һ������
void InputToFile(char *filepath)
{
	FILE *fp;
	char string[20];
	char tmp[20];
	int flag_num = 1;
	printf("����������ʽ   ~�����  $����ʼ����\n");
	fgets(string, sizeof(string), stdin);
	while (strcmp(string, "") == 0)
		fgets(string, sizeof(string), stdin);

	fp = fopen(filepath, "w");
	if (fp == NULL)
	{
		printf("�ļ��޷���\n");
		exit(0);
	}

	while (string[0] >= 'A'&&string[0] <= 'Z'&&string[1] == '-'&&string[2] == '>'&&string[3] != '\0')
	{
		int i = 3;
		char *p = string + 3;
		char *q = string + 3;


		if (flag_num == 1) //���������ĵ�һ������ʽ
		{
			tmp[0] = '$'; tmp[1] = '-'; tmp[2] = '>'; tmp[3] = string[0]; tmp[4] = '\0';
			fprintf(fp, "%s\n", tmp);
		}
		flag_num++;
		fprintf(fp, "%s\n", string);   //���������洢���ı���
		fgets(string, sizeof(string), stdin);  //��ȡ��һ���ı�
	}
	fclose(fp);
}

// �����ȡ��������ʽ
void OutPutRegu(pRegu pregu)
{
	printf("�������ʽ\n");
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
		printf("�ļ���ʧ��\n");
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
//���ս���ͷ��ս������
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
			//���� "->"
			if (*p == '-')
			{
				p += 2;
				continue;
			}
			//����������ս��
			if ('A' <= *p&&*p <= 'Z' || '$' == *p)
			{
				index = strlen(*non_terminal);
				*non_terminal = (char*)realloc(*non_terminal, sizeof(char)*(index + 2));
				(*non_terminal)[index] = '\0';

				//����÷��ս���Ѿ������ ����
				if (have(*non_terminal, *p)) p++;
				else
				{
					index = strlen(*non_terminal);
					(*non_terminal)[index] = *p;
					(*non_terminal)[++index] = '\0';
					p++;
				}
			}
			//��������� ����
			else if (*p == '~')
			{
				p++;
				continue;
			}
			//��������ս��
			else
			{
				index = strlen(*terminal);
				(*terminal) = (char*)realloc((*terminal), sizeof(char)*(index + 2));
				(*terminal)[index] = '\0';

				//������ս���Ѿ������
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

//�����ж�ĳ������ʽ�Ҳ� �Ƿ��ս��  ���з���true
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
//�����ж� ĳ�����ս���Ƿ��Ѿ�ȫ��ɾ�� ȫ��ɾ������ true
int NotExitInRegu(char symbol, char**pregu, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!pregu[i])continue;
		if (symbol == pregu[i][0])return FALSE;
	}
	return TRUE;
}
//���ڻ�ȡһ���ַ� ���ַ����е�λ��
int getindex(char *pstr, char symbol)
{
	int index;
	int len = strlen(pstr);
	for (index = 0; index < len; index++)
	{
		if (pstr[index] == symbol)return index;
	}
	printf("�Ҳ������ַ�1\n");
	return -1;
}
//�����ж��Ƿ����з��ս�������Ƶ���
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
//�ж��Ҳ��Ƿ���� ȷ����Ϊ�յķ��ս��
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
	//���Ƚ�������ʽ�ĸ���
	for (int i = 0; i < regu->num; i++)
	{
		pregu[i] = (char*)malloc(sizeof(char)*(strlen(regu->pregu[i]) + 1));
		strcpy(pregu[i], regu->pregu[i]);
	}

	for (int i = 0; i < regu->num; i++) (*pnull)[i] = NOTSURE;  //��ʼ������״̬Ϊ��ȷ��

	for (int i = 0; i < regu->num; i++)
	{
		if (!pregu[i])continue;
		//�������ֱ����ݹ�  ֱ��ɾ��
		if (pregu[i][0] == pregu[i][3])
		{
			tmp = pregu[i][0];
			free(pregu[i]);
			pregu[i] = NULL;

			//����Ѿ�ȫ��ɾ��
			if (NotExitInRegu(tmp, pregu, regu->num))
			{
				index = getindex(non_terminal, tmp);
				(*pnull)[index] = NO;
			}
			continue;
		}
		//��������ս��
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
		//���������
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

	//��������������
	while (exit_notsure(*pnull, strlen(non_terminal)))
	{
		for (int i = 0; i < regu->num; i++)
		{
			if (!pregu[i])continue;
			pleft_rec[getindex(non_terminal, pregu[i][0])] = TRUE;

			//��������һ����Ϊ�յķ��ս��
			len = strlen(pregu[i]);
			for (k = 3; k < len; k++)
			{
				if (YES == (*pnull)[getindex(non_terminal, pregu[i][k])]) continue;
				else break;
			}

			//���������Ƿ��ս��  ����״̬��ȷ��
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


			//����Ҳ඼���Ƴ���   ���Ϊ�� ��ɾ�����з���
			if (all_ars_null(pregu[i], *pnull, non_terminal))
			{
				index = getindex(non_terminal, pregu[i][0]);
				(*pnull)[index] = YES;
				tmp = pregu[i][0];                                   //�˴���ɾ�����Ҽӵ�

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
			//����Ҳ�������в����յķ��ս��
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
	//����÷��ŵ�first���ϻ�û�з���� �����ڴ没�洢
	if (!pgather[index])
	{
		pgather[index] = (char*)malloc(sizeof(char) * 2);
		pgather[index][0] = newsym;
		pgather[index][1] = '\0';
	}
	else
	{
		//����÷����Ѿ���������� �����ټ���
		if (have(pgather[index], newsym))
			return;
		len = strlen(pgather[index]);
		pgather[index] = (char*)realloc(pgather[index], sizeof(char)*(len + 1));
		pgather[index][len] = newsym;
		pgather[index][len + 1] = '\0';
	}
}

//���ظ÷��ս�� �Ƿ���ת�Ƶ���
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
//�ж��Ҳ��ǲ���ȫ���Ƿ��ս��
int IsNonterminal(char *pregu, char *terminal)
{
	char *p = pregu;
	for (int i = 3; i < strlen(p); i++)
	{
		if (have(terminal, p[i]))return FALSE;
	}
	return TRUE;
}

// �����������ĺϲ�first����
void  FirstAdd(char symbol, char newsym, int id, char *pall, char **pFirst)
{
	//id �����ж��Ƿ����  ���������
	int indexn = getindex(pall, newsym);
	int j;
	int len;
	//���������ķ�����Ҫ����
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

//��ȡ��һ�������Ƶ��յ��±�
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
	//flag �����жϸ��ַ��� first�����Ƿ��Ѿ������
	//������ż���÷��ŵ�first����
	char **p = pregu->pregu;
	char *ptmp = *p;
	int j = 0;

	//�����ǰ���ŵ�  first�����Ѿ����  ֱ�ӷ���
	if (flag[getindex(pall, symbol)])return symbol;
	//����÷������ս��
	if (have(terminal, symbol))
	{
		AddToFirst(symbol, symbol, pall, pFirst);
		return symbol;
	}

	//�����������Ƿ��ս��
	else if (have(non_terminal, symbol))
	{
		//����ݹ���Ϊ true ���ڱ��
		if (!pleft_rec[getindex(non_terminal, symbol)])pleft_rec[getindex(non_terminal, symbol)] = TRUE;

		for (j = 1; j < pregu->num; j++)
		{
			if (p[j])
			{
				//�ҵ�һ���Ե�ǰsymbol��ͷ�Ĳ���ʽ
				ptmp = p[j];
				if (ptmp[0] != symbol)continue;
				//����÷��ս���Ϳ���ת�Ƶ���   ����kong
				if (can_obtain_null(non_terminal, pnull, symbol))
				{
					AddToFirst(symbol, '~', pall, pFirst);
				}
				//����Ҳ��Է��ս����ʼ
				if (have(terminal, ptmp[3]))
				{
					AddToFirst(symbol, ptmp[3], pall, pFirst);
				}
				//����ǿ�
				else if ('~' == ptmp[3])continue;
				//����Ƿ��ս��
				else
				{
					//����Ҳ�����ʽ�������ս�� ��ֱ����ݹ�
					if (IsNonterminal(p[j], terminal) && p[j][0] != p[j][3])
					{
						//��ȡ��һ�������Ƴ��յķ��ս��
						int length = getPos(p[j], non_terminal, pnull);

						//�������ȫ�����Ƶ���
						if (length)
						{
							int i;
							for (i = 3; i < length; i++)
							{
								//��������˵ݹ���ѭ��  break��
								if (pleft_rec[getindex(non_terminal, ptmp[i])])break;
								FirstAdd(symbol,
									GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
									TRUE, pall, pFirst
								);
							}

							//�����������Ѿ����first���ϣ�ֱ�Ӽ���  ���������ټ���
							if (pleft_rec[getindex(non_terminal, ptmp[i])])
								FirstAdd(symbol, ptmp[i], FALSE, pall, pFirst);
							else
								FirstAdd(symbol, GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
									FALSE, pall, pFirst);


						}

						//����Ҳ������Ƶ�ȫ��
						else
						{
							int length = strlen(p[j]);
							int i;
							for (i = 3; i < length; i++)
							{
								//��������˵ݹ���ѭ��  break��
								if (pleft_rec[getindex(non_terminal, ptmp[i])])break;
								FirstAdd(symbol,
									GetFirst(ptmp[i], pFirst, pregu, pall, terminal, non_terminal, pnull, flag, pleft_rec),
									FALSE, pall, pFirst
								);

							}

							AddToFirst(symbol, '~', pall, pFirst);
						}

					}

					//��������ս��  �����ҵݹ�
					else
					{
						int i = 3;
						ptmp = p[j];
						//���ǰһ���ǿվͼ���ִ��
						do
						{
							//��ֹ�ݹ���ѭ��
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

	//����
	if (have(non_terminal, symbol))
	{
		pleft_rec[getindex(non_terminal, symbol)] = FALSE;
	}
	return symbol;

}
//�����ַ��±�
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
//�ϲ� follow��
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

	//����Ѿ��������  ��ֱ�ӷ���
	if (flag[getindex(non_terminal, symbol)]) return symbol;

	pleft_rec[getindex(non_terminal, symbol)] = TRUE;

	for (j = 0; j < pregu->num; j++)
	{
		if (p[j])
		{
			//�ҵ���ǰ�������Ҳ�����ʽ��λ��
			int pos = contain_symbol(p[j], symbol);
			int length = strlen(p[j]);

			int i;
			char *ptmp = p[j];

			if ('$' == symbol)AddToFirst(symbol, '#', non_terminal, pFollow);

			if (pos == FALSE)continue;

			//��������Ҷ�
			if (pos == length - 1)
			{
				if (ptmp[0] == ptmp[pos])continue;
				//���󲿼���follow��
				else if (pleft_rec[getindex(non_terminal, ptmp[0])])
					follow_add(symbol, ptmp[0], non_terminal, pFollow);
				else
					follow_add(symbol,
						GetFollow(ptmp[0], pFirst, pFollow, pregu, pall, non_terminal, pnull, flag, pleft_rec),
						non_terminal, pFollow);
			}

			//����
			else
			{
				for (i = pos + 1; i < length; i++)
				{
					//����÷��Ų����Ƶ���  ������Ϳ��˳�
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
				//������еķ��Ŷ�������
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
//��ȡ������ĺ�����
void GetAllSymbol(char **pall, char *non_terminal, char *terminal)
{
	int len = strlen(non_terminal) + strlen(terminal) + 2;
	*pall = (char*)malloc(sizeof(char)*len);
	strcpy(*pall, terminal);
	strcat(*pall, "#");
	strcat(*pall, non_terminal);
}

//�� .
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
//��ʼ��
void InitDFA(pProject * ppro)
{
	(*ppro) = (pProject)malloc(sizeof(Project));
	(*ppro)->pclosure = (pClosure)malloc(sizeof(Closure));
	(*ppro)->num = 0;
	(*ppro)->pclosure->num = 0;
	(*ppro)->pclosure->preguc = NULL;
}

//��ȡ . ����ķ���
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

//������̬ ����հ�
void calc_closure(pProject *ppro, int index, pRegu pregu, char *pregulate, char *non_terminal)
{
	//��ֱ̬�Ӽ����Ӧ�ıհ�
	int num;
	int len = strlen(pregulate);
	char symbol;
	int i, j;
	char *ptmp_regu = NULL;
	num = (*ppro)[index].pclosure->num;

	//��ʼ��
	(*ppro)[index].pclosure->preguc = (char**)realloc((*ppro)[index].pclosure->preguc, (num + 1) * sizeof(char*));
	(*ppro)[index].pclosure->preguc[num] = (char*)malloc(sizeof(char)*(len + 1));
	strcpy((*ppro)[index].pclosure->preguc[num], pregulate);
	(*ppro)[index].pclosure->num++;
	num = (*ppro)[index].pclosure->num;

	for (i = 0; i < (*ppro)[index].pclosure->num; i++)
	{
		//��ȡ . ������ŵ��ַ�
		symbol = get_symbol_after_dot((*ppro)[index].pclosure->preguc[i]);

		//���������ķ��ս��  ������չ
		if (have(non_terminal, symbol))
		{
			for (j = 0; j < pregu->num; j++)
			{
				//�����������  ����
				if (symbol == pregu->pregu[j][0])
				{
					num = (*ppro)[index].pclosure->num;
					InsertDot(&ptmp_regu, pregu->pregu[j]); // jia .

															//������ʽ���Ѿ������� ����
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

//���ܻ���ּ��� ����ʽ����ͬһ����Ŀ��
int ExistDFA(char *pregu, Analyze pana, char *pall, int *pindex)
{
	int index;
	char symbol;
	char *p;
	symbol = get_symbol_after_dot(pregu);
	index = getindex(pall, symbol);
	//������յ�  ���ش���
	if (pana.pparse->pproceed[index] == NULL) return FALSE;
	else
	{
		p = pana.pparse->pproceed[index];

		//���붯��
		if ('S' == p[0])
		{
			p++;
			*pindex = Str2Int(p); //������Ҫ��д����Ŀ�����
			return TRUE;
		}
		//goto��
		else if ('0' <= p[0] && p[0] <= '9')
		{
			*pindex = Str2Int(p);
			return TRUE;
		}
		//����ǹ�Լ  ������ͻ  
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

	//�ж���ԭ��Ŀ�����Ƿ����ҵ� ����Ŀ��  
	for (i = 0; i <= (*ppro)->num; i++)
	{
		if (Found(ptemp_regulate, (*ppro)[i].pclosure))
		{
		   //����ڸ���Ŀ���� �ҵ��� �����  �ͱ�������Ŀ��

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

//�ж��Ƿ������ͬ��ת��·��
int IsnewDFA(pProject *ppro, int num, char *pregulate)
{
	int i, j;
	char *pregu = NULL;
	char *ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
	strcpy(ptmp_regu, pregulate);
	dot_move_back(ptmp_regu);

	//�ж���ԭ��Ŀ�����Ƿ����ҵ�����Ŀ��     
	for (i = 0; i <= (*ppro)->num; i++)
	{
		//�����ĳ��״̬�����ҵ���Ҫ��ӵ� ��
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
//���������
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
			printf("����SLR��1���ķ�");
			exit(1);
		}
		(*pana)[status].pparse->pproceed[index] = (char*)malloc(sizeof(char)*(len + 1));
		strcpy((*pana)[status].pparse->pproceed[index], itostr);

		break;
	case REDUCE:
		//��� . ����ʲôҲ����
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

		//ɨ�����з��ս��  �ж��Ƿ���ڷ��ս��
		for (j = 0; j < length; j++)
		{
			//�жϸ��ս���Ƿ���follow ����
			if (!have(pFollow[index], terminal[j]))continue;
			if ((*pana)[status].pparse->pproceed[j])
			{
				printf("����SLR1");
				exit(1);
			}
			(*pana)[status].pparse->pproceed[j] = (char*)malloc(sizeof(char)*(len + 2));
			//����������  ����
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
			printf("����SLR1");
			exit(1);
		}
		(*pana)[status].pparse->pproceed[index] = (char*)malloc(sizeof(char)*(len + 2));

		strcpy((*pana)[status].pparse->pproceed[index], "S");
		strcat((*pana)[status].pparse->pproceed[index], itostr);

		break;
	default:
		printf("����SLR1");
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

	//��������Ŀ��ÿһ�����   �������Ӧ��״̬ת�Ʊ�  ��  �µĺ��״̬��
	for (int i = 0; i < num; i++)
	{
		pregulate = (*ppro)[index].pclosure->preguc[i];
		tmpchar = get_symbol_after_dot(pregulate);


		if (tmpchar != '\0')
		{
			//����Ѿ�������Ӧ��DFA  ���ǲ�����  ���� ͬһ����ʼ״̬�� �еļ������ת�Ƶ�ͬһ���µĺ��״̬  �� I0 �� ��һ���͵ڶ���
			if (ExistDFA(pregulate, (*pana)[index], pall, &toindex))
			{
				//����Ѿ�����ת��
				if (toindex > index) //�Ҿ������״̬һ���Ǵ��ڵ�  ��Ϊ�ӵ�ǰ״̬  ת�Ƶ����״̬��
				{
					ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
					strcpy(ptmp_regu, pregulate);
					dot_move_back(ptmp_regu);
					calc_closure(ppro, toindex, pregu, ptmp_regu, non_terminal);  //�������Ŀ
				}
			}

			else
			{
				//�������Ŀ�Ѿ���ȫ����   �����������ɸò���ʽת�Ƴ�ȥ�ıհ�   �� I4 E->.T  ����T�ֻص�״̬2   ��ǰת��
				//��ʱ����Ҫ���ظ������ ����ʽ�� dfa ֻ��Ҫ��������� ������
				if (toindex = is_newDFA(ppro, index, pregulate) != NO) //����ܻص�����
				{
					calc_analyze_table(pregulate, pFollow, pana, index, toindex, pregu, non_terminal, terminal, pall);
				}

				//
				else
				{
					*ppro = (pProject)realloc(*ppro, sizeof(Project)*((*ppro)->num + 2));
					if (!*ppro)
					{
						printf("�ڴ�������");
						exit(1);
					}

					//��ʼ��һ���±�
					(*ppro)->num++;
					(*ppro)[(*ppro)->num].pclosure = (pClosure)malloc(sizeof(Closure));
					(*ppro)[(*ppro)->num].pclosure->preguc = NULL;
					(*ppro)[(*ppro)->num].pclosure->num = 0;

					ptmp_regu = (char*)malloc(sizeof(char)*(strlen(pregulate) + 1));
					strcpy(ptmp_regu, pregulate);
					dot_move_back(ptmp_regu);

					//�����̬��ȡ��̬
					calc_closure(ppro, (*ppro)->num, pregu, ptmp_regu, non_terminal);

					//���������
					calc_analyze_table(pregulate, pFollow, pana, index, (*ppro)->num, pregu, non_terminal, terminal, pall);


				}
			}



		}
		else
		{
			//����ò���ʽ����û�ж�����   ��ֻ��Ҫ�����Լ�ķ����  �����ù���dfa   ��Ϊû��ת�Ƶĺ��״̬
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
	printf("\n****************�����Ŀ************************\n");
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
	printf("\n********************������********************\n");
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

//��ȡ�û�����
void GetInput(char **pstr, char *terminal)
{
	char t;
	int i = 0;
	printf("������\n");
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
	pProject pPro = NULL;    //���ڴ洢�������Ŀ��
	pAnalyze pAna = NULL;    //���ڴ洢����ķ����
	StackState sstate;
	StackSymbol ssymbol;
							 //	StackSymbol sSymbol;

	char *ptmp_reg = NULL;    //�����ݴ����ʽ
	char filepath[] = "productions.txt";  //���ڴ洢����ʽ��·��


	char *pAllSymbol = NULL;    //���ڴ洢���з���
	char *non_terminal = NULL;  //���ڴ洢���ս��
	char *terminal = NULL;      //���ڴ洢�ս��
	char **pFirst = NULL;     //���ڴ洢first����
	char **pFollow = NULL;    //���ڴ洢follow����
	int *pnull = NULL;        //���ڴ洢�÷��ս����Ӧ�ķ����Ƿ��ܵ��� ��
	int *pleft_rec = NULL;     //�����ж��Ƿ���ֱ����ݹ�
	char *ptmp = NULL;
	int *flag = NULL;
	char *pcontent = NULL; //���ڴ洢�û�����

	int i;
	int k;
	int lenOfAllSymbol;
	int len;

	printf("****************SLR(1)******************\n");

	getchar();

	InputToFile(filepath); // �������ж�ȡ���  �洢���ı���
	GetFromFile(&regu, filepath); // ���ı��ж�ȡ����ʽ
	OutPutRegu(&regu);

	if (0 == regu.num)
	{
		printf("δ��ȡ������ʽ ����\n");
		exit(0);
	}
	GetSymbol(&regu, &non_terminal, &terminal);  //���ս���ͷ��ս������

	len = strlen(non_terminal) + 1;              //��ʼ����ݹ麯��
	pleft_rec = (int*)malloc(sizeof(int)*len);
	for (i = 0; i < len; i++)pleft_rec[i] = false;

	//���������ת�Ƶ� �� �ķ��ս��
	GetNull(&pnull, &regu, non_terminal, terminal, pleft_rec);
	//output_null(pnull, strlen(non_terminal));   ���ڵ���  ������ս���Կյ��ж�

	printf("����ս���\n");

	GetAllSymbol(&pAllSymbol, non_terminal, terminal);

	lenOfAllSymbol = strlen(pAllSymbol);
	len = strlen(non_terminal) + 1;

	pFirst = (char**)malloc(sizeof(char*)*lenOfAllSymbol);
	pFollow = (char**)malloc(sizeof(char*)*len);

	//���г�ʼ��
	for (i = 0; i < lenOfAllSymbol; i++)pFirst[i] = NULL;
	for (i = 0; i < len; i++)
	{
		pleft_rec[i] = FALSE;
		pFollow[i] = NULL;
	}


	init_flag(&flag, lenOfAllSymbol);      //��ʼ�� flag Ϊfalse  Ȼ��������
	for (i = 0; i < lenOfAllSymbol; i++)
	{
		GetFirst(pAllSymbol[i], pFirst, &regu, pAllSymbol, terminal, non_terminal, pnull, flag, pleft_rec);
	}
	output_fisrt(pFirst, pAllSymbol);

	//���³�ʼ��flag
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
		printf("�����ڴ�ʧ��");
	}

	strcpy(ptmp, terminal);
	strcat(ptmp, "#");

	terminal = ptmp;

	InsertDot(&ptmp_reg, regu.pregu[0]);//�������
	InitDFA(&pPro);                    //��ʼ��DFA 
	calc_closure(&pPro, 0, &regu, ptmp_reg, non_terminal); //��������ıհ�

	lenOfAllSymbol = strlen(pAllSymbol);

	//���ݳ�̬������DFA ������״̬��

	for (int i = 0; i <= pPro->num; i++)
	{
		//��ʼ��������  ��Ӧ��ĳ��
		pAna = (pAnalyze)realloc(pAna, sizeof(Analyze)*(i + 1));
		pAna->num = i;

		pAna[i].pparse = (pParse)malloc(sizeof(Parse));
		pAna[i].pparse->num = lenOfAllSymbol;
		pAna[i].pparse->pproceed = (char**)malloc(sizeof(char*)*lenOfAllSymbol);

		for (k = 0; k < lenOfAllSymbol; k++)pAna[i].pparse->pproceed[k] = NULL;

		//���к��״̬�ı���  ���Ƿ������쵽�µ� ״̬
		GetDFA(&pPro, i, pFollow, &regu, &pAna, pAllSymbol, non_terminal, terminal);

	}

	output_project(pPro, pPro->num);
	output_DFA(pAna, pAllSymbol);
	output_analyze_table(pAna, pAllSymbol);

	printf("\n�˳���ɽ��� SLR(1) ����");
	GetInput(&pcontent, terminal);
	parsecontent(pcontent, &sstate, &ssymbol, &regu, pAna, pAllSymbol);

	printf("\n����\n");
	return 0;

}




//ֻ�� isnewdfa������������

//��Ҫ�Ķ���