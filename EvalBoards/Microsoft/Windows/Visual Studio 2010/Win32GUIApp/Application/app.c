/*
*********************************************************************************************************
*
*                                          Trabalho Prático - BCC722
*
*                                         TARGET = Microsoft Windows
*
* Arquivo			: app.c
* Versao			: 1.6
* Aluno(s)			: Paulo Felipe Possa Parreira e Jader Miranda
* Data				: 30/06/2018
* Descricao			: Implementação em uCOS-III do artigo "rtos based automatic scheduling for military application"
*********************************************************************************************************
*/

/* Includes */
#include <app_cfg.h>
#include <os.h>
#include <stdlib.h>
#include <os_cfg_app.h>
#include <windows.h>
#include <conio.h>

/* Constantes */
#define TSKSTART_STK_SIZE 2000
#define DIMENSAOCAMPO 20


/* Declaração dos TCBs e STKs para as tarefas*/
static OS_TCB AppStartTaskTCB;
static CPU_STK AppStartTaskStk[TSKSTART_STK_SIZE];
static OS_TCB AppTaskMovimentarRoboTCB;
static CPU_STK AppTaskMovimentarRoboStk[TSKSTART_STK_SIZE];
static OS_TCB AppTaskVerificarArredoresTCB;
static CPU_STK AppTaskVerificarArredoresStk[TSKSTART_STK_SIZE];
static OS_TCB AppTaskExplodirBombaTCB;
static CPU_STK AppTaskExplodirBombaStk[TSKSTART_STK_SIZE];
static OS_TCB AppTaskDesarmarBombaTCB;
static CPU_STK AppTaskDesarmarBombaStk[TSKSTART_STK_SIZE];

/* Declaração dos Semáforos */
OS_SEM mutexMotor; // Mutex do motor - impede o movimento do motor caso algum evento aconteça.
OS_SEM mutexSensorBomba; // Mutex do sensor de bombas - impede que mais de um recurso procure por bombas ao mesmo tempo.
OS_SEM mutexDefused; // Mutex do desarmador de bombas - bloqueia ações enquanto o robô está desarmando a bomba
OS_SEM mutexDesarmador; // Mutex que habilita o desarmador de bombas
OS_SEM semDesenharCampo; // Mutex que sincroniza o desenho do campo - sincronização

/**
*  Definição do campo e variáveis do robo
*/
int campo[DIMENSAOCAMPO][DIMENSAOCAMPO];
int robo[2];
int bombaEncontradaFlag[4] = {0,0,0,0};

/* Macros */
#define  APP_TASK_STOP();                             { while (DEF_ON) { \
	;            \
}                \
}

#define  APP_TEST_FAULT(err_var, err_code)            { if ((err_var) != (err_code)) {   \
	APP_TASK_STOP();             \
}                                \
}

/* Protótipos */
static void AppTaskVerificarArredores(void  *p_arg);
static void AppTaskMovimentarRobo(void  *p_arg);
static void preencherCampo();
static void desenharCampo();
void ClearScreen();

/**
* PreencherCampo
* Função que preenche o campo e define onde estão as bombas, necessita de uma seed para o srand
*/
static void preencherCampo(int seed) {
	int i = 0;
	int j = 0;

	srand(seed);
	for(i = 0; i < DIMENSAOCAMPO; i++) {
		for(j = 0; j < DIMENSAOCAMPO; j++) {
			if(rand() % 100 >= 85) {
				campo[i][j] = 1;
			} else {
				campo[i][j] = 0;
			}
		}
	}
}

