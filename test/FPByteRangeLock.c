/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test60()
{
char *name ="test60 illegal fork";
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test60: illegal fork\n");

	illegal_fork(dsi, AFP_BYTELOCK, name);
}

/* ------------------------- */
static void test_bytelock(u_int16_t vol, char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		nottested();
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	if (FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100)) {
		failed();
	}
	else if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 100)) {
		failed();
	}

	FAIL (FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100))
	
	/* some Mac OS do nothing here */
	FAIL (htonl(AFPERR_PARAM) != FPByteLock(Conn, fork, 0, 0 , -1, 75)) 

	FAIL (htonl(AFPERR_NORANGE) != FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 75)) 

	FAIL ( htonl(AFPERR_RANGEOVR) != FPByteLock(Conn, fork, 0, 0 /* set */, 80 , 100)) 

	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1)
		failed();
	else {
		FAIL (htonl(AFPERR_LOCK) != FPByteLock(Conn, fork1, 0, 0 /* set */ , 20, 60))
		FAIL (FPSetForkParam(Conn, fork, len , 50)) 
		FAIL (FPSetForkParam(Conn, fork1, len , 60)) 
		FAIL (htonl(AFPERR_LOCK) != FPRead(Conn, fork1, 0, 40, Data)) 
		FAIL (htonl(AFPERR_LOCK) != FPWrite(Conn, fork1, 10, 40, Data, 0)) 
		FAIL (FPCloseFork(Conn,fork1))
	}

	/* end */
	FAIL (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 100)) 

	FAIL (htonl(AFPERR_NORANGE) != FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 50)) 

	FAIL (FPSetForkParam(Conn, fork, len , 200)) 

	if (FPByteLock(Conn, fork, 1 /* end */, 0 /* set */, 0, 100)) {
		failed();
	}
	else if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 200, 100)) {
		failed();
	}

    /* RANGEOVR */
	if (FPByteLock(Conn, fork, 0 /* end */, 0 /* set */, 0, -1)) {
		failed();
	}
	else if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, -1)) {
		failed();
	}
	FAIL (htonl(AFPERR_PARAM) != FPByteLock(Conn, fork, 0 /* start */, 0 /* set */, 0, 0)) 

	/* some Mac OS do nothing here */
	FAIL (htonl(AFPERR_PARAM) != FPByteLock(Conn, fork, 0 /* start */, 0 /* set */, 0, -2))

	if (FPCloseFork(Conn,fork)) {
		nottested();
	}
	FAIL (htonl (AFPERR_PARAM ) != FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100)) 

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		nottested();
	}

}
/* ----------- */
STATIC void test63()
{
char *name = "test63 FPByteLock DF";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test63: FPByteLock Data Fork\n");
	test_bytelock(VolID, name, OPENFORK_DATA);
	return;
}

/* ----------- */
STATIC void test64()
{
char *name = "test64 FPByteLock RF";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test64: FPByteLock Resource Fork\n");
	test_bytelock(VolID, name, OPENFORK_RSCS);
	return;
}

/* -------------------------- */
static void test_bytelock3(char *name, int type)
{
int fork;
int fork1;
u_int16_t vol = VolID;
u_int16_t bitmap = 0;
u_int16_t vol2;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);
DSI *dsi2;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		nottested();
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
		return;
	}

	FAIL (FPSetForkParam(Conn, fork, len , 50))
	FAIL (FPByteLock(Conn, fork, 0, 0 , 0 , 100))

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		goto fin;
	}

	fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1) {
		failed();
	}
	else {
		FAIL (htonl(AFPERR_LOCK) != FPRead(Conn2, fork1, 0, 40, Data)) 
		FAIL (htonl(AFPERR_LOCK) != FPWrite(Conn2, fork1, 10, 40, Data, 0))
		FAIL (htonl(AFPERR_LOCK) != FPWrite(Conn2, fork1, 55, 40, Data, 0))
		FAIL (FPSetForkParam(Conn2, fork1, len , 60)) 
		FAIL (FPWrite(Conn, fork, 55, 40, Data, 0)) 
		FAIL (htonl(AFPERR_LOCK) != FPSetForkParam(Conn2, fork1, len , 60)) 
		FAIL (htonl(AFPERR_LOCK) != FPByteLock(Conn2, fork1, 0, 0 /* set */ , 20, 60))
		FAIL (FPSetForkParam(Conn, fork, len , 200))
		FAIL (FPSetForkParam(Conn2, fork1, len ,120))
		
	}
	FAIL (FPCloseFork(Conn,fork))
	if (fork1) FPCloseFork(Conn2,fork1);

	FAIL (FPCloseVol(Conn2,vol2))
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* --------------- */
STATIC void test65()
{
char *name = "t65 DF FPByteLock 2 users";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test65: FPByteLock 2users DATA FORK\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		
    test_bytelock3(name, OPENFORK_DATA);

	name = "t65 RF FPByteLock 2 users";
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test65: FPByteLock 2users Resource FORK\n");
    test_bytelock3(name, OPENFORK_RSCS);
}

