#ifndef scriptManagement_H

#define scriptManagement_H

int scriptManagementInitialization(void);
int scriptManagementTermination(void);

void CVICALLBACK scriptExecution_CB (int menuBar, int menuItem, void *callbackData,
		int panel);

#endif