/** 
* Cria a chance de ocorrer uma explosão nos arredores e avisa ao operador para ficar atento
*/
static void AppTaskExplodirBomba() {
	OS_ERR err_os;
	CPU_TS ts;
	int i = 0;
	int j = 0;
	while(1) {
		for(i = 0; i < DIMENSAOCAMPO; i++) {
			for(j = 0; j < DIMENSAOCAMPO; j++) {
				if(campo[i][j] == 1) {
					if(rand() % 100 > 90) {
						campo[i][j] = 2;
						OSSemPend(&mutexSensorBomba, 500, OS_OPT_PEND_BLOCKING, &ts, &err_os);
						if(err_os != OS_ERR_TIMEOUT) {
							OSSemPend(&semDesenharCampo,0,OS_OPT_PEND_BLOCKING, &ts, &err_os);
							desenharCampo();
							printf("\nUma bomba explodiou na pos [i][j] - ATENCAO");
							OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_DLY, &err_os);
							OSSemPost(&semDesenharCampo,OS_OPT_POST_ALL, &err_os);
							OSSemPost(&mutexSensorBomba,OS_OPT_POST_ALL, &err_os);
						}
					};
					OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_DLY, &err_os);
				}
			}
		}
	}
}

/**
* Realiza o desenho da tela
*/
static void desenharCampo() {
	int i = 0;
	int j = 0;
	ClearScreen();
	printf("BCC722 - Programacao em Tempo Real 18.1\nPaulo Felipe e Jader Miranda\nSimulacao de um Robo Detector de Metais\n");
	for(i = 0; i<= DIMENSAOCAMPO; i++)
		printf("===");
	printf("\n");
	for(i = DIMENSAOCAMPO-1; i >= 0; i--) {
		for(j = 0; j < DIMENSAOCAMPO; j++) {
			if(robo[0] == i && robo[1] == j) {
				printf(" r ");
			}
			else if(campo[i][j] == 1) {
				printf(" * ");
			}
			else if(campo[i][j] == 2) {
				printf(" x ");
			} else {
				printf("   ");

			}
		}
		printf("\n");
	}
	if(bombaEncontradaFlag[0] == 1 ) {
		printf("Bomba encontrada na posicao [%d][%d] - Robo Paralizado 1",robo[0]+1,robo[1]+2);
	}	
	if(bombaEncontradaFlag[1] == 1 ) {
		printf("Bomba encontrada na posicao [%d][%d] - Robo Paralizado 2",robo[0]+1,robo[1]);
	}
	if(bombaEncontradaFlag[2] == 1 ) {
		printf("Bomba encontrada na posicao [%d][%d] - Robo Paralizado 3",robo[0],robo[1]+1);
	}
	if(bombaEncontradaFlag[3] == 1 ) {
		printf("Bomba encontrada na posicao [%d][%d] - Robo Paralizado 4",robo[0]+2,robo[1]+1);
	}		
}

/**
* Função que limpa a tela do terminal utilizando a lib do windows
*/
void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition( hStdOut, homeCoords );
}


/**
* AppTaskDesarmarBomba
* Sempre que o robô encontrar uma bomba ele tentará desarmá-la, se o desarmador estiver disponível. 
* Caso contrário ele apenas avisa ao operador que existe uma bomba na proximidade.
*/
static void AppTaskDesarmarBomba (void *p_arg) {
	CPU_TS ts;
	OS_ERR err_os;
	int flagDesarmou = 0;
	while(1) {
		OSSemPend(&mutexDesarmador,0,OS_OPT_PEND_BLOCKING,&ts,&err_os);
		if(robo[1] < 19 && campo[robo[0]][robo[1]+1] == 1) {
			campo[robo[0]][robo[1]+1] = 0;
			flagDesarmou = 1;
		}
		if(robo[1] > 0 && campo[robo[0]][robo[1]-1] == 1) {
			campo[robo[0]][robo[1]-1] = 0;
			flagDesarmou = 1;
		} 
		if(robo[0] > 0 && campo[robo[0]-1][robo[1]] == 1) {
			campo[robo[0]-1][robo[1]] = 0;
			flagDesarmou = 1;
		} 
		if(robo[0] < 19 && campo[robo[0]+1][robo[1]] == 1) {
			campo[robo[0]+1][robo[1]] = 0;
			flagDesarmou = 1;
		} 
		if(flagDesarmou == 1) {
			OSSemPend(&semDesenharCampo,50,OS_OPT_PEND_BLOCKING, &ts, &err_os);
			if(err_os != OS_ERR_TIMEOUT) {
				desenharCampo();
				printf("\nDesarmando Bombas!");
				OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_DLY, &err_os);
				desenharCampo();
				OSSemPost(&semDesenharCampo,OS_OPT_POST_ALL, &err_os);
			} else {
				OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_DLY, &err_os);
				OSSemPost(&semDesenharCampo,OS_OPT_POST_ALL, &err_os);
			}
		}
		flagDesarmou = 0;
		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_DLY, &err_os);
		OSSemPost(&mutexDefused,OS_OPT_PEND_BLOCKING, &err_os);
		OSSemPost(&mutexDesarmador,OS_OPT_PEND_BLOCKING, &err_os);
	}
}

