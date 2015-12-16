#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	OCI_Connection * cn = NULL;

	if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
	{
		return -1;
	}

	cn = OCI_ConnectionCreate("192.168.1.139:1521/orcl", "scott", "tiger", OCI_SESSION_DEFAULT);
	
	if (cn != NULL)
	{
		printf("%s\n", OCI_GetVersionServer(cn));

		OCI_ConnectionFree(cn);
	}
	
	OCI_Cleanup();
	system("pause");
	return 0;
}