/* ---------------------------- */
static void test_bytelock2(char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
DSI *dsi2;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}
	/* fin */
	if (FPByteLock(Conn, fork, 0 /* end */, 0 /* set */, 0, -1)) {
		failed();
	}
	else {
		if (Conn->afp_version >= 30) {
			fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
			if (!fork1) {
				failed();
			}
			else {
				FAIL (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn, fork1, 0, 0, 20, 60)) 
				if (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn, fork1, 0, 0, ((off_t)1<<32)+2, 60)) {
					failed();
					FAIL (FPByteLock_ext(Conn, fork1, 0, 1, ((off_t)1<<32)+2, 60))
				}
				FPCloseFork(Conn,fork1);
			}
    		if (Conn2) {
				u_int16_t vol2;

				dsi2 = &Conn2->dsi;
				vol2  = FPOpenVol(Conn2, Vol);
				if (vol2 == 0xffff) {
					nottested();
					goto fin;
				}
				fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
				if (!fork1) {
					failed();
				}
				else {
					if (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn2, fork1, 0, 0, ((off_t)1<<32)+2, 60)) {
						failed();
						FPByteLock_ext(Conn2, fork1, 0, 1, ((off_t)1<<32)+2, 60);
					}
					FAIL (htonl(AFPERR_LOCK)  != FPWrite_ext(Conn2, fork1, ((off_t)1<<32)+2, 40, Data, 0)) 
					FAIL (FPCloseFork(Conn2,fork1))
				}
				FAIL (FPCloseVol(Conn2,vol2))
			}
		}
		FAIL (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, -1)) 
	}
	FAIL (FPCloseFork(Conn,fork))
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* -------------------------- */
void test78()
{
char *name = "t78 FPByteLock RF size -1";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test78: test Byte Lock size -1 with no large file support\n");
	test_bytelock2(name, OPENFORK_RSCS);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test78: test Byte Lock size -1 with no large file support, DATA fork\n");
	name = "t78 FPByteLock DF size -1";
	test_bytelock2(name, OPENFORK_DATA);
}

/* ----------- */
STATIC void test79()
{
int fork;
int fork1;
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int type = OPENFORK_DATA;
char *name = "t79 FPByteLock Read";
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test79: test Byte Lock and read conflict\n");
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		nottested();
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (FPSetForkParam(Conn, fork, len , 60)) {
		nottested();
	}

	FAIL (FPByteLock(Conn, fork, 0, 0, 0, 100)) 

	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1) {
		failed();
	} 
	else {
		FAIL (htonl(AFPERR_LOCK) != FPRead(Conn, fork1, 0, 40, Data)) 
		FPCloseFork(Conn,fork1);
	}

	FAIL (FPCloseFork(Conn,fork))
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		nottested();
	}
}

/* -------------------------- */
static void test_bytelock5(u_int16_t vol, char *name, int type)
{
int fork;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		failed();
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	FAIL (FPSetForkParam(Conn, fork, len , 50))

	FAIL (FPByteLock(Conn, fork, 0, 0 , 0 , 100)) 
	FAIL (FPRead(Conn, fork, 0, 40, Data))
	FAIL (FPWrite(Conn, fork, 10, 120, Data, 0)) 

	FAIL (FPByteLock(Conn, fork, 0, 1 , 0 , 100)) 
	FAIL (FPCloseFork(Conn,fork))
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		nottested();
	}
}

/* -------------------------- */
STATIC void test80()
{
char *name = "t80 RF FPByteLock Read write";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test80: Resource Fork test Byte Lock and read write same user(file)\n");
	test_bytelock5(VolID, name, OPENFORK_RSCS);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock:test80: Data Fork test Byte Lock and read write same user(file)\n");
	name = "t80 DF FPByteLock Read write";
	test_bytelock5(VolID, name, OPENFORK_DATA);
}

/* ----------- */
void FPByteRangeLock_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPByteRangeLock page 101\n");
    test60();
	test63();
	test64();
	test65();
	test78();
	test79();
	test80();
}