/**
* verificarArredores
* Realiza a verificação dos arredores do robo em busca de bombas para serem desarmadas.
* A ocorrência de bombas nos arredores deve bloquear a movimentação do robo e avisar no console que uma bomba foi encontrada.
*/
static void AppTaskVerificarArredores (void  *p_arg){	
	CPU_TS ts;
	OS_ERR  err_os;
	while(1) {
		OSSemPend(&mutexSensorBomba, 0, OS_OPT_PEND_BLOCKING, &ts, &err_os);
		/* Verifica se cada um dos 4 cantos possui bombas */
		if(robo[1] < 19 && campo[robo[0]][robo[1]+1] == 1) {
			bombaEncontradaFlag[0] = 1;
		} else {
			bombaEncontradaFlag[0] = 0;
		}
		if(robo[1] > 0 && campo[robo[0]][robo[1]-1] == 1) {
			bombaEncontradaFlag[1] = 1;
		} else {
			bombaEncontradaFlag[1] = 0;
		}
		if(robo[0] > 0 && campo[robo[0]-1][robo[1]] == 1) {
			bombaEncontradaFlag[2] = 1;
		} else {
			bombaEncontradaFlag[2] = 0;
		}
		if(robo[0] < 19 && campo[robo[0]+1][robo[1]] == 1) {
			bombaEncontradaFlag[3] = 1;
		} else {
			bombaEncontradaFlag[3] = 0;
		}
		OSSemPost(&mutexSensorBomba, OS_OPT_POST_ALL, &err_os);

		/* Desabilita o motor e ativa o desarmador caso encontre uma bomba */
		if(bombaEncontradaFlag[0] == 1 || bombaEncontradaFlag[1] == 1 || bombaEncontradaFlag[2] == 1 || bombaEncontradaFlag[3] == 1) {
			OSSemPend(&mutexMotor,0,OS_OPT_PEND_BLOCKING, &ts, &err_os);
			OSSemPost(&mutexDesarmador, OS_OPT_POST_ALL, &err_os);
			OSSemPend(&mutexDefused,0,OS_OPT_PEND_BLOCKING,&ts,&err_os);
			OSSemPost(&mutexMotor,OS_OPT_PEND_BLOCKING, &err_os);
		}

		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_DLY, &err_os);
	}	
	OSTaskDel(&AppTaskVerificarArredoresTCB, &err_os);
}




/**
* movimentarRobo
* Movimenta o robo através do campo, utilizando as teclas WASD
*/
static void AppTaskMovimentarRobo (void  *p_arg) {
	CPU_TS ts;
	OS_ERR  err_os;


	char tecla;
	while(1) {
		// Verifica se o motor está disponível e recebe a tecla
		OSSemPend(&mutexMotor, 500, OS_OPT_PEND_BLOCKING, &ts, &err_os);
		if(err_os != OS_ERR_TIMEOUT) {
			tecla = getch();
			switch(tecla)
			{
			case 'w':
			case 'W':
				if(bombaEncontradaFlag[3] != 1)
					robo[0]++;
				break;
			case 'a':
			case 'A':		
				if(bombaEncontradaFlag[1] != 1)	
					robo[1]--;
				break;
			case 's':
			case 'S':
				if(bombaEncontradaFlag[2] != 1)
					robo[0]--;
				break;
			case 'd':
			case 'D':
				if(bombaEncontradaFlag[0] != 1)
					robo[1]++;
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_DLY, &err_os);
		}		
		OSSemPost(&mutexMotor, OS_OPT_POST_ALL, &err_os);

		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_DLY, &err_os);
		desenharCampo();
	}	

}



/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Inicialização das tarefas e exemplificação.
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
* Returns     : none.
* Created by  : main().
*
*********************************************************************************************************
*/
static  void  App_TaskStart (void  *p_arg)
{

	OS_ERR  err_os;
	CPU_TS ts;

	/* Preenche e desenha o campo */
	preencherCampo(43293439);
	desenharCampo();

	/* Posição do Robo [x e y] */
	robo[0] = 0;
	robo[1] = 0;


	/** Inicializa os Semaforos */
	OSSemCreate(&mutexMotor, "mut_motor", 1,
		&err_os); 
	OSSemCreate(&mutexSensorBomba, "mut_sensor", 1,
		&err_os);
	OSSemCreate(&mutexDefused, "mut_defused", 0,
		&err_os);
	OSSemCreate(&mutexDesarmador, "mut_desarmador", 1,
		&err_os);
	OSSemCreate(&semDesenharCampo, "mut_desenhar_campo", 1,
		&err_os);


	/** Inicializa as Tarefas **/

	OSTaskCreate((OS_TCB*)&AppTaskMovimentarRoboTCB, 
		(CPU_CHAR*)"MovimentarRobo",
		(OS_TASK_PTR)AppTaskMovimentarRobo, (void*)0, 
		(OS_PRIO)1,
		(CPU_STK*)&AppTaskMovimentarRoboStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE, (OS_MSG_QTY)0u,
		(OS_TICK)0u, (void*)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR*)&err_os);

	OSTaskCreate((OS_TCB*)&AppTaskVerificarArredoresTCB, 
		(CPU_CHAR*)"VerificarArredores",
		(OS_TASK_PTR)AppTaskVerificarArredores, (void*)0, 
		(OS_PRIO)9,
		(CPU_STK*)&AppTaskVerificarArredoresStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE, (OS_MSG_QTY)0u,
		(OS_TICK)0u, (void*)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR*)&err_os);

	OSTaskCreate((OS_TCB*)&AppTaskExplodirBombaTCB, 
		(CPU_CHAR*)"ExplodirBomba",
		(OS_TASK_PTR)AppTaskExplodirBomba, (void*)0, 
		(OS_PRIO)1,
		(CPU_STK*)&AppTaskExplodirBombaStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE, (OS_MSG_QTY)0u,
		(OS_TICK)0u, (void*)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR*)&err_os);

	OSTaskCreate((OS_TCB*)&AppTaskDesarmarBombaTCB, 
		(CPU_CHAR*)"DesarmarBomba",
		(OS_TASK_PTR)AppTaskDesarmarBomba, (void*)0, 
		(OS_PRIO)15,
		(CPU_STK*)&AppTaskDesarmarBombaStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE, (OS_MSG_QTY)0u,
		(OS_TICK)0u, (void*)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR*)&err_os);

	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	while(1){
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_DLY, &err_os);
	}

}


/*
*********************************************************************************************************
*                                               main()
*
* Description : Funcao principal do programa.
* Arguments   : none.
* Returns     : none.
* Note(s)     : 
*********************************************************************************************************
*/

int  main (void)
{
	OS_ERR  err_os;

	OSInit(&err_os);                                            /* inicializa uC/OS-III.                                */
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	OSTaskCreate(
		(OS_TCB*)&AppStartTaskTCB, /* Cria a tarefa inicial.*/
		(CPU_CHAR*)"App Start Task", 
		(OS_TASK_PTR)App_TaskStart, (void*)0,
		(OS_PRIO)APP_TASK_START_PRIO, 
		(CPU_STK*)&AppStartTaskStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE, (OS_MSG_QTY)0u, (OS_TICK)0u,
		(void*)0, (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR*)&err_os);


	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	OSStart(&err_os); /* Inicia o funcionamento do escalonador. */
	APP_TEST_FAULT(err_os, OS_ERR_NONE);
}